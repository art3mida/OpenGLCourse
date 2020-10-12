You're mixing up 3 different things here:

1. OpenGL
2. the GL Utilities (GLU) which are not part of OpenGL
3. and the GL Extension Wrangler (GLEW)

GLEW and GLU are completely different things and you can not replace one with another.

`GL/gl.h` are the base OpenGL headers, which give you OpenGL-1.1 function and token declarations, any maybe more. For anything going beyond version 1.1 you must use the OpenGL extension mechanism. Since this is a boring and tedious task, that has been automatized by the GLEW project, which offer all the dirty details packed up in a easy to use library. The declarations of this library are found in the header file `GL/glew.h`. Since OpenGL extensions don't make sense without basic OpenGL, the GLEW header implicitly includes the regular OpenGL header, so as of including `GL/glew.h` you no longer need to include `GL/gl.h`.

Then there's GLU, a set of convenience methods, which BTW are seriously outdated and should not be used in any modern OpenGL program. There's no modern GLU, so just forget about it. Anyway, it's declarations are made available by the header `GL/glu.h` (the one you were asking about).

The errors you get *have nothing to do with include files* though. Those are linker errors. Just including the declarations is only half of the job. The other half is linking the actual *definitions* and those are not in the header by the library file; `libglew.so` or `libglew.a` on a *nix OS, `glew.lib` or `glew32.lib` or `glews.lib` or `glew32s.lib` on Windows. If not using the static versions (those without the 's') you also must have installed the right DLL.

So to use GLEW you need to include the header **and** add it to the list of libraries in the linker options. Also you must call `glewInit();` once you've obtained a OpenGL context in your program.





* **gl**: This is the base header file for OpenGL version 1.1. That means if you want to use any functionality beyond version 1.1, you have to add any extension library on this.
* **glew**: OpenGL Extension Wrangler Library. This is a cross-platform library for loading OpenGL extended functionality. When you initialize this library, it will check your platform and graphic card at run-time to know what functionality can be used in your program.
* **glu**: This is OpenGL utilities library, which has been not updated for long time. Don't need to use this header file.
* **glut**: OpenGL Utility Toolkit for Windowing API. This is good for small to medium size OpenGL program. If you need more sophisticated windowing libraries, use native window system toolkits like GTK or Qt for linux machines.
* **glfw**: OpenGL Frame Work. Another multi-platform library for creating windows and handling events. FreeGlut can be used as an alternative. glfw is designed for game development.
* **glm**: OpenGL Mathematics. It helps implementing vectors and matrices operations.





### OpenGL library on Linux (https://learnopengl.com/Getting-started/Creating-a-window)

On Linux systems you need to link to the `libGL.so` library by adding `-lGL` to your linker settings. If you can't find the library you probably need to install any of the Mesa, NVidia or AMD dev packages.

Then, once you've added both the GLFW and OpenGL library to the linker settings you can include the header files for GLFW as follows:

```
#include <GLFW\glfw3.h>
```

For Linux users compiling with GCC, the following command line options may help you compile the project: `-lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl`. Not correctly linking the corresponding libraries will generate many *undefined reference* errors.



#  Creating a window

The first thing we need to do before we start creating stunning graphics is to create an OpenGL context and an application window to draw in. However, those operations are specific per operating system and OpenGL purposefully tries to abstract itself from these operations. This means we have to create a window, define a context, and handle user input all by ourselves.

Luckily, there are quite a few libraries out there that provide the functionality we seek, some specifically aimed at OpenGL. Those libraries save us all the operation-system specific work and give us a window and an OpenGL context to render in. Some of the more popular libraries are GLUT, SDL, SFML and GLFW. On LearnOpenGL we will be using **GLFW**. Feel free to use any of the other libraries, the setup for most is similar to GLFW's setup.

## GLFW

GLFW is a library, written in C, specifically targeted at OpenGL. GLFW gives us the bare necessities required for rendering goodies to the screen. It allows us to create an OpenGL context, define window parameters, and handle user input, which is plenty enough for our purposes.

The focus of this and the next chapter is to get GLFW up and running, making sure it properly creates an OpenGL context and that it displays a simple window for us to mess around in. This chapter takes a step-by-step approach in retrieving, building and linking the GLFW library. We'll use Microsoft Visual Studio 2019 IDE as of this writing (note that the process is the same on the more recent visual studio versions). If you're not using Visual Studio (or an older version) don't worry, the process will be similar on most other IDEs.

## Building GLFW

GLFW can be obtained from their webpage's [download](http://www.glfw.org/download.html) page. GLFW already has pre-compiled binaries and header files for Visual Studio 2012 up to 2019, but for completeness' sake we will compile GLFW ourselves from the source code. This is to give you a feel for the process of compiling open-source libraries yourself as not every library will have pre-compiled binaries available. So let's download the *Source package*.

