#ifndef LIB_FT_H
# define LIB_FT_H

# include <stddef.h> // For size_t
# include <stdlib.h> // For malloc, free
# include <unistd.h> // For write, read

// String manipulation functions
size_t  ft_strlen(const char *str);
char    *ft_strdup(const char *s1);
char    *ft_strcpy(char *dst, const char *src);
char    *ft_strncpy(char *dst, const char *src, size_t len);
int     ft_strcmp(const char *s1, const char *s2);
int     ft_strncmp(const char *s1, const char *s2, size_t n);
char    *ft_strjoin(const char *s1, const char *s2);
char    *ft_strchr(const char *s, int c);
char    *ft_strrchr(const char *s, int c);
char    **ft_split(const char *s, char c);

// Memory manipulation functions
void    *ft_memset(void *b, int c, size_t len);
void    ft_bzero(void *s, size_t n);
void    *ft_memcpy(void *dst, const void *src, size_t n);
void    *ft_memmove(void *dst, const void *src, size_t len);
int     ft_memcmp(const void *s1, const void *s2, size_t n);
void    *ft_calloc(size_t count, size_t size);

// Character checks and conversions
int     ft_isalpha(int c);
int     ft_isdigit(int c);
int     ft_isalnum(int c);
int     ft_isascii(int c);
int     ft_isprint(int c);
int     ft_toupper(int c);
int     ft_tolower(int c);

// File descriptor functions
void    ft_putchar_fd(char c, int fd);
void    ft_putstr_fd(const char *s, int fd);
void    ft_putendl_fd(const char *s, int fd);
void    ft_putnbr_fd(int n, int fd);

// Conversion functions
int     ft_atoi(const char *str);
char    *ft_itoa(int n);

#endif