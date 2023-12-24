BUILD_DIR = build
SRCS = libserver.c server.c
OBJS = $(addprefix $(BUILD_DIR)/,$(SRCS:.c=.o))

.PHONY: all
all: $(BUILD_DIR)/server

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR)/server: $(OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir $(BUILD_DIR)
