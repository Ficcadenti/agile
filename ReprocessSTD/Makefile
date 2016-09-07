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

CFLAGS = -g -Wall -std=c++11 -L/usr/local/lib -I$(SUBDIR)/src -I$(SUBDIR)/inc 

LDFLAGS = 


SORGENTI 	=      $(SUBDIR)/src/main.cpp\
				   $(SUBDIR)/src/reprocess_std.cpp

HEADERS =       $(SUBDIR)/inc/reprocess_sdd.h			
                

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
