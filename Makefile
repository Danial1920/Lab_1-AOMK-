CC = clang
CFLAGS ?= -std=c89 -Wall -Wextra -fno-stack-protector -DECO_APPLE
INC = -IDependenciesFiles/Eco.Core1/SharedFiles -IDependenciesFiles/Eco.InterfaceBus1/SharedFiles -IDependenciesFiles/Eco.MemoryManager1/SharedFiles -IHeaderFiles -ISharedFiles
BUILD = BuildFiles/Host
TARGET = $(BUILD)/EcoLab1GeneratedUnitTest
SOURCES = SourceFiles/CEcoLab1Generated.c SourceFiles/CEcoLab1GeneratedFactory.c SourceFiles/polyfit_bus.c UnitTestFiles/SourceFiles/HostTests.c

.PHONY: all test strict check-no-libc sanitize check reference matrix
all: $(TARGET)

$(TARGET): $(SOURCES)
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $(INC) $(SOURCES) -o $@

test: $(TARGET)
	./$(TARGET)

strict:
	DesignFiles/Report/check_strict_c89.sh

check-no-libc:
	DesignFiles/Report/check_no_libc.sh

sanitize:
	DesignFiles/Report/check_sanitizers.sh

check: test strict check-no-libc sanitize

reference:
	python3 DesignFiles/Reference/reference_polyfit.py
	python3 DesignFiles/Reference/reference_scipy.py

matrix:
	DesignFiles/Report/build_matrix.sh

ECO_FRAMEWORK ?= $(CURDIR)/DependenciesFiles
ECO_INC = $(INC) -I$(ECO_FRAMEWORK)/Eco.FileSystemManagement1/SharedFiles -I$(ECO_FRAMEWORK)/Eco.MemoryManager1/SharedFiles -I$(ECO_FRAMEWORK)/Eco.InterfaceBus1/SharedFiles -I$(ECO_FRAMEWORK)/Eco.System1/SharedFiles
ECO_LIBRARIES = $(ECO_FRAMEWORK)/Eco.System1/BuildFiles/Mac/arm64/StaticRelease/lib00000000000000000000000053595333.a $(ECO_FRAMEWORK)/Eco.InterfaceBus1/BuildFiles/Mac/arm64/StaticRelease/lib00000000000000000000000042757331.a $(ECO_FRAMEWORK)/Eco.MemoryManager1/BuildFiles/Mac/arm64/StaticRelease/lib0000000000000000000000004D656D31.a $(ECO_FRAMEWORK)/Eco.FileSystemManagement1/BuildFiles/Mac/arm64/StaticRelease/lib00000000000000000000000046534D31.a
ECO_SOURCES = SourceFiles/CEcoLab1Generated.c SourceFiles/CEcoLab1GeneratedFactory.c SourceFiles/polyfit_bus.c UnitTestFiles/SourceFiles/EcoLab1Generated.c
.PHONY: eco-build eco-test
eco-build: $(BUILD)/EcoLab1EcoTest
eco-test: eco-build
	./$(BUILD)/EcoLab1EcoTest
	@echo "ECO TESTS PASSED: Gaussian and QR components"
$(BUILD)/EcoLab1EcoTest: $(ECO_SOURCES) $(wildcard HeaderFiles/*.h)
	@mkdir -p $(BUILD)
	@test -f "$(ECO_FRAMEWORK)/Eco.System1/BuildFiles/Mac/arm64/StaticRelease/lib00000000000000000000000053595333.a" || (echo "ECO_FRAMEWORK is missing Mac/arm64 Eco libraries"; exit 2)
	$(CC) $(CFLAGS) -DECO_AARCH64 -DECO_LIB $(ECO_INC) $(ECO_SOURCES) $(ECO_LIBRARIES) -o $@
