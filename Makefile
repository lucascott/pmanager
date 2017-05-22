
.PHONY: help clean build assets test exec

CC = gcc
CFLAGS =

BUILD_DIR = src/build
FILE_LIST = src/list.h src/list.c src/utils.h src/utils.c src/main.c
EXEC_NAME = pmanager

ASSETS_DIR = src/assets
DATA_FILE_LIST = src/datagen.c
DATA_EXEC_NAME = datagen
DATAFILE_NAME = data.txt

help:
	@echo "\nGRUPPO:"
	@echo " Luca Scotton (178801)"
	@echo " Pietro Bolcato (178601)"
	@echo " Daniele Del Sale (180369)"
	@echo " Giulio Marcon (178633)\n"
	@echo "DESCRIZIONE: Gestore di processi a interfaccia shell con supporto a creazione, terminazione, visualizzazione informazioni e clonazione di processi.\n"
	@echo "UTILIZZO: make [opzione]\n"
	@echo "OPZIONI:\n  help: visualizza questo messaggio\n  clean: rimuove le directory generate dal programma\n  build: genera l'eseguibile del programma\n  assets: genera directory con i files di supporto\n  test: esegue il programma in modalità test\n"

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
	@echo "Compilazione generatore dinamico in corso..."
	@$(CC) $(CFLAGS) -o $(ASSETS_DIR)/$(DATA_EXEC_NAME) $(DATA_FILE_LIST)
	@echo "Creazione dinamica file input in corso..."
	@$(ASSETS_DIR)/$(DATA_EXEC_NAME) >$(ASSETS_DIR)/$(DATAFILE_NAME)

test: assets
	@echo "\n\t[Esecuzione in modalità test]\n"
	@echo "Esecuzione pmanager in modalità test...\n"
	@$(BUILD_DIR)/$(EXEC_NAME) $(ASSETS_DIR)/$(DATAFILE_NAME)

exec: assets
	@echo "Esecuzione pmanager...\n"
	@$(BUILD_DIR)/$(EXEC_NAME)
