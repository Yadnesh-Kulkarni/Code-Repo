#include<stdio.h>
#include<stdlib.h>
#include<dlfcn.h>

typedef int (*makeSquare)(int);
makeSquare pfn = NULL;

int main()
{
	int result;
	void *lib = NULL;
	lib = dlopen("./square.so", RTLD_LAZY);
	if(lib == NULL)
	{
		printf("ERROR: Could not load library\n");
		printf("%s",dlerror());
		exit(0);
	}

	pfn = (makeSquare) dlsym(lib, "makeSquare");
	if(pfn == NULL)
	{
		printf("ERROR: Could not get function address\n");
		exit(0);
	}
	result = pfn(25);

	printf("Square of 25 = %d\n",result);

	return(0);
}