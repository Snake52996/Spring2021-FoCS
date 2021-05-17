extern void _my_putchar(char);
extern void _my_exit(void);
void my_print(int, int, int*);

static void my_print_helper(int value){
	if(value / 10) my_print_helper(value / 10);
	_my_putchar(value % 10 + '0');
}
void my_print(int start, int end, int* arr){
	int i = start;
	for(; i <= end; ++i){
		my_print_helper(arr[i]);
		_my_putchar(' ');
	}
}
