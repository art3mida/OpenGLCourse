#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <signal.h>

#define ASSERT(x) if (!(x)) raise(SIGTRAP);

#define GLCall(x) do {\
    ClearError(); \
    x; \
    ASSERT(LogCall(#x, __FILE__, __LINE__)) \
    } while(0)
 
/*
    Dealing with errors in OpenGL, without the use of external tools,
    can be done in two main ways:
    1. glGetError - in OpenGL since the beginning. Whenever we call an OpenGL
                    funciton, if an error occurs, a flag is set somewhere in memory.
                    When we call glGetError, we can read one of the flags. You need to
                    keep calling this funciton until you've read all of the flags.

                    This is usually used by first clearing all the errors 
                    before calling something (by calling it inside of a loop),
                    calling it, and then calling glGetError again, to see if out call
                    has generated any errors.

                    Return GL_NO_ERROR on no errorm which is guaranteed to be 0.
    
    2. glDebugMessageCallback - added in OpenGL 4.3. Allows us to specify
                    a function pointer to a function that will be called
                    whenever an error occurs.

    However, clearing error codes and then checking for new error after every
    call is a bit tedious. Additionally, we don't even know which line generated the
    error (we can kind of tell, but not really). We can solve this by setting
    breakpoints manually, but we don't want to do that, we want to solve this through 
    code.

    We can define an assert, that basically lets us define a breakpoint through
    code. Assert checks whether some condition is true at the given point, and
    if it's not, it breaks. The function that we should call is debugger specific,
    so it will be different for clang, gcc, msvc... For msvc, it's __debugbreak,
    and you can tell that it's a debugger-specific function by the two underscores
    at the beginning of it.

    gcc alternative would be raise(SIGTRAP), but that doesn't return the line.
    We want to know the file and the line, too. We can do that with the __func__, 
    __FILE__, __LINE__ macros. (adding parameters to LogCall()). #x turns the function
    call x into a string of the name of the function.

    The next step if getting rid of the ClearError() call to. Essentially, we want to
    wrap each OpenGL call with a macro that handles all of this for us.

    Pros of wrapping the macro into a scope (with do-while) is that it will
    work if called inside of an if for example.
    Cons: if you have any one line declarations and initializations, they will be out 
    of scope when the GLCall if finished.
*/

static void ClearError() {
    // At this point we don't care about error codes, we are just clearing it.
    while (glGetError() != GL_NO_ERROR);
}

static bool LogCall(const char* func, const char* file, int line) {
    while(GLenum error = glGetError()) {
        std::cerr << "OpenGL error (" << error 
                  << "): In function " << func << " in file " 
                  << file << " on line " << line << std::endl;
        return false;
    }

    return true;
}

// We will return this struct when parsing shaders.
struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

ShaderProgramSource ParseShaders(const std::string& filepath) {
    std::ifstream input(filepath);
    std::string line;
    std::stringstream ss[2];

    enum class ShaderType {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    ShaderType type = ShaderType::NONE;

    // We iterate through every line in the file.  
    while (getline(input, line)) {
        // If we found a shader, we set the mode to the type of the shader we found.
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
            else {
                std::cerr << "Unrecognised shader type.\n";
            }
        }
        // Otherwise, we just append the line to the appropriate shader string stream.
        // Don't forget the \n here, otherwise shaders won't compile!
        else {
            ss[(int)type] << line << '\n'; 
        }
    }

    return {ss[0].str(), ss[1].str()};
}

GLuint CompileShader(GLenum type, const std::string& source) {
    GLuint id = glCreateShader(type);
    const char* src = source.c_str();

    /*
        glShaderSource(GLuint shader, GLsizei count, const GLchar **string, constGLint *length)
        shader - the source of our shader
        count - how many source codes are we specifying
        string - the source code
        length - passing nullptr means that the string is null-terminated, so you don't
                 need to pass the actual length
    */
    glShaderSource(id, 1, &src, nullptr);

    glCompileShader(id);

    int result;
    // i - interger
    // v - vector (in this case, it means it wants a pointer)
    // The function for checking whether there was an error with creating a shader.
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        // char message[length];
        // You can't do it like this in C++, so we will use alloca to allocate
        // dynamically on the stack.
        char* message = (char*)alloca(length * sizeof(char));

        glGetShaderInfoLog(id, length, &length, message);
        std::cerr << "Failed to compile " 
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") 
                  <<  " shader!" << std::endl;
        std::cerr << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

GLuint CreateShader(const std::string& vertexSource, const std::string& fragmentSource) {
    GLuint program = glCreateProgram();

    GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDetachShader(program, vs);
    glDetachShader(program, fs);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main() {
    GLFWwindow* window;

    // Initializing glfw library.
    if (!glfwInit()) {
        return -1;
    }

    // Creating an OpenGL context;
    window = glfwCreateWindow(640, 480, "Error Handling", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // "Selecting" that context.
    glfwMakeContextCurrent(window);

    // Initializing glew.
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << glewGetErrorString(err) << std::endl;
        glfwTerminate();
        return -1;
    }

    // Making a vertex array object.
    GLuint vao;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);

    float positions[8] = {
        -0.5, 0.5,  // 1
        0.5, 0.5,   // 2
        0.5, -0.5,  // 3
        -0.5, -0.5  // 4
    };

    unsigned int indices[6] = {
        0, 1, 2,    // first triangle
        0, 2, 3     // second triangle
    };

    // Making a vertex buffer object.
    GLuint vbo;
    glCreateBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo); 
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), positions, GL_STATIC_DRAW);

    // Set vertex attributes.
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    // Enable vertex attribute (since we only have 1 attr, we are calling this once, for
    // attribute on index 0).
    glEnableVertexAttribArray(0);

    // Creating an index buffer.
    GLuint ibo;
    glCreateBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    // Loading shader source from file. MAKE SURE THE PATH IS CORRECT, if it's not,
    // shaders won't compile.
    ShaderProgramSource source = ParseShaders("res/shaders/Basic.shader");
    std::cout << "VERTEX SHADER:" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "FRAGMENT SHADER:" << std::endl;
    std::cout << source.FragmentSource << std::endl;

    // Creating a shader.
    GLuint shader = CreateShader(source.VertexSource, source.FragmentSource);

    // Binding the shader.
    glUseProgram(shader);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // If you put indices here instead of nullptr, you won't get an error,
        // but YOU WILL GET A BLACK SCREEN!
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // Deleting the program
    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}