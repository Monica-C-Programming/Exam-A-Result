INCLDIR = ./include
CC = gcc
CFLAGS = -O2 -include stdio.h
CFLAGS += -I$(INCLDIR)

OBJDIR = obj

_DEPS = task2_count.h task2_hash.h task2_sum.h task2_metadata.h #List all header files here
DEPS = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

_OBJS = main.o task2_count.o task2_hash.o task2_sum.o #List all files here
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJS) #Edit the output name here
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~


