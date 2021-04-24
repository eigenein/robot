TARGET_PATH := "/Volumes/CIRCUITPY"
DEVICE := "/dev/cu.usbmodem143201"

deploy: $(TARGET_PATH)/lib $(TARGET_PATH)/src $(TARGET_PATH)/code.py

venv:
	@python3.6 -m venv venv

.PHONY: $(TARGET_PATH)/lib
$(TARGET_PATH)/lib:
	@rsync -a lib $(TARGET_PATH)

.PHONY: $(TARGET_PATH)/src
$(TARGET_PATH)/src:
	@rsync -a src $(TARGET_PATH)

.PHONY: $(TARGET_PATH)/code.py
$(TARGET_PATH)/code.py:
	@echo "from src.__main__ import main; main()" > "$(TARGET_PATH)/code.py"

.PHONY: monitor
monitor:
	@cat "$(DEVICE)"

.PHONY: repl
repl:
	@screen "$(DEVICE)" 115200
