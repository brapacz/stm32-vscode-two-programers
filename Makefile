SHELL := /bin/bash
SMALL_UART_DEVICE ?= $(shell cat config.yaml | yq -r .small.uart.device)
BIG_UART_DEVICE ?= $(shell cat config.yaml | yq -r .big.uart.device)
TEST_LOG_LEVEL ?= $(shell cat config.yaml | yq -r .test.log_level)

.PHONY: erase
erase:
	@cd small && make erase
	@cd big && make erase

.PHONY: reset
reset:
	@cd small && make reset
	@cd big && make reset

.PHONY: flash
flash:
	@cd small && make flash
	@cd big && make flash

.PHONY: clean
clean:
	@cd small && make clean
	@cd big && make clean

.PHONY: all
all:
	@cd small && make all
	@cd big && make all

.PHONY: show-co
show-config:
	@cd small && make show-config
	@cd big && make show-config

.PHONY: serial
serial:
	@echo "TIP: you can use \`make serial\` command in each big/small directory to use already devined port"
	@echo "  defined for small: $(SMALL_UART_DEVICE)"
	@echo "  defined for big: $(BIG_UART_DEVICE)"
	@echo
	@./connect_to_serial

.PHONY: test
test: test-setup
	env LOG_LEVEL="$(TEST_LOG_LEVEL)" SMALL_UART_DEVICE=$(SMALL_UART_DEVICE) BIG_UART_DEVICE=$(BIG_UART_DEVICE) python3 -m unittest discover ./test -p '*.py' ${TESTOPTS}

.PHONY: test-setup
test-setup:
	pip3 install -r ./test/requirements.txt

config.yaml:
	@if [ ! -f "$@" ]; then \
		echo 1>&2 "File $@ is missing, check out $@.example and create it manually."; \
		false; \
	fi

%: config.yaml %.in
	@jinja -d config.yaml $@.in > $@ && echo generated $@ from $@.in

.PHONY: render-files
FILES_TO_RENDER:=$(shell find -iname '*.in' | rev | cut -c 4- | rev)
render-files: $(FILES_TO_RENDER)

.PHONY: setup-jinja
setup-jinja:
	@which jinja > /dev/null || pip3 install jinja-cli
