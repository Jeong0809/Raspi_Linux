#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
		srand((unsigned int)time(NULL));
		printf("난수 : %d\n", rand());
		printf("난수 : %d\n", rand());
		printf("난수 : %d\n", rand());
		printf("난수 : %d\n", rand());
		printf("난수 : %d\n", rand());
		printf("난수 : %d\n", rand());
		return 0;
}
