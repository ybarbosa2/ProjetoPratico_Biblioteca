# ==============================================================================
# Makefile - Projeto Prático de Estrutura de Dados em C (Tema C - Biblioteca)
# Compatível com mingw32-make / make no Windows (CMD/PowerShell) e Linux/macOS
# ==============================================================================

CC       := gcc
CFLAGS   := -Wall -Wextra -pedantic -std=c99 -Iinclude -g

SRC_DIR  := src
INC_DIR  := include
OBJ_DIR  := build

SRCS     := $(SRC_DIR)/main.c \
            $(SRC_DIR)/bst.c \
            $(SRC_DIR)/grafo.c \
            $(SRC_DIR)/lista.c \
            $(SRC_DIR)/pilha.c \
            $(SRC_DIR)/fila.c \
            $(SRC_DIR)/ordenacao.c \
            $(SRC_DIR)/io.c

OBJS     := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

ifeq ($(OS),Windows_NT)
    SHELL        := cmd.exe
    TARGET       := biblioteca.exe
    TEST2_TARGET := build/test_etapa2.exe
    TEST3_TARGET := build/test_etapa3.exe
    TEST4_TARGET := build/test_etapa4.exe
    MKDIR_OBJ    := if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"
    RM_CLEAN     := if exist "$(OBJ_DIR)" rmdir /S /Q "$(OBJ_DIR)" & if exist "$(TARGET)" del /F /Q "$(TARGET)"
    RUN_CMD      := $(TARGET)
    RUN_TEST2    := build\test_etapa2.exe
    RUN_TEST3    := build\test_etapa3.exe
    RUN_TEST4    := build\test_etapa4.exe
else
    TARGET       := biblioteca
    TEST2_TARGET := build/test_etapa2
    TEST3_TARGET := build/test_etapa3
    TEST4_TARGET := build/test_etapa4
    MKDIR_OBJ    := mkdir -p $(OBJ_DIR)
    RM_CLEAN     := rm -rf $(OBJ_DIR) $(TARGET)
    RUN_CMD      := ./$(TARGET)
    RUN_TEST2    := ./$(TEST2_TARGET)
    RUN_TEST3    := ./$(TEST3_TARGET)
    RUN_TEST4    := ./$(TEST4_TARGET)
endif

LIB_OBJS := $(filter-out $(OBJ_DIR)/main.o,$(OBJS))

.PHONY: all clean run test test2 test3 test4

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	$(MKDIR_OBJ)

test2: $(LIB_OBJS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) tests/test_etapa2.c $(LIB_OBJS) -o $(TEST2_TARGET)
	$(RUN_TEST2)

test3: $(LIB_OBJS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) tests/test_etapa3.c $(LIB_OBJS) -o $(TEST3_TARGET)
	$(RUN_TEST3)

test4: $(LIB_OBJS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) tests/test_etapa4.c $(LIB_OBJS) -o $(TEST4_TARGET)
	$(RUN_TEST4)

test: test2 test3 test4

run: $(TARGET)
	$(RUN_CMD)

clean:
	$(RM_CLEAN)
