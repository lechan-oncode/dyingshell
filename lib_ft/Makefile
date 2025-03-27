# Name of the library
NAME = libft.a

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror

# Archiver
AR = ar rcs

# Source files and object files
SRCS = *.c
OBJS = $(SRCS:.c=.o)

# Default target
all: $(NAME)

# Rule to create the library
$(NAME): $(OBJS)
	$(AR) $(NAME) $(OBJS)

# Clean object files
clean:
	@rm -f $(OBJS)

# Clean everything (object files and library)
fclean: clean
	@rm -f $(NAME)

# Rebuild everything
re: fclean all

# Phony targets
.PHONY: all clean fclean re