#include <GLFW/glfw3.h>

/*
    We are now going to demonstrate how to draw a triangle, using DEPRECATED methods.
    These are the methods that belong to legacy OpenGL, also known as immediate mode. 
    Legacy OpenGL is simpler, but less efficient. It's not used anymore, but since 
    it belongs to the early version of OpenGL, we are able to use it without extensions
    or additional includes. It's also the fastest way to draw a triangle on the screen 
    at this point. Since we just want to make sure our OpenGL is up and running, we 
    are going to use legacy OpenGL. It's perfectly fine to use it for debugging.
*/


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

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
