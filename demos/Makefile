PATH := $(DEVKITARM)/bin:$(PATH)

#  Project settings

NAME       := Fountain
SOURCE_DIR := src
LIB_DIR    := lib
DATA_DIR   := asset
SPECS      := -specs=gba.specs

# Compilation settings

CROSS	?= arm-none-eabi-
AS	:= $(CROSS)as
CC	:= $(CROSS)gcc
LD	:= $(CROSS)gcc
OBJCOPY	:= $(CROSS)objcopy

ARCH	:= -mthumb-interwork -mthumb

INCFLAGS := -I$(DEVKITPRO)/libtonc/include -I$(LIB_DIR)/gbfs/include -I$(LIB_DIR)/gbaMap/include
LIBFLAGS := -L$(DEVKITPRO)/libtonc/lib -ltonc -L$(LIB_DIR)/gbfs/lib -lgbfs -L$(LIB_DIR)/gbaMap/lib -lgbaMap
ASFLAGS	:= -mthumb-interwork
CFLAGS	:= $(ARCH) -O2 -Wall -fno-strict-aliasing $(INCFLAGS) $(LIBFLAGS)
LDFLAGS	:= $(ARCH) $(SPECS) $(LIBFLAGS)

.PHONY : build clean

# Find and predetermine all relevant source files

APP_MAIN_SOURCE := $(shell find $(SOURCE_DIR) -name '*main.c')
APP_MAIN_OBJECT := $(APP_MAIN_SOURCE:%.c=%.o)
APP_SOURCES     := $(shell find $(SOURCE_DIR) -name '*.c' ! -name "*main.c"  ! -name "*.test.c")
APP_OBJECTS     := $(APP_SOURCES:%.c=%.o)

# Build commands and dependencies

build: libs $(NAME).gba

no-content: libs $(NAME)-no_content.gba

libs:
	cd lib/gbaMap && make 
	cd lib/gbfs && make

assets:
	cd asset/build && make

$(NAME).gba : $(NAME)-no_content.gba $(NAME).gbfs
	cat $^ > $(NAME).gba

$(NAME)-no_content.gba : $(NAME).elf
	$(OBJCOPY) -v -O binary $< $@
	-@gbafix $@ -t$(NAME)
	padbin 256 $@

$(NAME).elf : $(APP_OBJECTS) $(APP_MAIN_OBJECT)
	$(LD) $^ $(LDFLAGS) -o $@

$(APP_OBJECTS) : %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(APP_MAIN_OBJECT) : $(APP_MAIN_SOURCE)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME).gbfs: assets
	gbfs $@ $(shell find $(DATA_DIR) -name '*.bin')

clean:
	@rm -fv *.gba
	@rm -fv *.elf
	@rm -fv *.sav
	@rm -fv *.gbfs
	@rm -rf $(APP_OBJECTS)
	@rm -rf $(APP_MAIN_OBJECT)
	cd lib/gbaMap && make clean
	cd lib/gbfs && make clean
	cd asset/build && make clean
