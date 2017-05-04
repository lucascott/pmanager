# Simple C Shell Makefile

.PHONY: help clean build assets test

CC = gcc
CFLAGS =
FILE_LIST = list.h list.c main.c
BUILD_DIR = build
ASSETS_DIR = assets
EXEC_NAME = pmanager

help:
	@echo "\nGRUPPO:\n"
	@echo "Luca Scotton,"
	@echo "Pietro Bolcato,"
	@echo "Daniele Del Sale,"
	@echo "Giulio Marcon\n"
	@echo "UTILIZZO: make [opzione]\nOPZIONI:\n\thelp\n\tclean\n\tbuild\n\tassets\n\ttest"

clean:
	@echo "Pulizia..."
	@rm -rf $(BUILD_DIR)
	@rm -rf $(ASSETS_DIR)

build: clean
	@echo "Creazione build directory..."
	@mkdir $(BUILD_DIR)
	@echo "Compilazione in corso..."
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(EXEC_NAME) $(FILE_LIST)

assets: build
	@echo "Creazione assets directory..."
	@mkdir -p $(ASSETS_DIR)

test: assets
	@echo "Eseguzione in modalità test..."
	$(BUILD_DIR)/$(EXEC_NAME)
