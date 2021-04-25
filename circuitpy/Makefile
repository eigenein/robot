TARGET_PATH := "/Volumes/CIRCUITPY"
LOCAL_DEVICE := "/dev/cu.usbmodem143201"
REMOTE_DEVICE := "/dev/cu.JDY-31-SPP-Port"

.PHONY: $(TARGET_PATH)
$(TARGET_PATH) deploy:
	@rsync -a lib $(TARGET_PATH)
	@rsync -a src $(TARGET_PATH)
	@echo "from src.__main__ import main; main()" > "$(TARGET_PATH)/code.py"

venv:
	@python3.6 -m venv venv

.PHONY: monitor
local-monitor:
	@cat "$(LOCAL_DEVICE)"

.PHONY: remote-monitor
remote-monitor:
	@cat "$(REMOTE_DEVICE)"

.PHONY: repl
repl:
	@screen "$(LOCAL_DEVICE)" 115200
