#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<limits.h>

// Add these functions to your file
int	ft_isalpha(int i)
{
	return ((i >= 'A' && i <= 'Z') || (i >= 'a' && i <= 'z'));
}

int	ft_isdigit(int i)
{
	return (i >= '0' && i <= '9');
}
int	ft_isalnum(int i)
{
	return (ft_isalpha(i) || ft_isdigit(i));
}
int	ft_atoi(const char *str)
{
	int	i;
	int	num;
	int	sign;

	i = 0;
	num = 0;
	sign = 1;
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == ' ')
		i++;
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		num = num * 10 + str[i] - '0';
		i++;
	}
	return (num * sign);
}

char	*ft_strchr(const char *s, int c)
{
	int	i;

	i = 0;
	while (s[i])
	{
		if (s[i] == (char)c)
			return ((char *)(s + i));
		i++;
	}
	if (s[i] == (char)c)
		return ((char *)(s + i));
	return (0);
}

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

size_t	ft_strlcpy(char *dst, const char *src, size_t dstsize)
{
	size_t	i;

	i = 0;
	if (!dstsize)
		return (ft_strlen(src));
	while (src[i] && i < (dstsize - 1))
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = 0;
	return (ft_strlen(src));
}

void	*ft_calloc(size_t count, size_t size)
{
	char	*m;
	size_t	i;

	if (count && size && count > (UINT_MAX / size))
		return (NULL);
	m = malloc(count * size);
	if (!m)
		return (0);
	i = -1;
	while (++i < size * count)
		m[i] = 0;
	return (m);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	s1_len;
	size_t	s2_len;
	char	*join;

	if (!s1 || !s2)
		return (NULL);
	s1_len = ft_strlen(s1);
	s2_len = ft_strlen(s2);
	join = (char *)malloc((s1_len + s2_len + 1) * sizeof(char));
	if (!join)
		return (NULL);
	ft_strlcpy(join, s1, s1_len + 1);
	ft_strlcpy((join + s1_len), s2, s2_len + 1);
	return (join);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*sub;
	size_t	new_len;

	if (!s)
		return (NULL);
	if (ft_strlen(s) < start)
	{
		sub = ft_calloc(1, sizeof(char));
		if (!sub)
			return (NULL);
	}
	else
	{
		new_len = ft_strlen(s + start);
		if (new_len >= len)
			new_len = len;
		sub = (char *)malloc((new_len + 1) * sizeof(char));
		if (!sub)
			return (NULL);
		sub[new_len] = 0;
		while (new_len-- > 0)
			sub[new_len] = s[start + new_len];
	}
	return (sub);
}

static int	numwords(char const *s, char c)
{
	int	i;
	int	words_count;

	i = 0;
	words_count = 0;
	while (s[i])
	{
		if (s[i] != c && (s[i + 1] == c || s[i + 1] == 0))
			words_count++;
		i++;
	}
	return (words_count);
}

static int	split_words(char **result, char const *s, char c, int word)
{
	int	i_start;
	int	i_end;

	i_end = 0;
	i_start = 0;
	while (s[i_end])
	{
		if (s[i_end] == c || s[i_end] == 0)
			i_start = i_end + 1;
		if (s[i_end] != c && (s[i_end + 1] == c || s[i_end + 1] == 0))
		{
			result[word] = (char *)malloc((i_end - i_start + 2) * sizeof(char));
			if (!result[word])
			{
				while (word++)
					free(result[word]);
				return (0);
			}
			ft_strlcpy(result[word], (s + i_start), (i_end - i_start + 2));
			word++;
		}
		i_end++;
	}
	result[word] = 0;
	return (1);
}

char	**ft_split(char const *s, char c)
{
	char	**result;

	if (!s)
		return (NULL);
	result = (char **)malloc((numwords(s, c) + 1) * sizeof(char *));
	if (!result)
		return (NULL);
	if (!split_words(result, s, c, 0))
		return (NULL);
	return (result);
}

char	*ft_strdup(const char *str)
{
	int		str_len;
	char	*dup;

	str_len = ft_strlen(str);
	dup = (char *)malloc((str_len + 1) * sizeof(char));
	if (!dup)
		return (0);
	dup[str_len] = 0;
	while (--str_len >= 0)
		dup[str_len] = str[str_len];
	return (dup);
}

int ft_strncmp(const char *s1, const char *s2, size_t n)
{
    size_t i;

    if (n == 0)
        return 0;
    for (i = 0; i < n; i++)
    {
        if (s1[i] != s2[i])
            return (unsigned char)s1[i] - (unsigned char)s2[i];
        if (s1[i] == '\0')
            return 0;
    }
    return 0;
}
typedef struct s_vars
{
    int             end;
    struct s_env    *env_list;
    struct s_list   *tokens;
    char            *args;
}   t_vars;

typedef struct  s_env
{
    int             index;
    char            *vari;
    char            *key;
    char            *val;
    struct s_env    *next;
    struct s_env    *prev;
}   t_env;

int init_env(int i, t_vars *vars, char **env)
{
    t_env *head;
    t_env *current;

    if (!env || !*env || !vars)
        return (1);
    head = (t_env *)malloc(sizeof(t_env));
    if (!head)
        return (1);
    current = head;
    current->prev = NULL;
    while (env[i])
    {
        current->vari = ft_strdup(env[i]);
        current->key = ft_split((env[i]), '=')[0];
        current->val = ft_split((env[i]), '=')[1];
        current->index = i++;
        current->next = (t_env *)malloc(sizeof(t_env));
        if (!current->next)
            return (1);
        current->next->prev = current;
        current = current->next;
    }
    current->next = NULL;
    vars->env_list = head;
    return (0);
}

char *get_env_val(char *arg, int *i, int *j, t_vars *vars)
{
    char *env_val;
    char *key;
    t_env *current;

    while (arg[*j] == 95 || ft_isalnum(arg[*j])) 
        (*j)++;
    if (*j == 0)
        return (ft_calloc(1, sizeof(char)));
    key = ft_substr(arg, *i, *j - *i);
	*i = *j;
    current = vars->env_list;
    while (current->key)
    {
        if (ft_strncmp(key, current->key, ft_strlen(current->key)) == 0)
            return (free(key), ft_strdup(current->val));
        current = current->next;
    }
    return (free(key), ft_calloc(1, sizeof(char)));
}

void    read_env(t_vars *vars)
{
    t_env *current;

    current = vars->env_list;
    while (current)
    {
        printf("key: %s, val: %s\n", current->key, current->val);
        current = current->next;
    }
}

int main(int ac, char **av, char **env)
{
    t_vars vars;
    char *env_val;
    int i = 0;
    int j = 0;

    (void)ac;
    (void)av;
    init_env(0, &vars, env);
    // read_env(&vars);
    env_val = get_env_val("", &i, &j, &vars);
    printf("env_val: %s\n", env_val);
	printf("i = %d, j = %d\n", i, j);
	char *join;
	join = ft_strjoin(env_val, "!");
	printf("|join:%s|\n", join);
    return (0);
}