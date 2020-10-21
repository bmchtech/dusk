.PHONY: clean

IMGS_DIR := ../img
MAPS_DIR := ../map

MAPS_SRC := $(shell find $(MAPS_DIR) -name '*.tmx')
MAPS_BIN := $(notdir $(MAPS_SRC:%.tmx=%.bin))

all: $(MAPS_BIN) imgs

clean:
	rm -rfv *.bin
	rm -rfv *.h
	rm -rfv atlas*

$(MAPS_BIN): $(MAPS_SRC)
	Tiled2GBA -b $< $@

imgs:
	crunch_gen atlas "$(IMGS_DIR)" -v -p -j -s256
