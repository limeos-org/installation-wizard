# ---
# Common
# ---

CC = clang
CFLAGS = -Wall -Wextra -g -MMD -MP

INTERNAL_LIBS = $(shell pkg-config --libs limeos-common-lib)
EXTERNAL_LIBS = -lncurses
LIBS = $(INTERNAL_LIBS) $(EXTERNAL_LIBS)

# ---
# Build
# ---

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

TARGET = $(BIN_DIR)/limeos-installation-wizard

SOURCES = $(shell find $(SRC_DIR) -name '*.c')
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS = $(OBJECTS:.o=.d)
-include $(DEPS)

INCLUDES = $(shell find $(SRC_DIR) -type d -exec printf "-I{} " \;)
CFLAGS += $(INCLUDES)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# ---
# Tests
# ---

TEST_DIR = tests/unit
TEST_OBJ_DIR = obj/tests
TEST_BIN_DIR = bin/tests
TEST_SRC_OBJ_DIR = obj/tests-src

TEST_SOURCES = $(shell find $(TEST_DIR) -name '*.c')
TEST_OBJECTS = $(TEST_SOURCES:$(TEST_DIR)/%.c=$(TEST_OBJ_DIR)/%.o)
TEST_BINARIES = $(TEST_SOURCES:$(TEST_DIR)/%.c=$(TEST_BIN_DIR)/%)
TEST_SRC_OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(TEST_SRC_OBJ_DIR)/%.o)
TEST_SRC_OBJECTS_NO_MAIN = $(filter-out $(TEST_SRC_OBJ_DIR)/main.o,$(TEST_SRC_OBJECTS))
TEST_DEPS = $(TEST_OBJECTS:.o=.d) $(TEST_SRC_OBJECTS:.o=.d)
-include $(TEST_DEPS)

TEST_CFLAGS = -Wall -Wextra -g -MMD -MP $(INCLUDES) -Itests -DTESTING
TEST_LIBS = $(LIBS) -lcmocka

$(TEST_SRC_OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

$(TEST_BIN_DIR)/%: $(TEST_OBJ_DIR)/%.o $(TEST_SRC_OBJECTS_NO_MAIN)
	@mkdir -p $(dir $@)
	$(CC) $< $(TEST_SRC_OBJECTS_NO_MAIN) -o $@ $(TEST_LIBS)

test: $(TEST_BINARIES)
	@failed=0; \
	for t in $(TEST_BINARIES); do \
		echo ""; \
		echo "Running tests from \"$(notdir $(TEST_DIR))/$${t#$(TEST_BIN_DIR)/}\":"; \
		echo ""; \
		$$t || failed=1; \
	done; \
	echo ""; \
	if [ $$failed -eq 0 ]; then \
		echo "All tests passed."; \
	else \
		echo "Some tests failed."; \
		exit 1; \
	fi

test-clean:
	rm -rf $(TEST_OBJ_DIR) $(TEST_BIN_DIR) $(TEST_SRC_OBJ_DIR)

# ---
# Other
# ---

.PRECIOUS: $(TEST_OBJECTS) $(TEST_SRC_OBJECTS)
.PHONY: all clean test test-clean
