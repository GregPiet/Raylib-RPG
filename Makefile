# **************************************************************************************************
#   Custom Raylib Makefile for modular C++ project (Windows / w64devkit)
# **************************************************************************************************

.PHONY: all clean

# === CONFIGURATION PROJET ===
PROJECT_NAME       ?= game
RAYLIB_PATH        ?= C:/raylib/raylib
COMPILER_PATH      ?= C:/raylib/w64devkit/bin
PLATFORM           ?= PLATFORM_DESKTOP

# === COMPILATEUR ===
CC = g++
CFLAGS = -Wall -std=c++17 -D_DEFAULT_SOURCE -Wno-missing-braces

# Mode debug / release
BUILD_MODE ?= DEBUG
ifeq ($(BUILD_MODE),DEBUG)
    CFLAGS += -g -O0
else
    CFLAGS += -O1 -s
endif

# === INCLUDES ===
INCLUDE_PATHS = -I. \
    -Isrc \
    -Isrc/core \
    -Isrc/world \
    -Isrc/entities \
    -Isrc/utils \
    -Isrc/pugixml \
    -I$(RAYLIB_PATH)/src \
    -I$(RAYLIB_PATH)/src/external

# === SOURCES ===
OBJS = \
    src/main.cpp \
    src/core/Game.cpp \
    src/world/Map.cpp \
    src/world/Collision.cpp \
    src/entities/Player.cpp \
    src/utils/Utils.cpp \
    src/pugixml.cpp



# === LIBRAIRIES ===
LDFLAGS = -L$(RAYLIB_PATH)/src
LDLIBS = -lraylib -lopengl32 -lgdi32 -lwinmm

# === RESSOURCES ===
CFLAGS += $(RAYLIB_PATH)/src/raylib.rc.data

# === OPTIONS WINDOWS ===
export PATH := $(COMPILER_PATH):$(PATH)

# === COMPILATION ===
all: $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJS)
	@echo 🔧 Compilation du projet $(PROJECT_NAME)...
	$(CC) -o $(PROJECT_NAME).exe $(OBJS) $(CFLAGS) $(INCLUDE_PATHS) $(LDFLAGS) $(LDLIBS) -D$(PLATFORM)
	@echo ✅ Compilation terminée avec succès !

# === NETTOYAGE ===
clean:
	@echo 🧹 Suppression des fichiers compilés...
	del /Q src\core\*.o 2>nul || true
	del /Q src\world\*.o 2>nul || true
	del /Q src\entities\*.o 2>nul || true
	del /Q src\utils\*.o 2>nul || true
	del /Q $(PROJECT_NAME).exe 2>nul || true
	@echo ✅ Nettoyage terminé !

# === INFO ===
help:
	@echo "Commandes disponibles :"
	@echo "  mingw32-make BUILD_MODE=DEBUG   -> Compilation avec debug"
	@echo "  mingw32-make BUILD_MODE=RELEASE -> Compilation optimisée"
	@echo "  mingw32-make clean              -> Nettoyer les fichiers compilés"
