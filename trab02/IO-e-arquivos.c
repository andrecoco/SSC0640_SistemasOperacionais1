#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

/*
	Programa feito para mostrar a utilização
	de 3 system calls do Linux: access(), fstat() e open()
*/

//syscalls de E/S e arquivos
void sysAccess(char *pathname, int mode);
void sysFstat(char *path);


int main(int argc, char *argv[]) {
	char *path = argv[1]; 		//recebe o parâmetro pela linha de comando
	int mode = F_OK;			

	//sysFstat(path);			//descomente para utilizar a funcao
	//sysAccess(path, mode);	//descomente para utilizar a funcao
	return 0;
}


void sysAccess(char *pathname, int mode) {

	int value = access(pathname, mode);
	if (value == 0) {
		printf("%s existe!\n", pathname);
	} else {
		if (value == EACCES) {
			printf("Permissao negada para %s\n", pathname);
		} else {
			printf("%s nao existe!\n", pathname);
		}
	}
		
}


void sysFstat(char *path) {
	struct stat status;

	size_t fd = open(path, O_RDONLY);
	if (fd == -1) {
		printf("Não foi possível abrir o arquivo.\n");
		return;
	}

	fstat(fd, &status);
	printf("File type: %d\n", status.st_mode);
	printf("File size: %ld\n", status.st_size);
	printf("File UID: %d\n", status.st_uid);
	printf("File GID: %d\n", status.st_gid);

}
