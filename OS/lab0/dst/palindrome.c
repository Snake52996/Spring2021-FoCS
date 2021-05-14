#include<stdio.h>
int main()
{
	int n;
	scanf("%d",&n);
	FILE* table = fopen("table", "rb");
	fseek(table, n, SEEK_SET);
	if(fgetc(table)){
		printf("Y");
	}else{
		printf("N");
	}
	fclose(table);
	return 0;
}
