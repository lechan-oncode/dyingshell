NAME = minishell
CC = gcc
CFLAGS = -Wall -Wextra -Werror
SRCS = minishell.c libft.c
OBJS = $(SRCS:.c=.o)
READLINE = -lreadline

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(READLINE) -o $(NAME)

clean:
	rm -f $(OBJS)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	rm -f $(NAME)
	
re: fclean all

.PHONY: all clean fclean re