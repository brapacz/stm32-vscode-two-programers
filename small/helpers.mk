flash: $(BUILD_DIR)/$(TARGET).bin
	st-flash --reset write $(BUILD_DIR)/$(TARGET).bin 0x08000000
