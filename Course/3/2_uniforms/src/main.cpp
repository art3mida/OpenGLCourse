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
        UNIFORMS
    Uniforms are a way for us to get data from the CPU side of things to the GPU side, 
    into our shader, so we can use it like a variable. 

    Currently (before this exercise), the color of our shape was being hardcoded in the shader.
    It would be great if we could define this on the CPU side, and manipulate it there.
    There is two main ways of doing this:
    1. VIA VERTEX BUFFER - setting attributes. They are set per vertex (very big
                            difference from uniforms)
    2. UNIFORMS - they are set per draw (before we cal glDrawElements)

    uniform vec4 u_Color;
    color = u_Color;
    Anything that begins with u_ in the shader is a uniform.
    Now we just need to set the uniform.

    You need to have the shader that is ALREADY BOUND when providing
    data for the uniform, so we know which shader do we want to affect.

    Two most important functions here are:
    - GLint glGetUniformLocation(program, name_of_uniform);
    - void glUniform4f(location, v0, v1, v2, v3);
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

    // Adjusts framerate (????)
    glfwSwapInterval(1);

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

    /*
        glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
            location - specifies the location of the uniform to be modified
    
        Before we call this, we need to retrieve the location of the variabel.
        Once our shader gets created, OpenGL assigns an ID to every uniform, whihc
        we look up by it's name (u_Color). In more modern versions of OpenGL,
        you can set and explicit uniform location (OpenGL 4.3).
    
        If a uniform is unused in a shader, it might get removed, and trying
        to retrieve it's location will return -1. Because there are reasons why
        we might have left it there unused (for later, we forgot...), we don't 
        necessarily want to terminate the program completely.
    */
    GLint location = glGetUniformLocation(shader, "u_Color");
    ASSERT(location != -1);

    float pink = 0.0f;
    float increment = 0.05f;
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        GLCall(glUniform4f(location, pink, 0.0f, pink, 1.0f));
        // If you put indices here instead of nullptr, you won't get an error,
        // but YOU WILL GET A BLACK SCREEN!
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        if(pink > 1.0f) {
            increment = -0.05f;
        }
        else if (pink < 0.0f) {
            increment = 0.05f;
        }

        pink += increment;

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // Deleting the program
    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}