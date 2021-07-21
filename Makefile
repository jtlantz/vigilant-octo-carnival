#
# Makefile that builds btest and other helper programs for the CS:APP data lab
# 
SRC_DIR := src
OBJ_DIR := obj
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(OBJ_DIR)/mm.o
BIN := mm
#C compiler
CC = gcc
#compiler flags
CFLAGS = -O -Wall -m64 -pthread
#dependencies
LIBFLAGS := -pthread -lpthread

default: all
all: mm

mm: $(OBJ)
	$(CC) $^ -o $@ $(LIBFLAGS) 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


$(OBJ_DIR):
	mkdir $@

clean:
	$(RM) $(OBJ) $(BIN)
	$(RM) -r $(OBJ_DIR)
