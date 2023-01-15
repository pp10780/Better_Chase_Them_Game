OBJDIR = obj
OUTDIR = bin

OBJS = chase_server.o init_server.o movement.o utils_server.o chase_client.o global_var.o draw.o threads.o
OUT	= server, client

OBJS0	= $(OBJDIR)/chase_server.o $(OBJDIR)/init_server.o $(OBJDIR)/movement.o $(OBJDIR)/utils_server.o $(OBJDIR)/global_var.o $(OBJDIR)/draw.o $(OBJDIR)/threads.o
SOURCE0	= chase_server.c init_server.c movement.c utils_server.c global_var.c draw.c threads.c
HEADER0	= defines.h init_server.h movement.h structs.h utils_server.h global_var.h draw.h threads.h
OUT0	= $(OUTDIR)/server
SRCDIR0 = Server

OBJS1	= $(OBJDIR)/chase_client.o
SOURCE1	= chase_client.c
HEADER1	= defines.h structs.h
OUT1	= $(OUTDIR)/client
SRCDIR1 = Player_client

CC	 = gcc
FLAGS = -g -c -Wall


all: server client
server: $(OBJS0) $(LFLAGS)
	$(CC) -g $(OBJS0) -o $(OUT0) -lpthread -lncurses

client: $(OBJS1) $(LFLAGS)
	$(CC) -g $(OBJS1) -o $(OUT1) -lncurses


$(OBJDIR)/chase_server.o: $(SRCDIR0)/chase_server.c
	$(CC) $(FLAGS) $(SRCDIR0)/chase_server.c -o $(OBJDIR)/chase_server.o

$(OBJDIR)/init_server.o: $(SRCDIR0)/init_server.c
	$(CC) $(FLAGS) $(SRCDIR0)/init_server.c -o $(OBJDIR)/init_server.o

$(OBJDIR)/movement.o: $(SRCDIR0)/movement.c
	$(CC) $(FLAGS) $(SRCDIR0)/movement.c -o $(OBJDIR)/movement.o

$(OBJDIR)/utils_server.o: $(SRCDIR0)/utils_server.c
	$(CC) $(FLAGS) $(SRCDIR0)/utils_server.c -o $(OBJDIR)/utils_server.o

$(OBJDIR)/global_var.o: $(SRCDIR0)/global_var.c
	$(CC) $(FLAGS) $(SRCDIR0)/global_var.c -o $(OBJDIR)/global_var.o

$(OBJDIR)/draw.o: $(SRCDIR0)/draw.c
	$(CC) $(FLAGS) $(SRCDIR0)/draw.c -o $(OBJDIR)/draw.o

$(OBJDIR)/threads.o: $(SRCDIR0)/threads.c
	$(CC) $(FLAGS) $(SRCDIR0)/threads.c -o $(OBJDIR)/threads.o

$(OBJDIR)/chase_client.o: $(SRCDIR1)/chase_client.c
	$(CC) $(FLAGS) $(SRCDIR1)/chase_client.c -o $(OBJDIR)/chase_client.o


clean:
	rm -f $(OBJDIR)/*.o $(OUTDIR)/*