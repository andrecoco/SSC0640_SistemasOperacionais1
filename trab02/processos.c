#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

/*
Código feito para utilizar as seguintes syscalls:
- execve()
- getuid()
- geteuid()
*/

int main(void) {
	int real = getuid();
	int effective = geteuid();
	printf("REAL UID: %d\n", real);
	printf("EFFECTIVE UID: %d\n", effective);

	return 0;
}