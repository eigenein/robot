TARGET_PATH := /Volumes/CIRCUITPY
LOCAL_DEVICE := $(wildcard /dev/cu.usbmodem*)
REMOTE_DEVICE := /dev/cu.JDY-31-SPP-Port

all: \
	$(TARGET_PATH)/lib \
	$(TARGET_PATH)/custom_logging.py \
	$(TARGET_PATH)/event_loop.py \
	$(TARGET_PATH)/code.py

.PHONY: $(TARGET_PATH)/lib
$(TARGET_PATH)/lib: lib
	rsync -a "lib" "$(TARGET_PATH)"

$(TARGET_PATH)/%: %
	cp "$<" "$(TARGET_PATH)/$<"

venv:
	python3.6 -m venv venv

.PHONY: monitor
local-monitor:
	@cat "$(LOCAL_DEVICE)"

.PHONY: remote-monitor
remote-monitor:
	@cat "$(REMOTE_DEVICE)"

.PHONY: repl
repl:
	@screen "$(LOCAL_DEVICE)" 115200
