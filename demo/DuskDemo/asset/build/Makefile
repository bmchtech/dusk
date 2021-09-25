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

%.bin: ../map/%.tmx
	Tiled2GBA -b $< $@

%.bin: ../rawmap/%.png
	grit "$<" -p -m -g -gB 4 -gu32 -pu32 -ftb -o "$@"

# $(IMGS_BIN): $(ATLS_PNG)
# 	grit "$<" -gt -p -g -gB 8 -gu32 -pu32 -Mh 2 -Mw 2 -ftb -o "$@"


# define a function: atlpng2grit(size), this will set Mw and Mh to size
atlpng2grit = grit "$<" $(GRIT_FLAGS) -Mh $(1) -Mw $(1) -o "$@"

# default rule for atlpng -> img bin
%.img.bin: %.png
	grit "$<" $(GRIT_FLAGS) -o "$@"

# import custom atlas grit recipes
include custom.mk

$(ATLS_PNG): $(ATLS_OUT)
	@true

# $(ATLS_OUT): $(ATLS_SRC)
# 	crunch_gen $@ "$<" -v -p -j -s256

a_%_: ../img/%
	crunch_gen $@ "$<" $(CRUNCH_FLAGS)

#---------------------------------------------------------------------------------
# rule to build soundbank from music files
#---------------------------------------------------------------------------------
$(SOUND_BIN) $(SOUND_H) : $(SOUND_SRC)
#---------------------------------------------------------------------------------
	@mmutil $^ -osoundbank.bin -hsoundbank.h

# debug:
#     $(foreach var,$(.VARIABLES),$(info $(var) = $($(var))))