SHELL := /bin/bash

erase:
	cd small && make erase
	cd big && make erase

reset:
	cd small && make reset
	cd big && make reset

flash:
	cd small && make flash
	cd big && make flash

clean:
	cd small && make clean
	cd big && make clean

all:
	cd small && make all
	cd big && make all

serial:
	./connect_to_serial
