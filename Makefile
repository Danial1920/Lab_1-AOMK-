CC = clang
PYTHON ?= python3
CFLAGS ?= -std=c89 -Wall -Wextra -fno-stack-protector -DECO_APPLE
INC = -IDependenciesFiles/Eco.Core1/SharedFiles -IDependenciesFiles/Eco.InterfaceBus1/SharedFiles -IDependenciesFiles/Eco.MemoryManager1/SharedFiles -IHeaderFiles -ISharedFiles
CALCULATOR_VARIANT ?= A
ifeq ($(CALCULATOR_VARIANT),C)
VARIANT_FLAGS = -DECO_REPLACE_A_WITH_C
else ifneq ($(CALCULATOR_VARIANT),A)
$(error CALCULATOR_VARIANT must be A or C)
endif
BUILD ?= BuildFiles/Host
TARGET = $(BUILD)/EcoLab1GeneratedUnitTest
SOURCES = SourceFiles/CEcoLab1Generated.c SourceFiles/CEcoLab1GeneratedFactory.c SourceFiles/CalculatorComponents.c SourceFiles/polyfit_bus.c UnitTestFiles/SourceFiles/HostTests.c
HEADERS = $(wildcard HeaderFiles/*.h SharedFiles/*.h UnitTestFiles/SourceFiles/*.h DependenciesFiles/*/SharedFiles/*.h)

.PHONY: all test strict check-no-libc sanitize check reference matlab-reference matrix verify FORCE
all: $(TARGET)

# These tiny console clients are always rebuilt. This also covers changed
# flags/Framework paths and sub-second edits with macOS' legacy GNU make.
$(TARGET): $(SOURCES) $(HEADERS) Makefile FORCE
	@mkdir -p "$(BUILD)"
	$(CC) $(CFLAGS) $(VARIANT_FLAGS) $(INC) $(SOURCES) -o "$@"

test: $(TARGET)
	"$(TARGET)"

strict:
	DesignFiles/Report/check_strict_c89.sh

check-no-libc:
	DesignFiles/Report/check_no_libc.sh

sanitize:
	ECO_FRAMEWORK="$(ECO_FRAMEWORK)" DesignFiles/Report/check_sanitizers.sh

# Host and compile checks; real Eco integration is also run by sanitize.
check: test strict check-no-libc sanitize validation-tools component-clients

reference: validation-tools
	$(PYTHON) DesignFiles/Reference/compare_scipy.py

matlab-reference:
	$(PYTHON) DesignFiles/Reference/compare_matlab.py

matrix:
	DesignFiles/Report/build_matrix.sh

ECO_FRAMEWORK ?= $(HOME)/Downloads/Eco.Framework
ECO_INC = $(INC) -I$(ECO_FRAMEWORK)/Eco.FileSystemManagement1/SharedFiles -I$(ECO_FRAMEWORK)/Eco.MemoryManager1/SharedFiles -I$(ECO_FRAMEWORK)/Eco.InterfaceBus1/SharedFiles -I$(ECO_FRAMEWORK)/Eco.System1/SharedFiles
ECO_LIBRARIES = $(ECO_FRAMEWORK)/Eco.System1/BuildFiles/Mac/arm64/StaticRelease/lib00000000000000000000000053595333.a $(ECO_FRAMEWORK)/Eco.InterfaceBus1/BuildFiles/Mac/arm64/StaticRelease/lib00000000000000000000000042757331.a $(ECO_FRAMEWORK)/Eco.MemoryManager1/BuildFiles/Mac/arm64/StaticRelease/lib0000000000000000000000004D656D31.a $(ECO_FRAMEWORK)/Eco.FileSystemManagement1/BuildFiles/Mac/arm64/StaticRelease/lib00000000000000000000000046534D31.a
ECO_SOURCES = SourceFiles/CEcoLab1Generated.c SourceFiles/CEcoLab1GeneratedFactory.c SourceFiles/CalculatorComponents.c SourceFiles/EcoLab2.c SourceFiles/polyfit_bus.c UnitTestFiles/SourceFiles/EcoLab1Generated.c
ECO_HEADERS = $(wildcard $(ECO_FRAMEWORK)/*/SharedFiles/*.h)
.PHONY: eco-build eco-test
eco-build: $(BUILD)/EcoLab1EcoTest
eco-test: eco-build
	"$(BUILD)/EcoLab1EcoTest"
	@echo "ECO TESTS PASSED: real Interface Bus, Lab1/Lab2/A-E and failure paths"
$(BUILD)/EcoLab1EcoTest: $(ECO_SOURCES) $(HEADERS) $(ECO_HEADERS) $(ECO_LIBRARIES) Makefile FORCE
	@mkdir -p "$(BUILD)"
	$(CC) $(CFLAGS) $(VARIANT_FLAGS) -DECO_AARCH64 -DECO_LIB $(ECO_INC) $(ECO_SOURCES) $(ECO_LIBRARIES) -o "$@"

verify: check eco-test-variants reference matlab-reference validation-build

.PHONY: validation-tools
validation-tools:
	$(PYTHON) DesignFiles/Reference/test_reference_validation.py

.PHONY: validation-build
validation-build:
	ECO_FRAMEWORK="$(ECO_FRAMEWORK)" $(PYTHON) DesignFiles/Reference/test_build_validation.py

.PHONY: eco-test-variants
eco-test-variants:
	$(MAKE) CALCULATOR_VARIANT=A BUILD=BuildFiles/VariantA ECO_FRAMEWORK="$(ECO_FRAMEWORK)" eco-test
	$(MAKE) CALCULATOR_VARIANT=C BUILD=BuildFiles/VariantC ECO_FRAMEWORK="$(ECO_FRAMEWORK)" eco-test

.PHONY: component-clients
component-clients:
	ECO_FRAMEWORK="$(ECO_FRAMEWORK)" $(PYTHON) DesignFiles/Report/build_component_clients.py
