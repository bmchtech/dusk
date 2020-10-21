.PHONY: clean atlas

IMGS_DIR := ../img
MAPS_DIR := ../map

IMGS_SRC := atlas0.png
IMGS_BIN := $(notdir $(IMGS_SRC:%.png=%.img.bin))
MAPS_SRC := $(shell find $(MAPS_DIR) -name '*.tmx')
MAPS_BIN := $(notdir $(MAPS_SRC:%.tmx=%.bin))

all: $(MAPS_BIN) $(IMGS_BIN)

clean:
	rm -rfv *.bin
	rm -rfv *.h
	rm -rfv atlas*

$(MAPS_BIN): $(MAPS_SRC)
	Tiled2GBA -b $< $@

$(IMGS_BIN): $(IMGS_SRC)
	grit $< -gt -p -g -gB 8 -gu32 -pu32 -Mh 2 -Mw 2 -ftb -o $@

atlas0.png:
	crunch_gen atlas "$(IMGS_DIR)" -v -p -j -s256
