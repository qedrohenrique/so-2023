#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <strings.h>

int get_operator_position(char** argv){
	int i = 0;
	while(argv[i]) {
		if (argv[i][0] == '|' || // Isso inclui o operador "||" 
			argv[i][0] == '<' || 
			argv[i][0] == '>' || // Isso inclui o operador ">>"
			argv[i][0] == '&'	 // Isso inclui o operador "&&"
			){
			return i;
		}
		i++;
	}
	return -1;
}

void imprime_argv(char **argv) {
	int i = 0;
	if (argv[i]) printf("===\n");
	while(argv[i]) {
		printf("%d: '%s'\n", i, argv[i]);
		i++;
	}
}

int main(int argc, char** argv){

	if(argc==1){
		printf("Uso: <comando> <arg1> ... < argn>.\n");
		return 0;
	}

	int operator_position;
	operator_position = get_operator_position(argv);

	char **cmd1, **cmd2;
	cmd1 = &argv[1];
	cmd2 = &argv[operator_position+1];

	// Comando unico
	if(operator_position == -1){
		execvp(cmd1[0], cmd1);
	}

	int fd1[2], fd2[2];
	if(pipe(fd1) == -1 || pipe(fd2) == -1){
		perror("pipe()");
		return -1;
	}

	return 0;
}