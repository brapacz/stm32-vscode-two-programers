MCU_SERIAL = $(shell cat STLINK_SERIAL)
BUILD_ID = $(TARGET) $(shell date +"%Y-%m-%d %H:%M:%S") git:$(shell git rev-parse HEAD)

C_DEFS += \
-DBUILD_ID="$(BUILD_ID)"


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
build_id:
	@echo $(BUILD_ID)
