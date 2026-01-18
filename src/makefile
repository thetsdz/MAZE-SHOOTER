# -----------------------------------------------------------------------------
# MAKEFILE FINAL — Raylib (Windows / Linux)
# -----------------------------------------------------------------------------

PROJECT_NAME = game
CC = gcc

SRCS = main.c level.c player.c projectile.c asset.c

CFLAGS = -Wall -std=c99 -Wno-missing-braces -I.

# -----------------------------------------------------------------------------
# DÉTECTION DE L'OS
# -----------------------------------------------------------------------------

ifeq ($(OS),Windows_NT)

    # ---------------- WINDOWS (MinGW / MSYS2) ----------------
    PLATFORM = WINDOWS
    RAYLIB_DIR = ../lib/windows/raylib

    CFLAGS  += -I$(RAYLIB_DIR)/include
    LDFLAGS = -L$(RAYLIB_DIR)/lib \
              -lraylib -lopengl32 -lgdi32 -lwinmm

    EXT = .exe
    RM = del /Q

else

    UNAME_S := $(shell uname -s)

    ifeq ($(UNAME_S),Linux)

        # ---------------- LINUX ----------------
        PLATFORM = LINUX
        RAYLIB_DIR = ../lib/linux/raylib

        CFLAGS  += -I$(RAYLIB_DIR)/include
        LDFLAGS = -L$(RAYLIB_DIR)/lib \
                  -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

        EXT =
        RM = rm -f

    endif
endif

# -----------------------------------------------------------------------------
# RÈGLES
# -----------------------------------------------------------------------------

all:
	$(CC) $(SRCS) -o $(PROJECT_NAME)$(EXT) $(CFLAGS) $(LDFLAGS)

run: all
	./$(PROJECT_NAME)$(EXT)

clean:
	$(RM) $(PROJECT_NAME)$(EXT)

info:
	@echo "Platform: $(PLATFORM)"
	@echo "Raylib:   $(RAYLIB_DIR)"
