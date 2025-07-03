INCLDIR = ./include
CC = gcc
CFLAGS = -O2
CFLAGS += -I$(INCLDIR)

OBJDIR = obj

_DEPS = task4.h utils.h#List all header files here
DEPS = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

_OBJS = task4_threads.o dbj2.o tea.o #List all files here
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

task4_threads: $(OBJS) #Edit the output name here
	gcc -o $@ $^  $(CFLAGS) -pthread -lrt

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~


