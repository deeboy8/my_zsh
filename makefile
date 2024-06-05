CC = gcc
CFLAGS  += -Wall -Werror -Wextra -g -g3 -fsanitize=address
MAIN = zsh
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

all: $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(LINKERFLAG) $^


debugger: $(SRCS)
	$(CC) $(CFLAGS) -g -o $@ $(LINKERFLAG) $(SRCS)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(MAIN) $(DEBUG)

re: fclean all