ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

ifeq ($(filter $(DEVKITARM)/bin,$(PATH)),)
export PATH:=$(DEVKITARM)/bin:$(PATH)
endif

CC = arm-none-eabi-gcc
# LINK = arm-none-eabi-gcc
LINK = arm-none-eabi-ld
AS = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy
CFLAGS += -Wall -mbig-endian -std=gnu99 -march=armv5tej -Os -I$(DEVKITPRO)/libnds/include
LDFLAGS += --script=link.ld -EB -L"$(DEVKITARM)/arm-none-eabi/lib" -Map=output.map

CFILES = $(wildcard source/*.c)
BINFILES = $(wildcard data/*.bin)
OFILES = $(BINFILES:data/%.bin=build/%.bin.o)
OFILES += $(CFILES:source/%.c=build/%.o)
DFILES = $(CFILES:source/%.c=build/%.d)
SFILES = $(wildcard source/*.s)
OFILES += $(SFILES:source/%.s=build/%.o)
PROJECTNAME = ${shell basename "$(CURDIR)"}
CWD = "$(CURDIR)""

#---------------------------------------------------------------------------------
# canned command sequence for binary data, taken from devkitARM
#---------------------------------------------------------------------------------
.PHONY:=all dirs

all: dirs $(PROJECTNAME).bin $(PROJECTNAME)_bin.h

dirs:
	@mkdir -p build

$(PROJECTNAME).bin: $(PROJECTNAME).elf
#	$(OBJCOPY) -O binary $< $@
	$(OBJCOPY) -j .text -j .rodata -O binary $< $@

$(PROJECTNAME)_bin.h: $(PROJECTNAME).bin
	xxd -i $< | sed "s/unsigned/static const unsigned/g;s/$(PROJECTNAME)$*/$(PROJECTNAME)/g" > $@

$(PROJECTNAME).elf: $(OFILES)
	$(LINK) $(LDFLAGS) -o $(PROJECTNAME).elf $(filter-out build/crt0.o, $(OFILES))

clean:
	@rm -f build/*.o build/*.d
	@rm -f $(PROJECTNAME).elf $(PROJECTNAME).bin $(PROJECTNAME)_bin.h
	@echo "all cleaned up !"

-include $(DFILES)

build/%.o: source/%.c
	$(CC) $(CFLAGS) -c $< -o $@
	@$(CC) -MM $< > build/$*.d

build/%.o: source/%.s
	$(CC) $(CFLAGS) -xassembler-with-cpp -c $< -o $@
	@$(CC) -MM $< > build/$*.d

build/%.bin.o: data/%.bin
	@echo $(notdir $<)
	@$(bin2o)
