#----------------------------------------------------------------
#
#  Makefile for Reprocess STD
#
#  Author: Raffaele Ficcadenti
#
#----------------------------------------------------------------



#----------------------------------------------------------------
# Definitions:
#


CC = g++
CP = /usr/bin/cp
#PREFIX=/usr/agile
TARGET = reprocess_std
PREFIX=../agile-test
BINDIR=${PREFIX}/${TARGET}
SUBDIR=./

CFLAGS = -g -Wall -L/usr/local/lib -I$(SUBDIR)/src -I$(SUBDIR)/inc 

LDFLAGS = 


SORGENTI 	=      $(SUBDIR)/src/main.cpp

HEADERS =       
                

$(TARGET): $(SORGENTI)
	$(CC) $(CFLAGS) -o $(TARGET) $(LDFLAGS) $(SORGENTI) $(HEADERS)


all: $(TARGET)

clean:
	$(RM) $(TARGET)

realclean:
	$(RM) $(BINDIR)/$(TARGET)
	$(RM) $(TARGET)

install:
	$(CP) $(TARGET) $(BINDIR)
