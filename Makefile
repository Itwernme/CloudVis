NAME ?= CloudVis
VERSION ?= 0.1.1-alpha
# options: LINUX WINDOWS
PLATFORM ?= LINUX

SOURCE := program.c main.c utils.c shader.c gui.c ext/glad.c

LIBS += -lm -lraylib

IDIR := inc
LDIR := lib
SDIR := src
RDIR := res
BDIR := build

OUTPUT := $(NAME)_$(VERSION)

OBJS = $(patsubst %.c, $(SDIR)/%.o, $(SOURCE))

CC := gcc

ifeq ($(PLATFORM),WINDOWS)
    CC = x86_64-w64-mingw32-gcc
    OUTPUT = $(NAME)_$(VERSION).exe
endif

MAKE ?= make

ifeq ($(PLATFORM),WINDOWS)
    MAKE = mingw32-make
endif

CFLAGS += -g -std=gnu99 -Wall -Werror
CFLAGS += -I$(IDIR)

# -L$(LDIR)
all:
	$(MAKE) $(NAME)

$(NAME): $(OBJS)
	@echo -e 'Building...'
	@$(CC) -o $(BDIR)/$(OUTPUT) $(OBJS) $(CFLAGS) $(LIBS)
	@echo -e 'Copying resources...'
	@cp -rf $(RDIR) $(BDIR)/
	@echo -e 'Cleaning up...'
	@find . -name "*.o" -type f -delete
	@echo -e 'Running...'
	@./$(BDIR)/$(OUTPUT)

%.o: %.c
	@$(CC) -c $< -o $@ $(CFLAGS)

.PHONY: clean

clean:
	@find . -name "*.o" -type f -delete
	@rm -rf $(BDIR)/*
