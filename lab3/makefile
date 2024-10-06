CC := gcc

INC_DIR := inc
OUT_DIR := out
SRC_DIR := src
UNIT_DIR := unit_tests

vpath %.c $(SRC_DIR) $(UNIT_DIR)

CFLAGS := -std=c99 -Wall -Werror -Wpedantic -Wextra -Wfloat-conversion -Wfloat-equal -Wvla -I $(INC_DIR)
LFLAGS := -lm

DEBUG_CFLAGS := -O0 -g
UNIT_TESTS_FLAGS := -lcheck -lrt

BUILD_SRCS := $(shell find $(SRC_DIR) -type f -regex .*\.c -printf "%P\n")
NO_MAIN_BUILD_SRCS := $(shell find $(SRC_DIR) -type f -not -name "*main*" -regex .*\.c -printf "%P\n")
UNIT_SRCS := $(shell find $(UNIT_DIR) -type f -name "*check_*" -regex .*\.c -printf "%P\n") 

BUILD_OBJS := $(foreach OBJ, $(BUILD_SRCS:.c=.o), $(OUT_DIR)/$(OBJ))
NO_MAIN_BUILD_OBJS := $(foreach OBJ, $(NO_MAIN_BUILD_SRCS:.c=.o), $(OUT_DIR)/$(OBJ))
UNIT_OBJS := $(foreach OBJ, $(UNIT_SRCS:.c=.o), $(OUT_DIR)/$(OBJ))

debug: CFLAGS += $(DEBUG_CFLAGS)
unit_tests.exe: CFLAGS += $(DEBUG_CFLAGS)
unit_tests.exe: LFLAGS += $(UNIT_TESTS_FLAGS)

# -----

$(OUT_DIR)/%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

# -----

app.exe: out_dir_check | $(BUILD_OBJS)
	$(CC) $(LFLAGS) -o "$@" $(BUILD_OBJS)

.PHONY: debug
debug: out_dir_check | $(BUILD_OBJS)
	$(CC) $(LFLAGS) -o "app.exe" $(BUILD_OBJS) 

# -----

unit_tests.exe: out_dir_check | $(UNIT_OBJS) $(NO_MAIN_BUILD_OBJS)
	$(CC) $(LFLAGS) -o "$@" $(UNIT_OBJS) $(NO_MAIN_BUILD_OBJS)

# -----

.PHONY: func
func: 
	@echo -----BUILD_RELEASE-----
	$(MAKE) -s app.exe
	bash ./func_tests/scripts/func_tests.sh

	$(MAKE) -s clean

	@echo -----BUILD_DEBUG-----
	$(MAKE) -s debug
	bash ./func_tests/scripts/func_tests.sh

# -----

.PHONY: out_dir_check
out_dir_check:
	@if ! [ -d $(OUT_DIR) ]; then mkdir $(OUT_DIR); fi

.PHONY: clean
clean:
	rm -rf out
	rm -rf app.exe
	rm -rf ./*.gcno
	rm -rf ./*.gcda
	rm -rf ./*.c.gcov
	rm -f ./func_tests/scripts/*.txt
