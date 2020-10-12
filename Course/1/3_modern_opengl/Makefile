PROGRAM = hello_glew
CPPFLAGS = -Wall -Wextra
LIBS = -lGLEW -lGL -lGLU -lglfw 

$(PROGRAM): main.o
	g++ $(CPPFLAGS) -o $(PROGRAM) main.o $(LIBS)

.PHONY: clean dist

clean:
	-rm *.o $(PROGRAM) *core
