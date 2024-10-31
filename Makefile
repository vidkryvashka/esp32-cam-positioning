RC_FILE = rc/index.html
OUT_FILE = src/index_html.h
SERIAL_PORT = cu.usbserial-10
CAM_LIB = lib/esp32-camera-latest

$(CAM_LIB):
	cd lib
	git clone https://github.com/espressif/esp32-camera.git
	mv esp32-camera esp32-camera-latest

all: $(CAM_LIB)
	# source /Users/user/.platformio/penv/bin/activate
	xxd -i $(RC_FILE) > $(OUT_FILE)
	pio run

flash: all
	platformio run --target upload --upload-port /dev/$(SERIAL_PORT)

