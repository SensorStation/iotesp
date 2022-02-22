all:
	idf.py flash monitor

build:
	idf.py build

flash:
	idf.py flash

monitor:
	idf.py monitor

.PHONY: all build flash monitor
