# include "minishell.h"

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