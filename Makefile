.PHONY: all
all: game

game: game.o test.o
	g++ -o game $^ -L. -lFrameworkRelease_x64

game.o: game.cpp
	g++ -c -Ipublic/ $<

%.o: private/%.cpp public/%.h
	g++ -c -Ipublic/ $<

.PHONY: clean
clean:
	rm -f *.o game
	