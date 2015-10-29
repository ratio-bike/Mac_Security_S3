#include <stdio.h>

int main()
{
	int *test = 0x1000f2804;
	int res = *test;

	printf("%0x\n",res);
}
