#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

/* 
    Read this explanation when you reach the return line of the ParseShader function.
    Since we want to return multiple things, we are going to create a struct
    that contains both of them, and then create that struct when returning
*/
struct ShaderProgramSource {
    std::string VertexShader;
    std::string FragmentShader;
};

/* In a game engine, you will probably want to read files using
    C FILE API, not C++, since C is faster, but no need to focus on that
    right now.
*/
static ShaderProgramSource ParseShader(const std::string& filepath) {
    // input file stream, this opens the file
    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    // go through file line-by-line, and check for new shaders
    std::string line;
    // one of the streams is going to be for the vs, one for fs
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line)) {
        // npos is n-bound string position, that means we ended up at the end
        // of the line, aka we haven't found the "#shader" 
        if(line.find("#shader") != std::string::npos) {
            if(line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            // if we found any other "type" of line, we need to push it into the 
            // appropriate string stream
            ss[(int)type] << line << '\n';
        }
    }

    return {ss[0].str(), ss[1].str()};
}

// The code for compiling the vertex and the fragment shader is pretty much
// the same, so that's why we are abstracting it. The type reffers to the
// type of shader we are creating (GL_VERTEX_SHADER/GL_FRAGMENT_SHADER)
static int CompileShader(unsigned int type, const std::string& source) {
    // The type unsinged int is absolutely the same as GLuint, but there 
    // is no reason to use the latter, so we are just keeping it simple.
    unsigned int id = glCreateShader(type);

    // Returns the raw pointer to the string (it acts like a cast, and 
    // the returned string is immutable). Equivalent to &source[0].
    // String needs to exist at this point, that is it needs to be alive 
    // at this point, because this doesn't copy the string, it's just a 
    // pointer to it.
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

static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    // This is basically like compiling two files, that we now need to link into
    // a single program. Error is set as a state that you can check.
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    // Deletes the intermediate, object files, that you don't need once you have a 
    // finished program.
    glDetachShader(program, vs);
    glDetachShader(program, fs);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main() {
    // Setting up glfw
    GLFWwindow* window;

    if(!glfwInit()) {
        return -1;
    }

    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(640, 480, "Shaders", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Setting up glew
    //glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if(GLEW_OK != err) {
        std::cerr << glewGetErrorString(err) << std::endl;
        glfwTerminate();
        return -1;
    }

    
    float positions[6] = {
        -0.5f, -0.5f,
        0.0f, 0.5f,
        0.5f, -0.5f
    };

    /* 
       Most meshes will use a collection of one or more vertex buffer objects
       to hold vertex points, texture-coordinates, vertex normals, etc. 
       In older GL implementations we would have to bind each one, and define
       their memory layout, every time that we draw the mesh. To simplify that,
       we have new thing called the vertex array object (VAO), which remembers 
       all of the vertex buffers that you want to use, and the memory layout 
       of each one. We set up the vertex array object once per mesh. 
       When we want to draw, all we do then is bind the VAO and draw. 
    */
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    // Creating a vertex buffer
    GLuint vbo;
    // Generating the vertex array object
    glGenBuffers(1, &vbo);
    // Binding ("selecting") the buffer and how we want to use it
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Filling it up with data
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, positions, GL_STATIC_DRAW);

    // Enabling attributes
    glEnableVertexAttribArray(0);
    // Specifying vertex attributes
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0);

    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    std::cout << "VERTEX SHADER:" << std::endl;
    std::cout << source.VertexShader << std::endl;
    std::cout << "FRAGMENT SHADER:" << std::endl;
    std::cout << source.FragmentShader << std::endl;

    unsigned int shader = CreateShader(source.VertexShader, source.FragmentShader);

    // Binding our shader program.
    glUseProgram(shader);

    std::cout << glGetError() <<std::endl;
    // Main game loop
    while(!glfwWindowShouldClose(window)) {
        // glClearColor(0.0, 0.0, 0.4, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw here
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}