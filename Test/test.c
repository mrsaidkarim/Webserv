#include <stdio.h>

int displayNumber() {

	static int n;

	printf("the number is :%d\n", n);
	return (n++);
}


int main() {
	
	while (displayNumber() < 100)
		;
	
}