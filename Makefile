CC = gcc
LDFLAGS =
BLDDIR = .
INCDIR = $(BLDDIR)/inc
SRCDIR = $(BLDDIR)/src
OBJDIR = $(BLDDIR)/obj
CFLAGS = -c -Wall -I$(INCDIR)
SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
EXE = bin/bin

all: clean $(EXE) 
    
$(EXE): $(OBJ) 
	$(CC) $(LDFLAGS) $(OBJDIR)/*.o -o $@ -lwiringPi

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@ -lm -lpthread -lwiringPi

clean:
	-rm -f $(OBJDIR)/*.o $(EXE)

run:
	${EXE}
