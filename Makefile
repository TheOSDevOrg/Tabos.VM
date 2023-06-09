SRC_DIR=Source
INC_DIR=Include
TEST_DIR=Tests
LIB_OUT_DIR=Output/lib
OBJECTS_OUT_DIR=Output/obj
TEST_OUT_DIR=Output/tests

C_SOURCES:=$(shell find '$(SRC_DIR)' -iname *.c)
TESTS:=$(shell find '$(TEST_DIR)' -iname *.c)
C_OUTPUTS:=$(subst $(SRC_DIR),$(OBJECTS_OUT_DIR),$(C_SOURCES:.c=.c.o))
TEST_OUTPUTS:=$(subst $(TEST_DIR),$(TEST_OUT_DIR),$(TESTS:.c=))

CC=i686-elf-gcc
C_FLAGS=--std=c17 -I $(INC_DIR) -D STRAPV2

LINKER=i686-elf-ld
LINKER_FLAGS=-relocatable

NAME=TABOS_VM
COMMITID=$(shell git rev-parse HEAD)

LIB_PATH:=$(LIB_OUT_DIR)/$(NAME).so

DEFAULT_GOAL:=lib

$(OBJECTS_OUT_DIR)/%.c.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(C_FLAGS) $< -o $@ -c -ffreestanding
	@echo "$(CC)	$< >> $@"

lib: $(C_OUTPUTS)
	@mkdir -p $(LIB_OUT_DIR)
	@$(LINKER) $(LINKER_FLAGS) $(C_OUTPUTS) -o $(LIB_PATH)
	@echo "$(LINKER)	$(LIB_PATH) >> $@"

%: 
	@echo "Tests are temporary disabled"

clean:
	@rm -rf $(OBJECTS_OUT_DIR)
	@rm -rf $(TEST_OUT_DIR)

.PRECIOUS: $(TEST_OUT_DIR)/%