SHELL := /bin/bash
SMALL_UART_DEVICE = $(shell cat config.yml | yq -r .small.uart.device)
BIG_UART_DEVICE = $(shell cat config.yml | yq -r .big.uart.device)

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

test: test-setup
	python3 -m unittest discover ./test -p '*.py'

test-setup:
	pip3 install -r ./test/requirements.txt
