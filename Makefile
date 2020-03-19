C_COMPILER = gcc
C_FLAGS    = -O3 \
             -Wall \
             -Wextra \
             -Wshadow \
             -pedantic
LD_FLAGS   =

SRC_DIR     = ./src
INCLUDE_DIR = ./include
BUILD_DIR   = ./build

HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))
TARGETS = delaunay

all: $(TARGETS)

coord-output: C_FLAGS += -DCOORD_OUTPUT
coord-output: all

.PHONY: make-build
make-build:
	mkdir -p $(BUILD_DIR)

$(TARGETS): make-build $(OBJECTS)
	$(C_COMPILER) -o $@ $(OBJECTS) $(LD_FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(C_COMPILER) $(C_FLAGS) -I$(INCLUDE_DIR) -c -o $@ $<

.PHONY: clean
clean:
	rm -f $(TARGETS)
	rm -f $(BUILD_DIR) -r