We'll be building all libraries as 64-bit binaries so make sure to get the 64-bit binaries if you're using their pre-compiled binaries.

Once you've downloaded the source package, extract it and open its content. We are only interested in a few items:

- The resulting library from compilation.
- The **include** folder.

Compiling the library from the source code guarantees that the resulting library is perfectly tailored for your CPU/OS, a luxury pre-compiled binaries don't always provide (sometimes, pre-compiled binaries are not available for your system). The problem with providing source code to the open world however is that not everyone uses the same IDE or build system for developing their application, which means the project/solution files provided may not be compatible with other people's setup. So people then have to setup their own project/solution with the given .c/.cpp and .h/.hpp files, which is cumbersome. Exactly for those reasons there is a tool called CMake.

### CMake

CMake is a tool that can generate project/solution files of the user's choice (e.g. Visual Studio, Code::Blocks, Eclipse) from a collection of source code files using pre-defined CMake scripts. This allows us to generate a Visual Studio 2019 project file from GLFW's source package which we can use to compile the library. First we need to download and install CMake which can be downloaded on their [download](http://www.cmake.org/cmake/resources/software.html) page.

Once CMake is installed you can choose to run CMake from the command line or through their GUI. Since we're not trying to overcomplicate things we're going to use the GUI. CMake requires a source code folder and a destination folder for the binaries. For the source code folder we're going to choose the root folder of the downloaded GLFW source package and for the build folder we're creating a new directory *build* and then select that directory.

![Image of CMake's logo](https://learnopengl.com/img/getting-started/cmake.png)

Once the source and destination folders have been set, click the `Configure` button so CMake can read the required settings and the source code. We then have to choose the generator for the project and since we're using Visual Studio 2019 we will choose the `Visual Studio 16` option (Visual Studio 2019 is also known as Visual Studio 16). CMake will then display the possible build options to configure the resulting library. We can leave them to their default values and click `Configure` again to store the settings. Once the settings have been set, we click `Generate` and the resulting project files will be generated in your `build` folder.

### Compilation

In the `build` folder a file named `GLFW.sln` can now be found and we open it with Visual Studio 2019. Since CMake generated a project file that already contains the proper configuration settings we only have to build the solution. CMake should've automatically configured the solution so it compiles to a 64-bit library; now hit build solution. This will give us a compiled library file that can be found in `build/src/Debug` named `glfw3.lib`.

Once we generated the library we need to make sure the IDE knows where to find the library and the include files for our OpenGL program. There are two common approaches in doing this:

1. We find the `/lib` and `/include` folders of the IDE/compiler and add the content of GLFW's `include` folder to the IDE's `/include` folder and similarly add `glfw3.lib` to the IDE's `/lib` folder. This works, but it's is not the recommended approach. It's hard to keep track of your library and include files and a new installation of your IDE/compiler results in you having to do this process all over again.
2. Another approach (and recommended) is to create a new set of directories at a location of your choice that contains all the header files/libraries from third party libraries to which you can refer to from your IDE/compiler. You could, for instance, create a single folder that contains a `Libs` and `Include` folder where we store all our library and header files respectively for OpenGL projects. Now all the third party libraries are organized within a single location (that can be shared across multiple computers). The requirement is, however, that each time we create a new project we have to tell the IDE where to find those directories.

Once the required files are stored at a location of your choice, we can start creating our first OpenGL GLFW project.



## Our first project

First, let's open up Visual Studio and create a new project. Choose C++ if multiple options are given and take the `Empty Project` (don't forget to give your project a suitable name). Since we're going to be doing everything in 64-bit and the project defaults to 32-bit, we'll need to change the dropdown at the top next to Debug from x86 to x64:

![Image of how to switch from x86 to x64](https://learnopengl.com/img/getting-started/x64.png)

Once that's done, we now have a workspace to create our very first OpenGL application!

## Linking

In order for the project to use GLFW we need to link the library with our project. This can be done by specifying we want to use `glfw3.lib` in the linker settings, but our project does not yet know where to find `glfw3.lib` since we store our third party libraries in a different directory. We thus need to add this directory to the project first.

We can tell the IDE to take this directory into account when it needs to look for library and include files. Right-click the project name in the solution explorer and then go to `VC++ Directories` as seen in the image below:

![Image of Visual Studio's VC++ Directories configuration](https://learnopengl.com/img/getting-started/vc_directories.png)

From there on out you can add your own directories to let the project know where to search. This can be done by manually inserting it into the text or clicking the appropriate location string and selecting the `<Edit..>` option. Do this for both the `Library Directories` and `Include Directories`:

![Image of Visual Studio's Include Directories configuration](https://learnopengl.com/img/getting-started/include_directories.png)

Here you can add as many extra directories as you'd like and from that point on the IDE will also search those directorie when searching for library and header files. As soon as your `Include` folder from GLFW is included, you will be able to find all the header files for GLFW by including `<GLFW/..>`. The same applies for the library directories.

Since VS can now find all the required files we can finally link GLFW to the project by going to the `Linker` tab and `Input`:

![Image of Visual Studio's link configuration](https://learnopengl.com/img/getting-started/linker_input.png)

To then link to a library you'd have to specify the name of the library to the linker. Since the library name is `glfw3.lib`, we add that to the `Additional Dependencies` field (either manually or using the `<Edit..>` option) and from that point on GLFW will be linked when we compile. In addition to GLFW we should also add a link entry to the OpenGL library, but this may differ per operating system:

### OpenGL library on Windows

If you're on Windows the OpenGL library `opengl32.lib` comes with the Microsoft SDK, which is installed by default when you install Visual Studio. Since this chapter uses the VS compiler and is on windows we add `opengl32.lib` to the linker settings. Note that the 64-bit equivalent of the OpenGL library is called `opengl32.lib`, just like the 32-bit equivalent, which is a bit of an unfortunate name.

### OpenGL library on Linux

On Linux systems you need to link to the `libGL.so` library by adding `-lGL` to your linker settings. If you can't find the library you probably need to install any of the Mesa, NVidia or AMD dev packages.

Then, once you've added both the GLFW and OpenGL library to the linker settings you can include the header files for GLFW as follows:

```
#include <GLFW\glfw3.h>
```

For Linux users compiling with GCC, the following command line options may help you compile the project: `-lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl`. Not correctly linking the corresponding libraries will generate many *undefined reference* errors.

This concludes the setup and configuration of GLFW.

## GLAD

We're still not quite there yet, since there is one other thing we still need to do. Because OpenGL is only really a standard/specification it is up to the driver manufacturer to implement the specification to a driver that the specific graphics card supports. Since there are many different versions of OpenGL drivers, the location of most of its functions is not known at compile-time and needs to be queried at run-time. It is then the task of the developer to retrieve the location of the functions he/she needs and store them in function pointers for later use. Retrieving those locations is [OS-specific](https://www.khronos.org/opengl/wiki/Load_OpenGL_Functions). In Windows it looks something like this:

```
// define the function's prototype
typedef void (*GL_GENBUFFERS) (GLsizei, GLuint*);
// find the function and assign it to a function pointer
GL_GENBUFFERS glGenBuffers  = (GL_GENBUFFERS)wglGetProcAddress("glGenBuffers");
// function can now be called as normal
unsigned int buffer;
glGenBuffers(1, &buffer);
```

As you can see the code looks complex and it's a cumbersome process to do this for each function you may need that is not yet declared. Thankfully, there are libraries for this purpose as well where **GLAD** is a popular and up-to-date library.

### Setting up GLAD

GLAD is an [open source](https://github.com/Dav1dde/glad) library that manages all that cumbersome work we talked about. GLAD has a slightly different configuration setup than most common open source libraries. GLAD uses a [web service](http://glad.dav1d.de/) where we can tell GLAD for which version of OpenGL we'd like to define and load all relevant OpenGL functions according to that version.

Go to the GLAD [web service](http://glad.dav1d.de/), make sure the language is set to C++, and in the API section select an OpenGL version of at least 3.3 (which is what we'll be using; higher versions are fine as well). Also make sure the profile is set to *Core* and that the *Generate a loader* option is ticked. Ignore the extensions (for now) and click *Generate* to produce the resulting library files.

GLAD by now should have provided you a zip file containing two include folders, and a single `glad.c` file. Copy both include folders (`glad` and `KHR`) into your include(s) directoy (or add an extra item pointing to these folders), and add the `glad.c` file to your project.

After the previous steps, you should be able to add the following include directive above your file:

```
#include <glad/glad.h> 
```

Hitting the compile button shouldn't give you any errors, at which point we're set to go for the [next](https://learnopengl.com/Getting-started/Hello-Window) chapter where we'll discuss how we can actually use GLFW and GLAD to configure an OpenGL context and spawn a window. Be sure to check that all your include and library directories are correct and that the library names in the linker settings match the corresponding libraries.





## RANDOM

+ runtime environment vs development environment
  + when you do apt search for the glew library for example, you are going to get libglew2.1 and libglew-dev, where the first one is the runtime environment, and the second one is the development environment