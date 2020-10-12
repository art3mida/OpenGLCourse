## Instalacija

Da bi mogli da programiramo u OpenGL-u, potrebno je imati odgovarajuće drajvere za GPU, kao i dev paket, koji zavisi od platforme koju koristimo, kao i jezika u kom programiramo.

U sve tri veće desktop platforme, OpenGL uglavnom dolazi uz sistem, ali nije loše potvrditi da imamo preuzet i instaliran skorašnji drajver za GPU.

[Detalji o instalaciji za sve sisteme.](https://www.khronos.org/opengl/wiki/Getting_Started)



## Implementacija OpenGL aplikacija

* **Izabrati jezik**
  * Vezivanja (binding - [wiki](https://en.wikipedia.org/wiki/Language_binding)) za OpenGL postoje u mnogim jezicima. Neki imaju i više OpenGL bindinga, od kojih nijedan nije oficijalan. Svi oni su bazirani na C/C++ vezivanjima.
  * Ako ne korisite C/C++, morate instalirati paket ili biblioteku koja sadrži OpenGL vezivanja. 
  * Ako koristite C/C++, onda morate napraviti build environment (Visual Studio project, GNU makefile, CMake file, ...) koji može da linkuje ka OpenGL-u. Pod Windowsom je potrebno statički linkovati ka biblioteci **OpenGL32.lib** (bez obzira da li je sistem 32- ili 64-bitni). Visual Studio, kao i većina Windows kompajlera, dolaze sa ovom bibliotekom.
  * Na Linuxu je potrebno linkovati ka **libGL.** To se radi pomoću parametra komandne linije, **-lGL**.
    * OpenGL-based programs must link with the *libGL* library. *libGL* implements the GLX interface as well as the main OpenGL API entrypoints. When using indirect rendering, *libGL* creates GLX protocol messages and sends them to the X server via a socket. When using direct rendering, *libGL* loads the appropriate 3D DRI driver then dispatches OpenGL library calls directly to that driver.
    * **GLX** ([initialism](https://en.wikipedia.org/wiki/Acronym_and_initialism) for "Open**GL** Extension to the **X** Window System") is an extension to the [X Window System core protocol](https://en.wikipedia.org/wiki/X_Window_System_core_protocol) providing an [interface](https://en.wikipedia.org/wiki/Interface_(computing)) between [OpenGL](https://en.wikipedia.org/wiki/OpenGL) and the [X Window System](https://en.wikipedia.org/wiki/X_Window_System) as well as extensions to OpenGL itself. It enables programs wishing to use OpenGL to do so within a window provided by the X Window System. GLX distinguishes two "states": indirect state and direct state.
* **Inicijalizacija**
  * Pre nego što možemo koristiti OpenGL u programu, moramo ga prvo inicijalizovati. Kako OpenGL ne zavisi od platforme, ne postoji standardan način za njegovu inicijalizaciju, već svaka platforma to radi drugačije. Ne-C/C++ vezivanja takođe ovo mogu raditi na različite načine.
  * Postoje dve faze OpenGL inicijalizacije. Prva je kreacija **OpenGL konteksta**, a druga je učitavanje svih potrebnih funkcija za korišćenje OpenGL-a. Neka ne-C/C++ vezivanja spajaju ova dva koraka u jedan.
  * **Kreiranje konteksta/prozora**
    * Kreiranje konteksta/prozora nije deo OpenGL specifikacije, pa se veoma razlikuje od platforme do platforme, kao i od bindinga do bindinga.
    * Ako koristite OpenGL, veoma je preporučljivo koristiti takozvani **window toolkit** biblioteke za rešavanje problema kreiranaj konteksta. Ove biblioteke kreiraju prozor, kače OpenGL kontekst na taj prozor i upravljaju osnovnim inputom za taj prozor. Nakon što steknete iskustvo sa OpenGL-om, možete ovo raditi i manuelno.
    * Sledeće biblioteke su osnovne i kreirane su specifično za kreiranje i upravljenje OpenGL prozorima. One takođe mogu upravljati i inputom, ali skoro ništa van toga. 
      * [**freeglut**](http://freeglut.sourceforge.net/) - A crossplatform windowing and keyboard/mouse handler. Its API is a superset of the GLUT API, and it is more stable and up to date than GLUT. It supports creating a core OpenGL context.
      * [**GLFW**](http://www.glfw.org/) - A crossplatform windowing and keyboard/mouse/joystick handler. Is more aimed for creating games. Supports Windows, Mac OS X and *nix systems. Supports creating a core OpenGL context.
      * [**GLUT**](http://en.wikipedia.org/wiki/OpenGL_Utility_Toolkit) - Veoma star, NE KORISTITI za ozbiljne projekte! The **OpenGL Utility Toolkit** (**GLUT**) is a library of utilities for OpenGL programs, which primarily perform system-level II/O with the host operating system. Functions performed include window definition, window control, and monitoring of keyboard and mouse input. Routines for drawing a number of geometric primitives (both in solid and wireframe mode) are also provided, including cubes, spheres and the [Utah teapot](https://en.wikipedia.org/wiki/Utah_teapot). GLUT also has some limited support for creating pop-up menus. The two aims of GLUT are to allow the creation of rather portable code between operating systems (GLUT is cross-platform) and to make learning OpenGL easier. Getting started with OpenGL programming while using GLUT often takes only a few lines of code and does not require knowledge of operating system–specific windowing APIs. All GLUT functions start with the `glut` prefix (for example, `glutPostRedisplay` marks the current window as needing to be redrawn).
    * Nekoliko multimedija biblioteka može da kreira OpenGL prozore, zajedno sa upravljanjem inputom, zvukom i drugim poslovima korisnim za igrice, simulacije i aplikacije slične tome:
      * [**Allegro version 5**](http://alleg.sourceforge.net/) - A cross-platform multimedia library with a C API focused on game development. Supports core OpenGL context creation. Abstractions over shader and low-level polygon drawing.
      * [**SDL**](http://www.libsdl.org/index.php) - Simple DirectMedia Layer is a cross-platform multimedia library with a C API. Supports creating a core OpenGL context. SDL is written in C, works natively with C++, and there are [bindings available](https://www.libsdl.org/languages.php) for several other languages, including C# and Python.
      * [**SFML**](http://www.sfml-dev.org/) - A cross-platform multimedia library with a C++ API. Supports creating a core OpenGL context. It is composed of five modules: system, window, graphics, audio and network.
      * [**Ecere SDK**](http://ecere.org/) - Provides rendering APIs for OpenGL, OpenGL ES and DirectX, 3D math and concepts like cameras, windowing and GUI widget library, wrapped in a compiler and IDE for its own streamlined "eC" language that cross-compiles to desktop, mobile and web platforms.
    * Postoji i mnogo **widget toolkita** koji imaju mogućnost da kreiraju OpenGL prozore, ali je njihov primarni fokus to da budu widget toolkiti (ugrađuju se u neku drugu aplikaciju).
      * [**FLTK**](http://www.fltk.org/) - A fast and light-weight cross-platform C++-based widget library that is tightly integrated with OpenGL. It includes a graphical UI editor (fluid) that makes development easy and efficient. It is used in Bjarne Stroustrup's book "Programming - Principles and Practice Using C++".
      * [**Qt**](http://qt-project.org/) - A C++ toolkit which abstracts the Linux, MacOS X and Windows away. It provides a number of OpenGL helper objects, which even abstract away the difference between desktop GL and OpenGL ES.
      * [**Game GUI**](https://github.com/sp4cerat/Game-GUI) - An OpenGL based C++ widget toolkit with skin support and integrated window manager for games.
      * [**wxWidgets**](http://www.wxwidgets.org/) -A C++ cross-platform widget toolkit.
  * **Učitavanje funkcija**
    * If you are using a non-C/C++ language binding, then the maintainer of that binding will already handle this as part of context creation. If you are using C/C++, read on.
    * In order to use OpenGL, you must get OpenGL API functions. For most libraries you are familiar with, you simply #include a header file, make sure a library is linked into your project or makefile, and it all works. OpenGL doesn't work that way.
    * For reasons that are ultimately irrelevant to this discussion, you must manually load functions via a platform-specific API call. This boilerplate work is done with various [OpenGL loading libraries](https://www.khronos.org/opengl/wiki/OpenGL_Loading_Library); these make this process smooth. You are *strongly* advised to use one.
    * If you want to do it manually however, there is a [guide as to how to load functions manually](https://www.khronos.org/opengl/wiki/Load_OpenGL_Functions). You still should use an extension loader.
    * Lista biblioteka:
      * [**GLEW**](http://glew.sourceforge.net/) - An OpenGL loading library for Windows, Linux, Mac OS X, FreeBSD, Irix, and Solaris.
      * [**gl3w**](https://github.com/skaslev/gl3w) - An OpenGL loading library, focusing on OpenGL3/4 Core context loading for Windows, Linux, Mac OS X and FreeBSD.
      * [**libepoxy**](https://github.com/anholt/libepoxy) - An OpenGL loading library which crashes with an error message instead of segfaulting if you do something wrong.
      * [**OpenGL Loader Generator**](https://bitbucket.org/alfonse/glloadgen/wiki/Home) - A tool for generating OpenGL loaders that include the exact version/extensions you want, and *only* them.
      * [**glad** - Multi-Language GL/GLES/EGL/GLX/WGL Loader-Generator based on the official specs.](https://github.com/Dav1dde/glad) - A tool (with a web-service) for generating OpenGL, OpenGL ES, EGL, GLX and WGL headers (and loaders) based on the official XML specifications.



## Korišćenje OpenGL-a

OpenGL is a rendering library. What OpenGL does not do is retain information about an "object". All OpenGL sees is a ball of triangles and a bag of state with which to render them. It does not remember that you drew a line in one location and a sphere in another.

Because of that, the general way to use OpenGL is to draw everything you need to draw, then show this image with a platform-dependent buffer swapping command. If you need to update the image, you draw everything again, even if you only need to update part of the image. If you want to animate objects moving on the screen, you need a loop that constantly clears and redraws the screen.

There are techniques for only updating a portion of the screen. And you can use OpenGL with these techniques. But OpenGL itself doesn't do it internally; *you* must remember where you drew everything. You must figure out what needs updating and clear only that part of the screen. And so forth.

There are many [tutorials and other materials](https://www.khronos.org/opengl/wiki/Getting_Started#Tutorials_and_How_To_Guides) available for learning how to use OpenGL, both on this wiki and online.



[Other references](https://www.khronos.org/opengl/wiki/Related_toolkits_and_APIs#Context.2FWindow_Toolkits)

