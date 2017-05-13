
.PHONY: help clean build assets test exec

CC = gcc
CFLAGS =

BUILD_DIR = src/build
FILE_LIST = src/list.h src/list.c src/main.c
EXEC_NAME = pmanager

ASSETS_DIR = src/assets
DATA_FILE_LIST = src/datagen.c
DATA_EXEC_NAME = datagen
DATAFILE_NAME = data.txt

help:
	@echo "\nGRUPPO:\n"
	@echo "Luca Scotton 178801,"
	@echo "Pietro Bolcato 178601,"
	@echo "Daniele Del Sale ,"
	@echo "Giulio Marcon\n"
	@echo "Desc\n\n"
	@echo "UTILIZZO: make [opzione]\nOPZIONI:\n  help\n  clean\n  build\n  assets\n  test"

clean:
	@echo "Pulizia..."
	@rm -rf $(BUILD_DIR)
	@rm -rf $(ASSETS_DIR)
	@echo "Pulizia completata"

build: clean
	@echo "Creazione build directory..."
	@mkdir $(BUILD_DIR)
	@echo "Compilazione in corso..."
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(EXEC_NAME) $(FILE_LIST)

assets: build
	@echo "Creazione assets directory..."
	@mkdir -p $(ASSETS_DIR)

test: assets
	@echo "\n\t[Esecuzione in modalità test]\n"
	@echo "Compilazione generatore dinamico in corso..."
	$(CC) $(CFLAGS) -o $(ASSETS_DIR)/$(DATA_EXEC_NAME) $(DATA_FILE_LIST)
	@echo "Creazione dinamica file input in corso..."
	$(ASSETS_DIR)/$(DATA_EXEC_NAME) > $(ASSETS_DIR)/$(DATAFILE_NAME)
	@echo "Esecuzione pmanager in modalità test..."
	$(BUILD_DIR)/$(EXEC_NAME) $(ASSETS_DIR)/$(DATAFILE_NAME)

exec: assets
	@echo "Esecuzione pmanager... -------------------------------------------------------------------"
	$(BUILD_DIR)/$(EXEC_NAME)
