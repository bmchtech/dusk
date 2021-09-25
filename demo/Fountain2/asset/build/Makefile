.PHONY: clean atlas

IMGS_DIR := ../img
MAPS_DIR := ../map
RAWMAPS_DIR := ../rawmap
SOUND_DIR := ../snd

ATLS_SRC := $(shell find $(IMGS_DIR) -mindepth 1 -type d)
ATLS_OUT := $(patsubst %,a_%_,$(notdir $(ATLS_SRC)))
ATLS_PNG := $(ATLS_OUT:%_=%_0.png)

IMGS_BIN := $(notdir $(ATLS_PNG:%.png=%.img.bin))

RAWMAPS_SRC := $(shell find $(RAWMAPS_DIR) -name '*.png')
RAWMAPS_BIN := $(notdir $(RAWMAPS_SRC:%.png=%.bin))

MAPS_SRC := $(shell find $(MAPS_DIR) -name '*.tmx')
MAPS_BIN := $(notdir $(MAPS_SRC:%.tmx=%.bin))

SOUND_SRC := $(shell find $(SOUND_DIR) -type f)
SOUND_BIN := soundbank.bin
SOUND_H := soundbank.h

GRIT_FLAGS := -gt -p -g -gB 8 -gu32 -pu32 -ftb -fh!
CRUNCH_FLAGS := -p -j -b -s256 -p0

all: $(MAPS_BIN) $(RAWMAPS_BIN) $(IMGS_BIN) $(SOUND_BIN) $(SOUND_H)

clean:
	rm -rfv *.bin
	rm -rfv *.b
	rm -rfv *.h
	rm -rfv atl*
	rm -rfv *.png
	rm -rfv *.hash
	rm -rfv *.json

# define a function: atlpng2grit(size), this will set Mw and Mh to size
atlpng2grit = grit "$<" $(GRIT_FLAGS) -Mh $(1) -Mw $(1) -o "$@"

# default rule for atlpng -> img bin
%.img.bin: %.png
	grit "$<" $(GRIT_FLAGS) -o "$@"

# import custom recipes
include custom.mk

# rule for building atlas pngs from sprite dirs
# build a_thing_
a_%_: ../img/%
	crunch_gen "$@" "$<" $(CRUNCH_FLAGS)
# convert a_thing_ to a_thing_0.png (fake recipe)
$(ATLS_PNG): $(ATLS_OUT)
	@true

# rule for building tmx maps
%.bin: ../map/%.tmx
	Tiled2GBA -b $< $@

# rule for building raw maps
%.bin: ../rawmap/%.png
	grit "$<" -p -m -g -gB 4 -gu32 -pu32 -ftb -o "$@"

# rule for building soundbank
$(SOUND_BIN) $(SOUND_H): $(SOUND_SRC)
	# @mmutil $^ -osoundbank.bin -hsoundbank.h
	@true

# debug:
#     $(foreach var,$(.VARIABLES),$(info $(var) = $($(var))))