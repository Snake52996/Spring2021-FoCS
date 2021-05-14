#include<stdio.h>
int main(){
	char result[10001];
	char buffer[8];
	int l, r;
	int temp_result;
	for(int i = 1; i <= 10000; ++i){
		r = sprintf(buffer, "%d", i) - 1;
		temp_result = 1;
		for(l = 0; l < r; ++l, --r) if(buffer[l] != buffer[r]){
			temp_result = 0;
			break;
		}
		result[i] = temp_result;
	}
	FILE* f = fopen("table", "wb");
	fwrite(result, 1, 10001, f);
	fclose(f);
	return 0;
}
