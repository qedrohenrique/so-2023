#include <stdio.h>

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

int main(int argc, char** argv){

	if(argc==1){
		printf("Uso: <comando> <arg1> ... < argn>.\n");
		return 0;
	}

	int operator_position;
	operator_position = get_operator_position(argv);	

	return 0;
}