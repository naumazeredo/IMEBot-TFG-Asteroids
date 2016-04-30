all:
	@mkdir -p bin
	g++ *.cpp -o bin/IMEBot -std=c++11 -O2

debug:
	@mkdir -p bin
	g++ *.cpp -o bin/IMEBot -std=c++11

copy:
	cp bin/IMEBot ../../../.
