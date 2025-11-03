CC = gcc
EXEC = replaygame
LIBS = -lncurses -lm
INC_PATH = ../cli-lib/include
LIB_SRC_PATH = ../cli-lib/src
CFLAGS = -Wall -I$(INC_PATH)


SRCS = main.c


LIB_SRCS = $(LIB_SRC_PATH)/screen.c $(LIB_SRC_PATH)/keyboard.c $(LIB_SRC_PATH)/timer.c


OBJS = $(SRCS:.c=.o)
LIB_OBJS = $(LIB_SRCS:.c=.o)


all: $(EXEC)

$(EXEC): $(OBJS) $(LIB_OBJS)
	$(CC) $(OBJS) $(LIB_OBJS) -o $(EXEC) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


$(LIB_SRC_PATH)/%.o: $(LIB_SRC_PATH)/%.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(EXEC) $(OBJS) $(LIB_OBJS)
