############################################################################
BIN_DIR = ../bin

APP_INCL = -I../include

ORA_LIB  = -L$(ORACLE_HOME)/lib -lclntsh

SHARE_DIR  = /usr/local/aibase2/libmm
SHARE_INCL = -I$(SHARE_DIR)/include
SHARE_LIB  = -L$(SHARE_DIR)/lib64 -laibase2

############################################################################

CPP = g++
CPP_FLAGS = -g -m64 -Wall -O2 -DLINUX

INCLS = $(APP_INCL) $(SHARE_INCL)
LIBS  = $(ORA_LIB) $(SHARE_LIB)

############################################################################
CP = cp
MV = mv
RM = rm -f

