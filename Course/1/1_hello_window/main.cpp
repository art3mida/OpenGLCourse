#include <GLFW/glfw3.h>

/*
    We want this code to run on all platforms, since one of the main advantages of 
    using OpenGL is the fact that it is cross-platform. However, since creating a 
    window and an OpenGL context is not a part of the OpenGL specificaton, 
    all of it is platform-dependant. That means that we need to use platform 
    specific API, like Win32 for Windows. That's what we would do if we were creating
    a game engine, or something like that. We would take the time to set up the 
    window by ourselves, using the API for each platform that we want to support.

    For now, however, we want to focus just on learning OpenGL and how it works, so 
    we are going to use a library that manages windows for us, across platforms. One
    of those libraries is GLFW (also freeglut). These libraries are very lightweight,
    unlike SDL for example, that allows us a bunch of other functionality, too.

    You can download GLFW here:
    https://www.glfw.org/

    Now, if we were building a serious, big project, what we would want to do is to
    download the binaries, compile them and add them to our project ourselves. But again,
    we are focusing on learning OpenGL, so we are going to install precomiled binaries.
    You can download precomiled binaries for Windows/MacOS, or search for them in your
    packet management software if you are using Linux (you can do that by executing
    "apt search glfw"). 

    A few things are going to show up. You don't need all of them, so just execute:
    sudo apt install libglfw3 libglfw3-dev libglfw3-doc

    After this, you should be able to run this program, which is the example program from
    the GLFW site. When you include GLFW, don't forget to link it when compiling. You 
    do this by adding the -lglfw flag. You also need to link to libGL, which is the Linux
    implementation of the main OpenGL API entrypoints (essentially it's the Linux
    implementation of OpenGL). You do that by adding the -lGL flag.
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

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
