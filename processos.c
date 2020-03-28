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
	int euid = geteuid();
	printf("The REAL UID =: %d\n", real);
	printf("The EFFECTIVE UID =: %d\n", euid);
}