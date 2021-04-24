TARGET_PATH := "/Volumes/CIRCUITPY"
LOCAL_DEVICE := "/dev/cu.usbmodem143201"
REMOTE_DEVICE := "/dev/cu.JDY-31-SPP-Port"

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
local-monitor:
	@cat "$(LOCAL_DEVICE)"

.PHONY: remote-monitor
remote-monitor:
	@cat "$(REMOTE_DEVICE)"

.PHONY: repl
local-repl:
	@screen "$(LOCAL_DEVICE)" 115200
