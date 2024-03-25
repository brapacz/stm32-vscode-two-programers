MCU_SERIAL = $(shell cat STLINK_SERIAL)

flash: $(BUILD_DIR)/$(TARGET).bin _stlink-serial
	st-flash $(STFLASH_ARGS) --serial $(MCU_SERIAL) write $(BUILD_DIR)/$(TARGET).bin 0x08000000

erase: _stlink-serial
	st-flash $(STFLASH_ARGS) --serial $(MCU_SERIAL) erase

reset: _stlink-serial
	st-flash $(STFLASH_ARGS) --serial $(MCU_SERIAL) reset

_stlink-serial:
	@if [ ! -f STLINK_SERIAL ]; then \
		echo "STLINK_SERIAL file is missing. Create it in $(shell pwd)/STLINK_SERIAL and put one of the following serial numbers:"; \
		st-info --probe; \
		false; \
	fi
