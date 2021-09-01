#include <string.h>
#include <stdbool.h>

int indexOf(char ch, char str[], int order)
{
	unsigned int i, numberOfOccurence = 0;
	for (i = 0; i < strlen(str); i++)
	{
		if (str[i] == ch)numberOfOccurence++;
		if (numberOfOccurence == order)return i;
	}
	return -1;
}



int lastIndexOf(char ch, char str[])
{
	unsigned int i = strlen(str) - 1;
	while (i >= 0)
	{
		if (str[i] == ch)return i;
		i--;
	}
	return -1;
}

void substring(char str[], char substringBuffer[], int firstIndex, int lastIndex)
{
	int i = 0;
	while (i < lastIndex - firstIndex)
	{
		substringBuffer[i] = str[i + firstIndex];
		i++;
	}
	substringBuffer[i] = '\0';
}

int frequency(char ch, char str[])
{
	int i, f = 0;
	for (i = 0; str[i] != '\0'; i++)
	{
		if (str[i] == ch)f++;
	}
	return f;
}

bool startsWith(char pre[], char str[])
{
	int lenpre = strlen(pre), lenstr = strlen(str);
	return lenstr < lenpre ? false : _strnicmp(pre, str, lenpre) == 0;
}

