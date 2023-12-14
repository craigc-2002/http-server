all: server


server: server.c
	$(CC) $(CFLAGS) $^ -o $@
