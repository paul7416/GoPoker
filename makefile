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
CFLAGS += -Wconversion
CFLAGS += -I$(LIB_DIR)
#CFLAGS += -g
#CFLAGS += -fsanitize=address
CFLAGS += -isystem
CFLAGS += /usr/include
CFLAGS += -isystem
CFLAGS += /usr/include/x86_64-linux-gnu
CFLAGS += -funroll-loops
CFLAGS += -fpeel-loops
CFLAGS += -fPIC
LDFLAGS = -flto

# Object files
EVALUATOR_O = $(OBJ_DIR)/evaluator.o
EV_O = $(OBJ_DIR)/ev.o
TABLE_IMPORT_O = $(OBJ_DIR)/table_import.o
RANGE_EQUILATOR_O = $(OBJ_DIR)/range_equilator.o
LIST_O = $(OBJ_DIR)/list.o
SIMULATOR_O = $(OBJ_DIR)/simulator.o
HISTOGRAM_O = $(OBJ_DIR)/histogram.o
PLAYER_O = $(OBJ_DIR)/player.o
SOLVER_O = $(OBJ_DIR)/solver.o
DECK_O = $(OBJ_DIR)/deck.o


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

$(BIN_DIR)/solver: $(OBJ_DIR)/solver.o $(EV_O) $(EVALUATOR_O) $(PLAYER_O) $(TABLE_IMPORT_O) $(ICM_O) $(HISTOGRAM_O) $(SIMULATOR_O)| $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# === Shared Library ===
$(BIN_DIR)/solver.so: $(SOLVER_O) $(EV_O) $(EVALUATOR_O) $(PLAYER_O) $(TABLE_IMPORT_O) $(ICM_O) $(HISTOGRAM_O) $(SIMULATOR_O) $(DECK_O) | $(BIN_DIR)
	$(CC) -shared $(CFLAGS) $^ -o $@ $(LDFLAGS)


# === Tests ===
$(BIN_DIR)/test_solver: $(OBJ_DIR)/test_solver.o $(EV_O) $(EVALUATOR_O) $(PLAYER_O) $(TABLE_IMPORT_O) $(ICM_O) $(HISTOGRAM_O) $(SIMULATOR_O) $(SOLVER_O) $(DECK_O) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/test_evaluator: $(OBJ_DIR)/test_evaluator.o $(EVALUATOR_O) $(TABLE_IMPORT_O) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/test_evaluate_round: $(OBJ_DIR)/test_evaluate_round.o $(EVALUATOR_O) $(TABLE_IMPORT_O) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/test_simulator: $(OBJ_DIR)/test_simulator.o $(EVALUATOR_O) $(TABLE_IMPORT_O) $(SIMULATOR_O) $(HISTOGRAM_O) $(PLAYER_O) $(DECK_O) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/test_range_equilator: $(OBJ_DIR)/test_range_equilator.o $(RANGE_EQUILATOR_O) $(TABLE_IMPORT_O) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/test_ev: $(OBJ_DIR)/test_ev.o $(EV_O) $(EVALUATOR_O) $(PLAYER_O) $(TABLE_IMPORT_O) $(ICM_O) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/test_list: $(OBJ_DIR)/test_list.o $(LIST_O) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# === Future example ===
# $(BIN_DIR)/sng_simulator: $(OBJ_DIR)/sng_simulator.o $(EVALUATOR_O) $(ICM_O) $(OBJ_DIR)/sng_monte_carlo.o | $(BIN_DIR)
# 	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Include dependency files
-include $(OBJ_DIR)/*.d

clean:
	rm -f $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d
	rm -f $(ALL_TARGETS) $(TEST_TARGETS)

.PHONY: all clean rebuild test
