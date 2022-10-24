.PHONY: all
all: Arkanoid

Arkanoid: Game.o Platform.o Mouse.o WallElement.o Block.o Health.o Ball.o
	g++ -o Arkanoid $^ -L. -lFrameworkRelease_x64

Game.o: Game.cpp public/Platform.h public/Mouse.h public/WallElement.h public/Block.h public/Health.h public/Ball.h
	g++ -c -Ipublic Game.cpp

%.o: private/%.cpp public/%.h
	g++ -c -Ipublic -I. $<

.PHONY: clean
clean:
	rm -f *.o Arkanoid.exe
	