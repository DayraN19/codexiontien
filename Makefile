NAME    = codexion

CC      = cc
CFLAGS  = -Wall -Wextra -Werror -pthread

SRCS    = main.c \
          utils.c \
          log.c \
          scheduler.c \
          dongle.c \
          coder.c \
          monitor.c \
          init.c \
          coder_utils.c \
          init_utils.c \
          scheduler_utils.c

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