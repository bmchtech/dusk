.PHONY: clean atlas

IMGS_DIR := ../img
MAPS_DIR := ../map

ATLS_SRC := $(shell find $(IMGS_DIR) -mindepth 1 -type d)
ATLS_OUT := $(patsubst %,atlas_%_,$(notdir $(ATLS_SRC)))
ATLS_PNG := $(ATLS_OUT:%_=%_0.png)
IMGS_BIN := $(notdir $(ATLS_PNG:%.png=%.img.bin))
MAPS_SRC := $(shell find $(MAPS_DIR) -name '*.tmx')
MAPS_BIN := $(notdir $(MAPS_SRC:%.tmx=%.bin))

GRIT_FLAGS := -gt -p -g -gB 8 -gu32 -pu32 -ftb

all: $(MAPS_BIN) $(IMGS_BIN)

clean:
	rm -rfv *.bin
	rm -rfv *.h
	rm -rfv atlas*

$(MAPS_BIN): $(MAPS_SRC)
	Tiled2GBA -b $< $@

# $(IMGS_BIN): $(ATLS_PNG)
# 	grit "$<" -gt -p -g -gB 8 -gu32 -pu32 -Mh 2 -Mw 2 -ftb -o "$@"

%.img.bin: %.png
	grit "$<" $(GRIT_FLAGS) -o "$@"

atlas_chars_0.img.bin: atlas_chars_0.png
	grit "$<" $(GRIT_FLAGS) -Mh 2 -Mw 2 -o "$@"

atlas_logo_0.img.bin: atlas_logo_0.png
	grit "$<" $(GRIT_FLAGS) -Mh 8 -Mw 8 -o "$@"

$(ATLS_PNG): $(ATLS_OUT)
	@true

# $(ATLS_OUT): $(ATLS_SRC)
# 	crunch_gen $@ "$<" -v -p -j -s256

atlas_%_: ../img/%
	crunch_gen $@ "$<" -v -p -j -s256

# debug:
#     $(foreach var,$(.VARIABLES),$(info $(var) = $($(var))))