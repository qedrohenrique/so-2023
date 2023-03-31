#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <strings.h>

#define OPERATOR_OR 0
#define OPERATOR_AND 1
#define OPERATOR_PIPE 2
#define OPERATOR_BACKGROUND 3
#define OPERATOR_INPUT 4
#define OPERATOR_OUTPUT_OVERWRITE 5
#define OPERATOR_OUTPUT_APPEND 6

/*
	Retorna a posicao do primeiro operador da matriz de argumentos, dada uma posicao inicial.
*/
int get_operator_position(int pos, char** argv){
	while(argv[pos]) {
		if (argv[pos][0] == '|' || // Isso inclui o operador "||" 
			argv[pos][0] == '<' || 
			argv[pos][0] == '>' || // Isso inclui o operador ">>"
			argv[pos][0] == '&'	 // Isso inclui o operador "&&"
			){
			return pos;
		}
		pos++;
	}
	return -1;
}

/*
	Retorna o numero de operadores na matriz de argumentos.
*/
int count_operators(char **argv){
    int i = 0, count = 0;
    while (argv[i]){
        if (argv[i][0] == '|' || // Isso inclui o operador "||" 
			argv[i][0] == '<' || 
			argv[i][0] == '>' || // Isso inclui o operador ">>"
			argv[i][0] == '&'	 // Isso inclui o operador "&&"
			) count++;
        i++;
    }
    return count;
}

/*
	Retorna o tipo de um operador, dada uma posicao.
*/
int get_operator_type(int pos, char** argv){
	if(argv[pos][0] == '|' && argv[pos][1] == '|') return OPERATOR_OR;
	if(argv[pos][0] == '&' && argv[pos][1] == '&') return OPERATOR_AND;
	if(argv[pos][0] == '|') return OPERATOR_PIPE;
	if(argv[pos][0] == '&') return OPERATOR_BACKGROUND;
	if(argv[pos][0] == '<') return OPERATOR_INPUT;
	if(argv[pos][0] == '>') return OPERATOR_OUTPUT_OVERWRITE;
	if(argv[pos][0] == '>' && argv[pos][1] == '>') return OPERATOR_OUTPUT_APPEND;
	return -1;
}

/*
	Imprime a matriz de argumentos.
*/
void imprime_argv(char **argv, int argc) {
	int i = 0;
	if (argv[i]) printf("===\n");
	while(i != argc) {
		printf("%d: '%s'\n", i, argv[i]);
		i++;
	}
}

int main(int argc, char** argv){

	if(argc==1){
		printf("Uso: <comando> <arg1> ... <argn>.\n");
		return 0;
	}

	char** cmd = &argv[1];

	// Comando simples (sem nenhum operador)
	if(get_operator_position(0, argv) == -1){
		execvp(cmd[0], cmd);
	}

	// Comando com operadores
	else{

		int fd[2], command_pos = 1, operator_pos;
		int aux = STDIN_FILENO;
		int num_op = count_operators(argv);
		int status = -1; // Nao executado ainda.

		int operator = -1;

		for(int i = 0; i < num_op+1; i++){
			operator_pos = get_operator_position(command_pos, argv);
			cmd = &argv[command_pos];

			if (operator_pos != -1) {
				operator = get_operator_type(operator_pos, argv);
				cmd[operator_pos - command_pos] = NULL;
			}
			
			//printf("%s - %d com %d numero de argumentos.\n", argv[command_pos], command_pos, operator_pos-command_pos-1);

			if (pipe(fd) < 0){
	            perror("pipe()");
	            return -1;
	        }

			pid_t p_id = fork();
			
			if (p_id == 0){    
				// printf("Filho: Processo (%d) - Comando (%s) %s\n", getpid(), cmd[0], cmd[1]);
	            close(fd[0]);            
	            dup2(aux, STDIN_FILENO); 
	            if (i < num_op) dup2(fd[1], STDOUT_FILENO); 
	            execvp(cmd[0], cmd);
	            return 0;
	        }
	        else if (p_id > 0){ 
				// printf("Pai: Processo (%d) - Comando (%s) %s\n", getpid(), cmd[0], cmd[1]);

				switch(operator){
	            	case OPERATOR_BACKGROUND:
	            		aux = fd[0];
			            close(fd[1]);
			            waitpid(p_id, &status, WNOHANG);
            		case OPERATOR_PIPE:
            			aux = fd[0];
			            close(fd[1]);
			            waitpid(p_id, &status, 0);
	            		break;
            		default:
            			break;
	            }

	            
    			}else{
	            perror("fork()");
	            return -1;
	        }

	        command_pos = operator_pos + 1;
	    }
	}	

	return 0;
}