TARGET_PATH := /Volumes/CIRCUITPY
LOCAL_DEVICE := $(wildcard /dev/cu.usbmodem*)
REMOTE_DEVICE := /dev/cu.JDY-31-SPP-Port

all: \
	$(TARGET_PATH)/lib \
	$(TARGET_PATH)/micro_logging.py \
	$(TARGET_PATH)/micro_asyncio.py \
	$(TARGET_PATH)/peripherals.py \
	$(TARGET_PATH)/async_busio.py \
	$(TARGET_PATH)/bno055.py \
	$(TARGET_PATH)/repl.py \
	$(TARGET_PATH)/code.py

.PHONY: clean
clean:
	rm -rf "$(TARGET_PATH)/lib"
	rm $(TARGET_PATH)/*.py

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
remote-repl:
	@screen "$(REMOTE_DEVICE)" 9600

.PHONY: repl
local-repl:
	@screen "$(LOCAL_DEVICE)" 115200
