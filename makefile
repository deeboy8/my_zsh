CC = gcc
CFLAGS  += -Wall -Werror -Wextra -g #-g3 -fsanitize=address
MAIN = dsh
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

ALL: $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(LINKERFLAG) $^


debugger: $(SRCS)
	$(CC) $(CFLAGS) -g -o $@ $(LINKERFLAG) $(SRCS)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(MAIN) $(DEBUG)

re: fclean all