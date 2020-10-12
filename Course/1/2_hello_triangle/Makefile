PROGRAM = hello_triangle
CPPFLAGS = -Wall -Wextra
LIBS = -lglfw -lGL

$(PROGRAM): main.o
	g++ $(CPPFLAGS) -o $(PROGRAM) main.o $(LIBS)

.PHONY: clean dist

clean:
	-rm *.o $(PROGRAM) *core
