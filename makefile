all:
	g++ -std=c++20 src/main.cpp -o klondike

clean:
	rm -f klondike *.o
