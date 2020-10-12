#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

/*
    Triangle is a basic primitive in computer graphics. That means that we will use
    a triangle, or many triangles, to draw everything we need to draw. When drawing a
    square, we will draw it by drawing two adjacent triangles:
     _        _  
    |_|  =   |/ +  /_|
    
    But by doing this, some of our vertices will be saved in the memory twice. This
    is wasteful, and we san solve this by using index buffers. That allows us to reuse
    existing vertices. This might not seem wasteful here, since we only have 2 triangles,
    with vertices characterized only by positions, even though it's already 50% more memory
    than we actually need. Now, imagine having a huge 3D model, with vertices that contain
    texture coordinates, normals, binormals, tangents... It starts to get very inefficient.
    Referring to a vertex just by it's index would be a lot more efficient.

    Creating an index buffer works the same as anything else in OpenGL, but when binging it,
    we will bind it as GL_ELEMENT_ARRAY_BUFFER, and not with GL_ARRAY_BUFFER as we did when 
    using vertex buffers. Then, we will draw using the index buffer with:
    glDrawELements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)
        count (6) - number of indices (not vertices) we will be drawing
        type - type of data that is in the index buffer
        pointer - pointer to the index buffer (we don't need to set anything here, 
                    since we have the index buffer bound in the GL_ELEMENT_ARRAY_BUFFER)

    This approach is how most of the stuff is drawn, even in AAA games. glDrawElements is
    the way to go.
*/

struct ShaderProgramSource {
    std::string VertexShader;
    std::string FragmentShader;
};

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

    /* Recreating this using an index buffer.     
    float positions[12] = {
        -0.5f, -0.5f,
        0.5f, -0.5f,
        0.5f, 0.5f,

        0.5f, 0.5f,
        -0.5f, 0.5f,
        -0.5f, -0.5f
    };
    */

   float positions[12] = {
        -0.5f, -0.5f,   // 0
        0.5f, -0.5f,    // 1
        0.5f, 0.5f,     // 2
        -0.5f, 0.5f,    // 3
    };

    unsigned int indices[] {
        0, 1, 2,    // first triangle
        2, 3, 0     // second triangle
    };

    /* 
       Vertex array buffer 
    */
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 2, positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0);

    /*
        Creating and index buffer object - it's basically the same as creating a vbo,
        except when binding it, we bind it as GL_ELEMENT_ARRAY_BUFFER, 
    */
    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indices, GL_STATIC_DRAW);


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
        // If you use GL_INT here, that is an invalid enum for this object, so
        // that is gonna mess everything up, but we are not gonna get any
        // error in the console!
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}