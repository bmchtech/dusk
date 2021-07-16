.PHONY: clean atlas

IMGS_DIR := ../img
MAPS_DIR := ../map
SOUND_DIR := ../snd

ATLS_SRC := $(shell find $(IMGS_DIR) -mindepth 1 -type d)
ATLS_OUT := $(patsubst %,atl_%_,$(notdir $(ATLS_SRC)))
ATLS_PNG := $(ATLS_OUT:%_=%_0.png)

IMGS_BIN := $(notdir $(ATLS_PNG:%.png=%.img.bin))

MAPS_SRC := $(shell find $(MAPS_DIR) -name '*.tmx')
MAPS_BIN := $(notdir $(MAPS_SRC:%.tmx=%.bin))

SOUND_SRC := $(shell find $(SOUND_DIR))
SOUND_BIN := soundbank.bin
SOUND_H := soundbank.h

GRIT_FLAGS := -gt -p -g -gB 8 -gu32 -pu32 -ftb

all: $(MAPS_BIN) $(IMGS_BIN) $(SOUND_BIN) $(SOUND_H)

clean:
	rm -rfv *.bin
	rm -rfv *.h
	rm -rfv atl*

%.bin: ../map/%.tmx
	Tiled2GBA -b $< $@

# $(IMGS_BIN): $(ATLS_PNG)
# 	grit "$<" -gt -p -g -gB 8 -gu32 -pu32 -Mh 2 -Mw 2 -ftb -o "$@"

%.img.bin: %.png
	grit "$<" $(GRIT_FLAGS) -o "$@"

atl_part_0.img.bin: atl_part_0.png
	grit "$<" $(GRIT_FLAGS) -Mh 1 -Mw 1 -o "$@"

atl_chars_0.img.bin: atl_chars_0.png
	grit "$<" $(GRIT_FLAGS) -Mh 2 -Mw 2 -o "$@"

atl_logo_0.img.bin: atl_logo_0.png
	grit "$<" $(GRIT_FLAGS) -Mh 8 -Mw 8 -o "$@"

atl_thred_0.img.bin: atl_thred_0.png
	grit "$<" $(GRIT_FLAGS) -Mh 8 -Mw 8 -o "$@"

atl_mus_0.img.bin: atl_mus_0.png
	grit "$<" $(GRIT_FLAGS) -Mh 8 -Mw 8 -o "$@"

$(ATLS_PNG): $(ATLS_OUT)
	@true

# $(ATLS_OUT): $(ATLS_SRC)
# 	crunch_gen $@ "$<" -v -p -j -s256

atl_%_: ../img/%
	crunch_gen $@ "$<" -v -p -j -b -s256 -p0

#---------------------------------------------------------------------------------
# rule to build soundbank from music files
#---------------------------------------------------------------------------------
$(SOUND_BIN) $(SOUND_H) : $(SOUND_SRC)
#---------------------------------------------------------------------------------
	@mmutil $^ -osoundbank.bin -hsoundbank.h

# debug:
#     $(foreach var,$(.VARIABLES),$(info $(var) = $($(var))))