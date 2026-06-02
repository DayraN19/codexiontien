NAME    = codexion

CC      = cc
CFLAGS  = -Wall -Wextra -Werror -pthread

SRCS    = srcs/main.c      \
          srcs/utils.c     \
          srcs/log.c       \
          srcs/scheduler.c \
          srcs/dongle.c    \
          srcs/coder.c     \
          srcs/monitor.c   \
          srcs/init.c

OBJS    = $(SRCS:.c=.o)

INCLUDES = -I includes

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re