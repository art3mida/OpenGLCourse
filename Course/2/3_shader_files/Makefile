PROGRAM = shaders
CPPFLAGS = -Wall -Wextra
LIBS = -lGLEW -lGL -lGLU -lglfw 

$(PROGRAM): src/main.cpp
	g++ $(CPPFLAGS) -o $(PROGRAM) src/main.cpp $(LIBS)

.PHONY: clean dist

clean:
	-rm *.o $(PROGRAM) *core
