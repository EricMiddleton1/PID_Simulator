CC = g++
LD = g++

LDFLAGS = -std=c++14 -Wall -Wextra -Werror -pedantic
CCFLAGS = $(LDFLAGS) -c
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

SRCDIR = src
OBJDIR = obj

EXE = PID


all: $(EXE)


$(EXE):	$(OBJDIR)/Rotor.o $(OBJDIR)/main.o
	$(LD) $(LDFLAGS) $(OBJDIR)/Rotor.o $(OBJDIR)/main.o -o $(EXE) $(LIBS)


$(OBJDIR)/Rotor.o:	$(SRCDIR)/Rotor.hpp $(SRCDIR)/Rotor.cpp
	$(CC) $(CCFLAGS) $(SRCDIR)/Rotor.cpp -o $(OBJDIR)/Rotor.o

$(OBJDIR)/main.o: $(SRCDIR)/main.cpp
	$(CC) $(CCFLAGS) $(SRCDIR)/main.cpp -o $(OBJDIR)/main.o
