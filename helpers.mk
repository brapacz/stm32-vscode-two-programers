flash: $(BUILD_DIR)/$(TARGET).bin
	st-flash --serial $(MCU_SERIAL) write $(BUILD_DIR)/$(TARGET).bin 0x08000000

erase:
	st-flash --serial $(MCU_SERIAL) erase

reset:
	st-flash --serial $(MCU_SERIAL) reset
