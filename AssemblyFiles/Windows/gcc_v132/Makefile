# Configurations

# Platform detection
PLATFORM = Windows

ifeq ($(OS), Windows_NT)
	#WINDOWS USE THESE DEFINITIONS
#	RM = del /Q /F
	RM = -rm -f
#	SLASH = \\
	SLASH = /
	SWALLOW_OUTPUT = >nul 2>nul
    FRAMEWORK := $(subst \,/,$(ECO_FRAMEWORK))	
	TOOLCHAIN = $(subst \,/,$(ECO_TOOLCHAIN))
	TOOL = $(TOOLCHAIN)$(SLASH)eco-i686-win-dwarf-gcc-13.2.0-win-x86_64$(SLASH)bin$(SLASH)i686-w64-mingw32-
else
	#LINUX USE THESE DEFINITIONS
	RM = -rm -f
	SLASH = /
	SWALLOW_OUTPUT =
	TOOLCHAIN = 
	TOOL = 
endif 

# Target configuration
ifeq ($(TARGET), 1)
	OUT_TARGET = 4D572607A26A48C1BBC94AB2CB6CFF86.lib
	BASE_TARGET = Static
	CCFLAGS_TARGET = -Wall -DUGUID_UTILITY -DECO_WINDOWS -DECO_LIB
	LDFLAGS_TARGET = 
else
	OUT_TARGET = 4D572607A26A48C1BBC94AB2CB6CFF86.dll
	BASE_TARGET = Dynamic
	CCFLAGS_TARGET = -Wall -fPIC -DUGUID_UTILITY -DECO_WINDOWS -DECO_DLL
	LDFLAGS_TARGET = -shared
endif

# Debug symbols configuration
ifeq ($(DEBUG), 1)
	CONFIG_TARGET = $(BASE_TARGET)Debug
    CCFLAGS_TARGET += -g -DDEBUG  
    LDFLAGS_TARGET += 
    ifeq ($(TARGET), 1)
        CCFLAGS_TARGET += 
    else
        LDFLAGS_TARGET += 
    endif
else
	CONFIG_TARGET = $(BASE_TARGET)Release
    CCFLAGS_TARGET += -DNDEBUG 
    LDFLAGS_TARGET +=
    ifeq ($(TARGET), 1)
        CCFLAGS_TARGET += 
    endif
endif

# Architecture selection
ifeq ($(ARCH), x86_32)
	CCFLAGS_TARGET += -DECO_X86_32
	LDFLAGS_TARGET +=
	ARCH_TARGET = x86
else ifeq ($(ARCH), x86)
	CCFLAGS_TARGET += -DECO_X86_32
	LDFLAGS_TARGET +=
	ARCH_TARGET = x86
else ifeq ($(ARCH), x86_64)
	CCFLAGS_TARGET += -DECO_X86_64  
	LDFLAGS_TARGET +=
	ARCH_TARGET = amd64
else ifeq ($(ARCH), x64)
	CCFLAGS_TARGET += -DECO_X86_64  
	LDFLAGS_TARGET +=
	ARCH_TARGET = amd64
endif

# Device selection
ifdef DEVICE
    DEVICE_TARGET = $(DEVICE)$(SLASH)
endif

# Include directories
INC = -I "..$(SLASH)..$(SLASH)..$(SLASH)DependenciesFiles$(SLASH)Eco.Core1$(SLASH)SharedFiles" \
-I "..$(SLASH)..$(SLASH)..$(SLASH)DependenciesFiles$(SLASH)Eco.InterfaceBus1$(SLASH)SharedFiles" \
-I "..$(SLASH)..$(SLASH)..$(SLASH)DependenciesFiles$(SLASH)Eco.MemoryManager1$(SLASH)SharedFiles" \
-I "..$(SLASH)..$(SLASH)..$(SLASH)DependenciesFiles$(SLASH)Eco.FileSystemManagement1$(SLASH)SharedFiles" \
-I "..$(SLASH)..$(SLASH)..$(SLASH)DependenciesFiles$(SLASH)Eco.System1$(SLASH)SharedFiles"\
-I ..$(SLASH)..$(SLASH)..$(SLASH)HeaderFiles \
-I ..$(SLASH)..$(SLASH)..$(SLASH)SharedFiles


# Source files.
SRC = \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoLab1Generated.c \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoLab1GeneratedFactory.c \
      ..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CalculatorComponents.c \
      ..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)EcoLab2.c \
      ..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)polyfit_bus.c

# Libraries
LIBS = -lkernel32 -luser32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32

# Object files
OBJ = $(SRC:.c=.o)

OUT = $(OUT_TARGET)
OUT_DIR = ..$(SLASH)..$(SLASH)..$(SLASH)BuildFiles$(SLASH)$(PLATFORM)$(SLASH)$(ARCH_TARGET)$(SLASH)$(DEVICE_TARGET)$(CONFIG_TARGET)

# C/C++ compiler flags
CCFLAGS = $(CCFLAGS_TARGET) -DECO_FLOAT_T_DEFINED -DECO_DOUBLE_T_DEFINED
 
 # Compile flags
LDFLAGS = $(LDFLAGS_TARGET)

# Tools
CC = $(TOOL)gcc
AR = $(TOOL)gcc-ar
OC = $(TOOL)objcopy

ifeq ($(TARGET), 1)
	CMD_TARGET=$(AR) rcs $(OUT_DIR)$(SLASH)$(OUT) $(OBJ) $(LDFLAGS)
else
	CMD_TARGET=$(CC) -o $(LIBS) $(OUT_DIR)$(SLASH)$(OUT) $(OBJ) $(LDFLAGS)
endif
 
.SUFFIXES: .c

all:  $(OUT)

.c.o:
	mkdir -p "$(OUT_DIR)"
	$(CC) $(INC) $(CCFLAGS) -c $< -o $@
 
$(OUT): $(OBJ)
	$(CMD_TARGET)
	$(RM) $(OBJ)

clean:
	$(RM) $(OBJ) $(OUT_DIR)$(SLASH)$(OUT)
