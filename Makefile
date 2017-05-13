
.PHONY: help clean build assets test exec

CC = gcc
CFLAGS =

BUILD_DIR = build
FILE_LIST = list.h list.c main.c
EXEC_NAME = pmanager

ASSETS_DIR = assets
DATA_FILE_LIST = datagen.c
DATA_EXEC_NAME = datagen
DATAFILE_NAME = data.txt

help:
	@echo "\nGRUPPO:\n"
	@echo "Luca Scotton 178801,"
	@echo "Pietro Bolcato 178601,"
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
