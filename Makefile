SRCS = libserver.c server.c
OBJS = $(SRCS:.c=.o)

all: server

server: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@
