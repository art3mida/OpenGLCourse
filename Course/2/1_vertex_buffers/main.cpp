/*
    Everything graphics-wise begins with a triangle. That's why we are going to 
    draw one right now, but this time using modern OpenGL, as opposed to using
    glBegin, glEnd, and glVertex2d.

    For that, we need a vertex buffer and a shader. Vertex buffer is essentially
    just a memory buffer. It's a blob of memory into which we can push buffers, but
    in our video RAM in our GPU. When we issue a draw call, the GPU reads the 
    data from the buffer, and renders it onto the screen. Additionally, we need
    to specify how those vertices will be interpreted. Since the program runs
    on CPU, the draw call will be issued there, but the GPU is the one that
    has the data, and that's the one that renders it. So we need to write a 
    program for the GPU that will tell it how to interpret the data. That's 
    why we need to write a shader. A shader is just a program that runs on GPU.

    OpenGL operates like a state machine. That means that instead of calling a 
    method to draw a triangle, and passing everything we need for said triangle,
    like we would with a method od a class, we just say "I want to draw triangles"
    from the already selected buffer and shader (all info already exists as 
    different states).
*/

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    GLFWwindow* window;

    if(!glfwInit()) {
        return -1;
    }

    window = glfwCreateWindow(640, 480, "Vertex Buffers", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if(GLEW_OK != err) {
        std::cerr << glewGetErrorString(err) << std::endl;
        return -1;
    }

    /*  
        Since our triangle is static, we can generate a buffer outside of 
        our game loop. We pass how many buffers we want, and the pointer to the 
        buffer. OpenGL is like a state machine, so every object we create can
        have a unique ID assigned to it, and be referred to by that ID all the 
        time.
        If we want to "select" a specific buffer, we just pass its ID.
    */

    unsigned int buffer;
    float positions[6] = {
        -0.5f, -0.5f,
        0.0f, 0.5f,
        0.5f, -0.5f
    };

    /* VIDEO 1 - Vertex Buffers */
    // This generates one buffer on said address.
    glGenBuffers(1, &buffer);
    // This "selects" the buffer and how we want to use it.
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    // Specifies the data. We can either provide the data right away (we are
    // going to do this now, since we already know the vertices of our
    // triangle), or just say how big the buffer is going to be, and pass NULL.
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);
    /* END OF VIDEO 1 */

    /* VIDEO 2 - Vertex Attributes and Layouts 
        OpenGL pipeline typically works in the following way:
        1. We store some data on the GPU
        2. We write a shader, which is a program that runs on the GPU, to
           read and interpret that data
        If we are drawing geometry, we are going to use vertex buffers. When
        the GPU reads from the buffer, it needs to know the layout of the buffer,
        that is, what is in the memory (it could be vertices, textures...).
        That is what is done with vertex attributes. From the shader side, we 
        need to match the layout that we provided on the C++ side.

        In our buffer, we have three vertices. Before we continue, I should mention
        what a vertex is. Vertex has nothing to do with a position. A vertex is just
        a point on our geometry. It can contain a lot more info than just the position,
        and saying our vertex is (0.5, 0.5) does not mean anything, unless we specifically
        say that this is a position. It could also contain texture coord, normals, colors,
        binormals, tangents...

        There are two main funcitons that we use for this:
        1. glVertexAttribPointer - define an array of generic vertex attribute data
            - void glVertexAttribPointer(	
                GLuint index,              -> what is the index of the attribute we are referring to
 	            GLint size,                -> number of components per vertex attribute (in our case, 2)
 	            GLenum type,               -> specifies the data type of each component in the array (GL_FLOAT here)
 	            GLboolean normalized,      -> not important for floats since they are already normalized, but for color
                                              for example ([0,255]), you can either normalize it yourself, or
                                              leave it up to OpenGL to do it by specifying true here.
 	            GLsizei stride,            -> number of bytes between each vertex (here, it would be 2*sizeof(float))
 	            const GLvoid * pointer);   -> offset of the beginning of the attribute in a single vertex. For example,
                                              if we have a vertex that is characterized by a 3D position and a
                                              2D texture. For the position, the offset is 0. Then we go 
                                              12 bytes forward, and we reach the texture coo. So the offset of 
                                              the texture attribute is 12. IT'S JUST A NUMBER.
        2. glEnableVertexArrayAttrib - enables the generic vertex attrib array specified by the index
            - Since this is a state machine, we have generated the buffer, binded the buffer and now we need to enable it.
              We can do this step any time after we have binded it. 
    */

    // Sets vertex attributes. We only have one attribute (position), so we will be
    // calling this function only once.
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    // Enables the buffer.
    glEnableVertexAttribArray(1);

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // We still need a shader, so this won't work. However, some GPU drivers
        // will provide you with a default shader, so on some systems, this might
        // work already.
        // glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}