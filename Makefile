RC_FILE = rc/index.html
OUT_FILE = src/index_html.h
SERIAL_PORT = cu.usbserial-10

all:
	# source /Users/user/.platformio/penv/bin/activate
	xxd -i $(RC_FILE) > $(OUT_FILE)
	pio run

flash: all
	platformio run --target upload --upload-port /dev/$(SERIAL_PORT)