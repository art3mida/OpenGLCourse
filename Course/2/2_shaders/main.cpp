#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

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


    /*
        "#version 330 core" means:
        GLSL - OpenGL Shading Language
        Version 330 (OpenGL 3.3)
        Core - no deprecated funciton

        How do we access the floats in our shader?
        layout (location = 0) in vec4 position
        The 0 is the location of our attribute in the position vector. It should 
        match the location we provide in the C++ part.
        We are saying vec4, even though our vertex is characterized by only 2 
        coordinates, because it needs to be a vec4 for gl_Position anyway. We 
        are letting OpenGL cast it to vec4, instead of manually doing it ourselves
        later, when calling gl_Position.

        The main function works pretty much the same as our actual main function.

        In the fragment shader, we set the color to red, so our triangle is 
        going to be red this time.
    */

    // Creating the shaders (we are using raw string literals)
    std::string vs = R"glsl(
        #version 330 core

        layout (location = 0) in vec2 position;

        void main(){
            gl_Position = vec4(position, 0.0, 1.0);
        }
        )glsl";

    std::string fs = R"glsl(
        #version 330 core

        layout (location = 0) out vec4 color;

        void main(){
            color = vec4(1.0, 0.0, 0.0, 1.0);
        }
        )glsl";

    /*
        If you are having trouble and you are running Linux I highly recommend that you examine your glxinfo. This will provide information about the type of graphics and OpenGL that are supported for your system. 
        sudo glxinfo | grep "OpenGL" 
        this is the *unix command to check it out. 
        Try changing the version to what your system says it will support or multiple versions. 

        Unfortunately my computer does not support OpenGL Core only es at least on my Linux dual boot. I have not tried this on Windows.

        Good news is it displaying the red triangle using #version 310 es
    */

    unsigned int shader = CreateShader(vs, fs);

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