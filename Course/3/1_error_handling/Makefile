PROGRAM = error_handling
CPPFLAGS = -Wall -Wextra
LIBS = -lGLEW -lGL -lGLU -lglfw

$(PROGRAM): src/main.cpp
	g++ src/main.cpp -o $(PROGRAM) $(CPPFLAGS) $(LIBS)

.PHONY: clean dist

clean:
	-rm *.o $(PROGRAM) *core