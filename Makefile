RC_FILE = rc/index.html
OUT_FILE = src/index_html.h

all:
	# source /Users/user/.platformio/penv/bin/activate
	xxd -i $(RC_FILE) > $(OUT_FILE)
	pio run