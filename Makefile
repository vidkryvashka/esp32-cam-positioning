HTML_RC_FILE = rc/index.html
HTML_H_FILE = src/index_html.h
SERIAL_PORT = cu.usbserial-10
PIO_ENV = /Users/user/.platformio/penv/bin/activate
CAMERA_LIB = lib/esp32-camera-latest
SRC = src
BUILD_DIR = .pio/build


$(CAMERA_LIB):
	@echo "executing camera lib works"
	cd lib && \
	git clone https://github.com/espressif/esp32-camera.git && \
	mv esp32-camera esp32-camera-latest

$(HTML_H_FILE): $(HTML_RC_FILE)
	@echo "Generating $(HTML_H_FILE) from $(HTML_RC_FILE)"
	xxd -i $(HTML_RC_FILE) > $(HTML_H_FILE)

$(BUILD_DIR): $(SRC)

all: $(CAMERA_LIB) $(HTML_H_FILE)
	platformio run

build: $(CAMERA_LIB) $(HTML_H_FILE) $(BUILD_DIR)
	@echo "Building project"
	platformio run

flash: $(BUILD_DIR)
	@echo "Flashing project"
	platformio run --target upload --upload-port /dev/$(SERIAL_PORT)