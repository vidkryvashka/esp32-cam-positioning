HTML_FRONTEND_FILE = frontend/index.html
HTML_H_FILE = include/server/index_html.h
PIO_ENV = /Users/user/.platformio/penv/bin/activate
CAMERA_LIB = lib/esp32-camera-latest
SRC = src
BUILD_DIR = .pio/build


$(HTML_H_FILE): $(HTML_FRONTEND_FILE)
	@echo "Generating $(HTML_H_FILE) from $(HTML_FRONTEND_FILE)"
	xxd -i $(HTML_FRONTEND_FILE) > $(HTML_H_FILE)

$(BUILD_DIR): $(SRC) $(HTML_H_FILE)
	platformio run


.PHONY: all build flash clean

build: $(BUILD_DIR)
	@echo "Running PlatformIO build process..."
	platformio run

all: build

flash: $(BUILD_DIR)
	@echo "Flashing project"
	platformio run --target upload --upload-port /dev/$(SERIAL_PORT)

monitor:
	platformio device monitor --port $(SERIAL_PORT)

clean:
	@echo "Cleaning up build files and html.h..."
	platformio run --target clean
	@if [ -e $(HTML_H_FILE) ]; then rm $(HTML_H_FILE); fi
	@if [ -e .pio ]; then rm -rf .pio; fi