.PHONY: all
all: Arkanoid

Arkanoid: Game.o Platform.o Mouse.o WallElement.o
	g++ -o Arkanoid $^ -L. -lFrameworkRelease_x64

Game.o: Game.cpp
	g++ -c -Ipublic $<

%.o: private/%.cpp public/%.h
	g++ -c -Ipublic -I. $<

.PHONY: clean
clean:
	rm -f *.o Arkanoid.exe
	