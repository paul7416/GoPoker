# Makefile for poker evaluator project
# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
TEST_DIR = test
LIB_DIR = lib

# Compiler
CC = gcc
CFLAGS = -O3 -Wall -Wextra -Wpedantic -march=native -std=c11
CFLAGS += -I$(LIB_DIR)
#CFLAGS += -pg
CFLAGS += -isystem
CFLAGS += /usr/include
CFLAGS += -isystem
CFLAGS += /usr/include/x86_64-linux-gnu
CFLAGS += -funroll-loops
CFLAGS += -fpeel-loops
LDFLAGS = -flto

# Object files
EVALUATOR_O = $(OBJ_DIR)/evaluator.o
EV_O = $(OBJ_DIR)/ev.o
TABLE_IMPORT_O = $(OBJ_DIR)/table_import.o
RANGE_EQUILATOR_O = $(OBJ_DIR)/range_equilator.o
LIST_O = $(OBJ_DIR)/list.o
EV_CALC_O = $(OBJ_DIR)/ev_calculator.o
HISTOGRAM_O = $(OBJ_DIR)/histogram.o
PLAYER_O = $(OBJ_DIR)/player.o


# Create directories if they don't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compile src/*.c files into .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Compile test/*.c files into .o files
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# === Executables ===

$(BIN_DIR)/c_solver: $(OBJ_DIR)/c_solver.o $(EVALUATOR_O) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/hand_masks_generator: $(OBJ_DIR)/hand_masks_generator.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/random_hand_generator: $(OBJ_DIR)/random_hand_generator.o $(EVALUATOR_O) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# === Tests ===

$(BIN_DIR)/test_evaluator: $(OBJ_DIR)/test_evaluator.o $(EVALUATOR_O) $(TABLE_IMPORT_O) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/test_ev_calculator: $(OBJ_DIR)/test_ev_calculator.o $(EVALUATOR_O) $(TABLE_IMPORT_O) $(EV_CALC_O) $(HISTOGRAM_O) $(PLAYER_O) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/test_range_equilator: $(OBJ_DIR)/test_range_equilator.o $(RANGE_EQUILATOR_O) $(TABLE_IMPORT_O) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/test_ev: $(OBJ_DIR)/test_ev.o $(EV_O) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/test_list: $(OBJ_DIR)/test_list.o $(LIST_O) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# === Future example ===
# $(BIN_DIR)/sng_simulator: $(OBJ_DIR)/sng_simulator.o $(EVALUATOR_O) $(ICM_O) $(OBJ_DIR)/sng_monte_carlo.o | $(BIN_DIR)
# 	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Include dependency files
-include $(OBJ_DIR)/*.d

# Targets
ALL_TARGETS = $(BIN_DIR)/c_solver $(BIN_DIR)/hand_masks_generator $(BIN_DIR)/random_hand_generator $(BIN_DIR)/icm
TEST_TARGETS = $(BIN_DIR)/test_evaluator

all: $(ALL_TARGETS)

test: $(TEST_TARGETS)
	@for t in $(TEST_TARGETS); do echo "Running $$t..."; $$t || exit 1; done

clean:
	rm -f $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d
	rm -f $(ALL_TARGETS) $(TEST_TARGETS)

rebuild: clean all

.PHONY: all clean rebuild test
