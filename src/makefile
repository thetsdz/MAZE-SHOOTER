# -----------------------------------------------------------------------------
# MAKEFILE ADAPTÉ (Game)
# -----------------------------------------------------------------------------

PROJECT_NAME = game
CC = gcc

# CFLAGS : -I permet d'ajouter le dossier d'include (optionnel si tu mets le chemin complet dans le .c, mais plus propre)
CFLAGS = -Wall -std=c99 -Wno-missing-braces -I. -I$(RAYLIB_DIR)/include

# LISTE DES FICHIERS SOURCE
SRCS = main.c level.c player.c projectile.c

# DÉTECTION OS & LIBRAIRIES
# On ajoute -L pour dire au compilateur où chercher le fichier binaire de raylib
ifeq ($(OS),Windows_NT)
    # Sur Windows, on cherche souvent dans lib/
    LDFLAGS = -L$(RAYLIB_DIR)/lib -lraylib -lopengl32 -lgdi32 -lwinmm
    EXT = .exe
    RM = del /Q
    RAYLIB_DIR = ../lib/raylib/windows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        # Sur Linux, il faut souvent lier explicitement m (math), pthread, etc.
        LDFLAGS = -L$(RAYLIB_DIR)/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
        EXT = 
        RM = rm -f
        RAYLIB_DIR = ../lib/raylib/linux
    endif
    ifeq ($(UNAME_S),Darwin)
        # MacOS
        LDFLAGS = -L$(RAYLIB_DIR)/lib -lraylib -framework IOKit -framework Cocoa -framework OpenGL
        EXT = 
        RM = rm -f
        RAYLIB_DIR = ../lib/raylib/linux
    endif
endif

# RÈGLES DE COMPILATION

# 1. Règle par défaut
all:
	$(CC) $(SRCS) -o $(PROJECT_NAME)$(EXT) $(CFLAGS) $(LDFLAGS)

# 2. Lancer le jeu après compilation
run: all
	./$(PROJECT_NAME)$(EXT)

# 3. Nettoyer les fichiers générés
clean:
	$(RM) $(PROJECT_NAME)$(EXT)