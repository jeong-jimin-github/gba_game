#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment.")
endif

include $(DEVKITARM)/gba_rules

MAKEFLAGS   += --no-print-directory
NAME        = game
GAMECODE    = GAME
DATE        = $(shell date +%Y-%m-%d)
OBJDIR      = obj
ROMDIR      = roms
MUSIC_DIR   = music

#---------------------------------------------------------------------------------
INCLUDES    = -I. -I./lib -I$(DEVKITPRO)/libgba/include -I$(OBJDIR)
LIBDIRS     = -L$(DEVKITPRO)/libgba/lib -L$(DEVKITPRO)/libgba/lib/gba
LIBS        = -lmm -lgba

SFILES  = $(wildcard res/*.s) $(filter-out lib/crt0.s, $(wildcard lib/*.s)) $(wildcard *.s)
CFILES  = $(wildcard res/*.c) $(wildcard lib/*.c) $(wildcard *.c)

AUDIOFILES := $(wildcard $(MUSIC_DIR)/*.*)
ifneq ($(strip $(AUDIOFILES)),)
    BINFILES := soundbank.bin
endif

#---------------------------------------------------------------------------------
ARCH    := -mcpu=arm7tdmi -mtune=arm7tdmi -mthumb -mthumb-interwork
CFLAGS  := -g -O3 $(ARCH) -fomit-frame-pointer -ffast-math -Wall $(INCLUDES)
ASFLAGS := -g $(ARCH)
LDFLAGS = -g $(ARCH) -Wl,-Map,$(MAPFILE) -specs=gba.specs $(LIBDIRS)

TARGET_ELF  = $(ROMDIR)/$(NAME)-$(DATE).elf
TARGET_BIN  = $(ROMDIR)/$(NAME)-$(DATE).gba
MAPFILE     = $(ROMDIR)/$(NAME)-$(DATE).map
NMFILE      = $(ROMDIR)/$(NAME)-$(DATE).nm

OFILES_SOURCES := $(addprefix $(OBJDIR)/, $(notdir $(SFILES:.s=.o) $(CFILES:.c=.o)))
OFILES_BIN     := $(addprefix $(OBJDIR)/, $(addsuffix .o, $(BINFILES)))
OFILES         := $(OFILES_SOURCES) $(OFILES_BIN)

VPATH := $(dir $(SFILES) $(CFILES))

#---------------------------------------------------------------------------------
.PHONY: all clean res_build

all: $(ROMDIR) $(OBJDIR) res_build $(TARGET_BIN)
	@echo Build Complete: $(TARGET_BIN)

$(ROMDIR) $(OBJDIR):
	@[ -d $@ ] || mkdir -p $@

res_build:
	@$(MAKE) -C res

clean:
	@$(MAKE) clean -C res
	rm -rf $(OBJDIR) $(ROMDIR)
	rm -f soundbank.bin soundbank.h soundbank_bin.h

$(TARGET_BIN): $(TARGET_ELF)
	@$(OBJCOPY) -v -O binary $< $@
	@gbafix $@ -t$(NAME) -c$(GAMECODE)

$(TARGET_ELF): $(OFILES)
	@echo # Linking $@
	@$(CC) $(OFILES) $(LDFLAGS) $(LIBS) -o $@

soundbank.bin soundbank.h : $(AUDIOFILES)
	@echo # Generating Soundbank from $(MUSIC_DIR)
	@mmutil $^ -osoundbank.bin -hsoundbank.h

$(OBJDIR)/soundbank.bin.o $(OBJDIR)/soundbank_bin.h : soundbank.bin
	@echo # Converting soundbank.bin to object and header
	@$(bin2o)
	@mv soundbank.bin.o $(OBJDIR)/
	@mv soundbank_bin.h $(OBJDIR)/

$(OFILES_SOURCES): soundbank.h $(OBJDIR)/soundbank_bin.h

$(OBJDIR)/%.o: %.c
	@echo # compiling $<
	@$(CC) -MMD -MP -MF $(OBJDIR)/$*.d $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.s
	@echo # assembling $<
	@$(CC) $(ASFLAGS) -c $< -o $@

-include $(OBJDIR)/*.d