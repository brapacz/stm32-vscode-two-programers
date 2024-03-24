erase:
	cd small && make erase
	cd big && make erase

clean:
	cd small && make clean
	cd big && make clean

all:
	cd small && make all
	cd big && make all

hello:
	echo hello
