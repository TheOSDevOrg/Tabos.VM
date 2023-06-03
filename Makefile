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

CC=gcc
C_FLAGS=-I$(INC_DIR)

NAME=TABOS_VM
COMMITID=$(shell git rev-parse HEAD)

LIB_PATH:=$(LIB_OUT_DIR)/$(NAME).so

$(OBJECTS_OUT_DIR)/%.c.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $< -o $@ $(C_FLAGS) -c
	@echo "$(CC)	$< >> $@"

lib: $(C_OUTPUTS)
	@mkdir -p $(LIB_OUT_DIR)
	@gcc -shared -o $(LIB_PATH) $(C_OUTPUTS)
	@echo "$(CC)	$(LIB_PATH) >> $@"

%: $(TEST_DIR)/%.c lib
	@mkdir -p $(TEST_OUT_DIR)
	@$(CC) $< $(LIB_PATH) -o $(TEST_OUT_DIR)/$@ $(C_FLAGS)
	@./$(TEST_OUT_DIR)/$@

clean:
	@rm -rf $(OBJECTS_OUT_DIR)
	@rm -rf $(TEST_OUT_DIR)