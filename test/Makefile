export PORTLIBS	:=	$(DEVKITPRO)/portlibs/arm
export PATH	:=	$(DEVKITARM)/bin:$(PORTLIBS)/bin:$(PATH)
LIBNDS		:=	$(DEVKITPRO)/libnds

export CC	:=	arm-none-eabi-gcc
export AS	:=	arm-none-eabi-as
export AR	:=	arm-none-eabi-ar
export OBJCOPY	:=	arm-none-eabi-objcopy
export OBJDUMP	:=	arm-none-eabi-objdump
export LD	:=	arm-none-eabi-ld

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET		:=	zzz
BUILD		:=	build
SOURCES		:=	source
INCLUDES	:=	include source

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	

CFLAGS	:=	-g -Wall -Os\
 		-march=armv5te -mtune=arm946e-s -fomit-frame-pointer -std=gnu99 \
		-ffast-math -fshort-wchar -mthumb-interwork -fno-short-enums\
		-DARM9 -nodefaultlibs -I. -fno-builtin -Wno-unknown-pragmas \
		-Wno-unused-function -c -DSDK_ARM9 -DSDK_TS -D_NITRO \
		-DSDK_SVC_WAITVBLANK_COMPATIBLE -DNITRODWC_NOGS

CFLAGS	+=	$(INCLUDE)

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-T $(CURDIR)/../symbols.x -T $(CURDIR)/../linker.x -g $(ARCH)

LDFLAGS += -Ttext $(shell cat ../baseaddr.txt)

#---------------------------------------------------------------------------------
# include SDK and NITRO-System headers if present (full paths as env vars)
#---------------------------------------------------------------------------------
ifdef NITROSDK_ROOT
	SDK_INCLUDE	:=	/$(subst :,,$(subst \,/,$(NITROSDK_ROOT)))/include
endif
ifdef NITROSDK3_ROOT
	SDK_INCLUDE	:=	/$(subst :,,$(subst \,/,$(NITROSDK3_ROOT)))/include
endif

ifdef NITROSYSTEM_ROOT
	NNS_INCLUDE	:=	/$(subst :,,$(subst \,/,$(NITROSYSTEM_ROOT)))/include
endif
ifdef NITROSYSTEM050901_ROOT
	NNS_INCLUDE	:=	/$(subst :,,$(subst \,/,$(NITROSYSTEM050901_ROOT)))/include
endif

ifdef NITROWIFI_ROOT
	NITROWIFI_INCLUDE :=	/$(subst :,,$(subst \,/,$(NITROWIFI_ROOT)))/include
endif

ifdef NITRODWC_ROOT
	NITRODWC_INCLUDE :=	/$(subst :,,$(subst \,/,$(NITRODWC_ROOT)))/include
endif
ifdef NITRODWC1_2_ROOT
	NITRODWC_INCLUDE :=	/$(subst :,,$(subst \,/,$(NITRODWC1_2_ROOT)))/include
endif



ifdef SDK_INCLUDE
	CFLAGS += -isystem $(SDK_INCLUDE)
endif
ifdef NNS_INCLUDE
	CFLAGS += -isystem $(NNS_INCLUDE)
endif
ifdef NITROWIFI_INCLUDE
	CFLAGS += -isystem $(NITROWIFI_INCLUDE)
endif
ifdef NITRODWC_INCLUDE
	CFLAGS += -isystem $(NITRODWC_INCLUDE)
endif

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project (order is important)
#---------------------------------------------------------------------------------
LIBS	:= -lnds9 -lc
 
 
#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=	$(LIBNDS)  $(DEVKITARM) $(DEVKITARM)/arm-none-eabi 

#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))

export OFILES	:=	$(CFILES:.c=.o) $(SFILES:.s=.o)
 
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-iquote $(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)
 
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib) -L$(DEVKITARM)/lib/gcc/arm-none-eabi/4.7.1


.PHONY: $(BUILD) clean
  
#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
 
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).nds $(TARGET).bin $(TARGET).sym

#---------------------------------------------------------------------------------
else
 
#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------

#all: $(OUTPUT).bin $(OUTPUT).sym

$(OUTPUT).bin : $(OUTPUT).elf
	@$(OBJCOPY) -O binary $< $@
	@arm-none-eabi-nm ../$(TARGET).elf > ../$(TARGET).txt
	@echo -----------------
	@../../pmaker $(shell cat ../baseaddr.txt) ../$(TARGET).bin ../hooks.txt ../$(TARGET).txt
	@echo -----------------
	@rm ../$(TARGET).elf ../$(TARGET).txt
	
#---------------------------------------------------------------------------------
%.elf: $(OFILES)
	@echo linking...
	@$(LD)  $(LDFLAGS) $(OFILES) $(LIBPATHS) $(LIBS) -o $@
	
#---------------------------------------------------------------------------------
%.o: %.c
	@echo $(notdir $<)
	@$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d $(CFLAGS) -c $< -o $@ $(ERROR_FILTER)
	
#---------------------------------------------------------------------------------
%.o: %.s
	@echo $(notdir $<)
	@$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d -x assembler-with-cpp $(ASFLAGS) -c $< -o $@ $(ERROR_FILTER)

-include $(DEPSDIR)/*.d
 
#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
