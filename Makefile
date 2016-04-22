all:
	g++ *.cpp -o bin/IMEBot -std=c++11

copy:
	cp bin/IMEBot ../../../.
