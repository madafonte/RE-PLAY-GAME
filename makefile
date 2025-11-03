# Define o nome do executável
TARGET = replay_menu

# --- MUDANÇAS AQUI ---
# Define as pastas de origem
SRC_DIR = src
INCLUDE_DIR = include
# --------------------

BUILD_DIR = build
BIN_DIR = bin

# Encontra todos os arquivos .c na pasta SRC_DIR
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Define os objetos (.o) que serão criados a partir dos .c
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# --- MUDANÇA AQUI ---
# Flags de compilação (Adicionamos -I$(INCLUDE_DIR) para "enxergar" os .h)
CC = gcc
CFLAGS = -Wall -std=c99 -I$(INCLUDE_DIR) -g
# --------------------

# Flags de linkagem (bibliotecas Raylib e outras)
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Regra padrão: Compilar e linkar tudo
all: $(BIN_DIR)/$(TARGET)

# Cria as pastas de build e bin se não existirem
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Regra para compilar cada arquivo .c em .o (Esta regra já estava correta)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para linkar os objetos e criar o executável final
$(BIN_DIR)/$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

# Limpa os arquivos compilados
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)