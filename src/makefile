# ==========================================
# CONFIGURATION DU COMPILATEUR
# ==========================================
CC = gcc

# Flags de compilation :
# -Wall -Wextra : Affiche tous les avertissements (bonnes pratiques)
# -g : Ajoute les infos de débogage (utile pour gdb/valgrind)
# -I. : Cherche les .h dans le dossier courant
# -I../lib/raylib/include : Cherche les headers Raylib
CFLAGS = -Wall -Wextra -g -I. -I../lib/raylib/include

# Flags de l'éditeur de liens (Linker) :
# -L../lib/raylib/lib : Cherche la librairie compilée (.a) dans ce dossier
LDFLAGS = -L../lib/raylib/lib

# Bibliothèques à lier :
# -lraylib : La librairie Raylib
# -lm : Math library (indispensable pour la 3D)
# -lpthread -ldl -lrt -lX11 : Dépendances système pour Raylib sous Linux
LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Nom de l'exécutable final
EXEC = jeu

# Liste des fichiers sources
SRC = main.c pile.c labyrinthe.c

# Liste des fichiers objets (remplace .c par .o)
OBJ = $(SRC:.c=.o)

# ==========================================
# RÈGLES DE COMPILATION
# ==========================================

# Règle par défaut (lancée par la commande "make")
all: $(EXEC)

# Édition de liens : Crée l'exécutable à partir des objets
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS) $(LDLIBS)

# Compilation générique des fichiers .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ==========================================
# GESTION DES DÉPENDANCES
# ==========================================
# Ici, on précise quel fichier dépend de quel header.
# Si un .h change, on recompilera le .c associé.

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

pile.o: pile.c pile.h
	$(CC) $(CFLAGS) -c pile.c -o pile.o

labyrinthe.o: labyrinthe.c pile.h fonctionslabyrinthe.h
	$(CC) $(CFLAGS) -c labyrinthe.c -o labyrinthe.o

# ==========================================
# NETTOYAGE
# ==========================================

# Supprime les fichiers temporaires ("make clean")
clean:
	rm -f $(OBJ) $(EXEC)

# Indique que 'all' et 'clean' ne sont pas des fichiers réels
.PHONY: all clean