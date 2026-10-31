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
            $(SRC_DIR)/ordenacao.c

OBJS     := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

ifeq ($(OS),Windows_NT)
    SHELL     := cmd.exe
    TARGET    := biblioteca.exe
    MKDIR_OBJ := if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"
    RM_CLEAN  := if exist "$(OBJ_DIR)" rmdir /S /Q "$(OBJ_DIR)" & if exist "$(TARGET)" del /F /Q "$(TARGET)"
    RUN_CMD   := $(TARGET)
else
    TARGET    := biblioteca
    MKDIR_OBJ := mkdir -p $(OBJ_DIR)
    RM_CLEAN  := rm -rf $(OBJ_DIR) $(TARGET)
    RUN_CMD   := ./$(TARGET)
endif

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	$(MKDIR_OBJ)

run: $(TARGET)
	$(RUN_CMD)

clean:
	$(RM_CLEAN)
