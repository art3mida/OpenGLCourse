PROGRAM = shaders
CPPFLAGS = -Wall -Wextra
LIBS = -lGLEW -lGL -lGLU -lglfw 

$(PROGRAM): main.cpp
	g++ $(CPPFLAGS) -o $(PROGRAM) main.cpp $(LIBS)

.PHONY: clean dist

clean:
	-rm *.o $(PROGRAM) *core
