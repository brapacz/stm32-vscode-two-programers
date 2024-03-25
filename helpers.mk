MCU_SERIAL := $(shell cat STLINK_SERIAL)

flash: $(BUILD_DIR)/$(TARGET).bin
	st-flash $(STFLASH_ARGS) --serial $(MCU_SERIAL) write $(BUILD_DIR)/$(TARGET).bin 0x08000000

erase:
	st-flash $(STFLASH_ARGS) --serial $(MCU_SERIAL) erase

reset:
	st-flash $(STFLASH_ARGS) --serial $(MCU_SERIAL) reset
