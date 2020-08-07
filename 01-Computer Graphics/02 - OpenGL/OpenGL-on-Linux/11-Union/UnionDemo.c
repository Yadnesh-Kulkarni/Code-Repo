#include<stdio.h>
#include<stdlib.h>

union {
	int intVal;
	float floatVal;
	char stringVal[10];
} u;

int main(void)
{
	printf("Enter integer value to store in union.\n");
	scanf("%d",&u.intVal);

	printf("Value stored in union at this point is..%d\n\n", u.intVal);

	printf("Enter float value to store in union.\n");
	scanf("%f",&u.floatVal);

	printf("Value stored in union at this point is..%f\n\n", u.floatVal);	

	printf("Enter string value to store in union.\n");
	scanf("%s",u.stringVal);

	printf("Value stored in union at this point is..%s\n\n", u.stringVal);
}

