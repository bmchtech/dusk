#
# Makefile for libdusk.
#

LIBNAME		:= dusk
VERSION		:= 0.1.7

#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif
ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro)
endif
include $(DEVKITARM)/gba_rules

BUILD		:=	build
SRCDIRS		:=	src src/contrib
INCDIRS		:=	include include/contrib

DEPFLAGS	:= -I$(DEVKITPRO)/libtonc/include -L$(DEVKITPRO)/libtonc/lib -ltonc

DATESTRING	:=	$(shell date +%Y)$(shell date +%m)$(shell date +%d)
GITVERSION	:=	$(shell git describe --long --tags | sed 's/^v//;s/\([^-]*-g\)/r\1/;s/-/./g')

ARCH		:=	-mthumb -mthumb-interwork
RARCH		:= -mthumb-interwork -mthumb
IARCH		:= -mthumb-interwork -marm

bTEMPS		:= 0	# Save gcc temporaries (.i and .s files)
# DEBUG		:= 0	# Generate debug info

#---------------------------------------------------------------------------------
# Options for code generation
#---------------------------------------------------------------------------------

CBASE   := $(INCLUDE) -Wall -fno-strict-aliasing #-fno-tree-loop-optimize
CBASE	+= -O2
CBASE	+= $(DEPFLAGS)
CBASE	+= -Werror=implicit-function-declaration

ifeq ($(DEBUG),1)
	CBASE	+= -g -DDEBUG
endif

DEFINES = -DGBA -DDUSK_VERSION=\"$(GITVERSION)\"
CBASE	+= $(DEFINES)

RCFLAGS := $(CBASE) $(RARCH)
ICFLAGS := $(CBASE) $(IARCH) -mlong-calls #-fno-gcse
CFLAGS  := $(RCFLAGS)

ASFLAGS := $(INCLUDE) -Wa,--warn $(ARCH)

# --- Save temporary files ? ---
ifeq ($(strip $(bTEMPS)), 1)
	RCFLAGS  += -save-temps
	ICFLAGS  += -save-temps
	CFLAGS	 += -save-temps
	CXXFLAGS += -save-temps
endif

#---------------------------------------------------------------------------------
# Path to tools - this can be deleted if you set the path in windows
#---------------------------------------------------------------------------------

export PATH		:=	$(DEVKITARM)/bin:$(PATH)

#---------------------------------------------------------------------------------

ifneq ($(BUILD),$(notdir $(CURDIR)))

export TARGET	:=	$(CURDIR)/lib/lib$(LIBNAME).a

export VPATH	:=	$(foreach dir,$(DATADIRS),$(CURDIR)/$(dir)) $(foreach dir,$(SRCDIRS),$(CURDIR)/$(dir))

ICFILES		:=	$(foreach dir,$(SRCDIRS),$(notdir $(wildcard $(dir)/*.iwram.c)))
RCFILES		:=	$(foreach dir,$(SRCDIRS),$(notdir $(wildcard $(dir)/*.c)))
CFILES		:=  $(ICFILES) $(RCFILES)

SFILES		:=	$(foreach dir,$(SRCDIRS),$(notdir $(wildcard $(dir)/*.s)))

export OFILES	:=	$(CFILES:.c=.o) $(SFILES:.s=.o)
export INCLUDE	:=	$(foreach dir,$(INCDIRS),-I$(CURDIR)/$(dir))
export DEPSDIR	:=	$(CURDIR)/build

.PHONY: $(BUILD) clean docs

$(BUILD):
	@[ -d lib ] || mkdir -p lib
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo clean ...
	@rm -fr $(BUILD)
	@rm -fr $(TARGET)

install:
	@mkdir -p $(DESTDIR)$(DEVKITPRO)/$(LIBNAME)/lib
	@cp -rv include $(DESTDIR)$(DEVKITPRO)/$(LIBNAME)/include
	@cp -v lib/lib$(LIBNAME).a $(DESTDIR)$(DEVKITPRO)/$(LIBNAME)/lib/

#---------------------------------------------------------------------------------

else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------

%.a :

$(TARGET): $(OFILES)

%.a : $(OFILES)
	@echo Building $@
	@rm -f $@
	@$(AR) -crs $@ $^
	$(PREFIX)nm -Sn $@ > $(basename $(notdir $@)).map

%.iwram.o : %.iwram.c
	@echo $(notdir $<)
	$(CC) -MMD -MP -MF $(DEPSDIR)/$(@:.o=.d) $(ICFLAGS) -c $< -o $@

%.o : %.c
	@echo $(notdir $<)
	$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d $(RCFLAGS) -c $< -o $@

-include $(DEPENDS)

endif

#---------------------------------------------------------------------------------
