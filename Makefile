.PHONY: all
all: game

game: game.o Platform.o Mouse.o
	g++ -o game $^ -L. -lFrameworkRelease_x64

game.o: game.cpp
	g++ -c -Ipublic $<

%.o: private/%.cpp public/%.h
	g++ -c -Ipublic -I. $<

.PHONY: clean
clean:
	rm -f *.o game
	