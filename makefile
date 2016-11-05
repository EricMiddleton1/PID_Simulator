CC = g++
LD = g++

LDFLAGS = -std=c++14 -Wall -Wextra -pedantic
CCFLAGS = $(LDFLAGS) -c
LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lpthread

SRCDIR = src
OBJDIR = obj

EXE = PID


all: $(EXE)


$(EXE):	$(OBJDIR)/Rotor.o $(OBJDIR)/Logger.o $(OBJDIR)/main.o
	$(LD) $(LDFLAGS) $(OBJDIR)/Rotor.o $(OBJDIR)/Logger.o $(OBJDIR)/main.o \
	-o $(EXE) $(LIBS)


$(OBJDIR)/Rotor.o:	$(SRCDIR)/Rotor.hpp $(SRCDIR)/Rotor.cpp
	$(CC) $(CCFLAGS) $(SRCDIR)/Rotor.cpp -o $(OBJDIR)/Rotor.o

$(OBJDIR)/Logger.o:	$(SRCDIR)/Logger.hpp $(SRCDIR)/Logger.cpp
	$(CC) $(CCFLAGS) $(SRCDIR)/Logger.cpp -o $(OBJDIR)/Logger.o

$(OBJDIR)/main.o: $(SRCDIR)/main.cpp
	$(CC) $(CCFLAGS) $(SRCDIR)/main.cpp -o $(OBJDIR)/main.o
