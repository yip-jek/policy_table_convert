############################################################################
BIN_DIR = ../bin

APP_INCL = -I../include

############################################################################
ifeq ($(shell uname -m), i686) # 32 bit OS
OS_BITS = 32

SHARE_DIR  = ../aibase2/libmm
SHARE_INCL = -I$(SHARE_DIR)/include
SHARE_LIB  = -L$(SHARE_DIR)/lib -laibase2
else # 64 bit OS
OS_BITS = 64

SHARE_DIR  = /usr/local/aibase2/libmm
SHARE_INCL = -I$(SHARE_DIR)/include
SHARE_LIB  = -L$(SHARE_DIR)/lib64 -laibase2
endif

CPP = g++
CPP_FLAGS = -g -m$(OS_BITS) -Wall -O2 -DLINUX

INCLS = $(APP_INCL) $(SHARE_INCL)
LIBS  = $(SHARE_LIB)

############################################################################
CP = cp
MV = mv
RM = rm -f

