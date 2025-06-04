#include "betterc.hpp"

int log2ceil(int input)
{
	if (input == 0)
		return 0;
	if (input < 0)
		input *= -1;
	int i = 1;
	while ((i < input) && (i > 0))
	{
		i <<= 1;
	}
	if (i < 0)
		return 0;
	else
		return i;
}
