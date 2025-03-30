# include <stdio.h>

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
	return (NULL);
}

#include <stdio.h>
int main()
{
	char str[] = "Hollaloly";
    if (ft_strchr(str, 'z') == NULL)
        printf("NULL\n");
    else
        printf("%s\n", ft_strchr(str, 'z'));
	return (0);
}
