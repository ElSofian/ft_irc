# include "Utils.hpp"

int	atoi(char *str) {
	int		result = 0;
	int		sign = 1;
	int		i = 0;

	if (str[i] == '-') {
		sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9') {
		result = result * 10 + str[i] - '0';
		i++;
	}
	return (result * sign);
}