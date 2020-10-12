// Include glew before any other OpenGL things!!!!!!!!
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

/*  
    If you leave this out, and compile it, you might get a linking error or
    no error at all, but the program won't work. If you go into the glew.h
    file, and find the glewInit() function, you will see there is a macro before
    the return value. By default it uses the one that goes with using a dynamic
    library, but since we are not using that, we need to define the following macro.
*/
    


/*
    Glew by default includes gl.h, which are the basic, OpenGL 1.1 functions that come
    with the system. Those are not the functions we want to use, since they are both
    deprecated, and not as efficient as other methods. 

    If we want to use newer functions (anything after OpenGL 1.1) we need to get them 
    from somewhere. However, the functions we need are implemented in our GPU drivers,
    so it's not like we are going to download them from somewhere. In order to use them,
    we need to get into those drivers, get the function declarations, and then link
    against the functions as well. So we need to access the driver.dll (on Win) and
    retrieve function pointers to the functions inside these libraries.

    There are a lot of functions for this. Accessing specific drivers is not 
    cross-platform. Also, doing this manually is extremenly tedious. So we are going
    to use a library for this, that provides the OpenGL function declarations, constants,
    etc in the .h file, and the .c files goes into the appropriate driver and gets the 
    function pointer. Just to reiterate, the libraries that we are going to use
    DO NOT implement the functions, they just access the functions that are already
    on our computer, in binary form.

    We are gonna use the GLEW library for this (OpenGL Extension Wrangler). 
    Another one that we could use is GLAD. GLEW is a little bit simpler, so 
    we are using that one.
*/


int main()
{
    GLFWwindow* window;

    
   if (!glfwInit()) {
       return -1;
   }

    /* 
        It is ESSENTIAL to initialize a valid OpenGL context before we can call
        glewInit() to initialize the extension entry points. Since glfwInit()
        only initializes the library, we can't yet call glewInit. We need to
        initialize the context with the following funciton.
    */

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "GLEW practice", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }


    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /*    
        If glewInit() returns GLEW_OK, the initialization succeeded and we can 
        use the available extensions, as well as core OpenGL functionality.
    */
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << glewGetErrorString(err) << std::endl;
        return -1;
    }

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Drawing a trianle */
        glBegin(GL_TRIANGLES);
        glVertex2d(-0.5f, -0.5f);
        glVertex2d(0.0f, 0.5f);
        glVertex2d(0.5f, -0.5f);
        glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}