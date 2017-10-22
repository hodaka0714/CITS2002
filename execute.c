#include "myshell.h"

/*
   CITS2002 Project 2 2017
   Name(s):		student-name1 (, student-name2)
   Student number(s):	student-number-1 (, student-number-2)
   Date:		date-of-submission
 */

// -------------------------------------------------------------------

bool isnumber(char *str){
	bool judge = true;
	for(int i = 0; i < strlen(str); i++){
		if(str[i] < 48 || str[i] > 57){
			judge = false;
		}
	} 
	return judge;
}

bool containsslash(char *str){
	bool judge = false;
	for(int i = 0; i < strlen(str); i++){
		if(str[i] == '/'){
			judge = true;
		}
	}	
	return judge;
}

//This function create child process.
void each_cmd(SHELLCMD *t){
	int pid = fork();

	// ensure that a new process was created
	if(pid == -1) {                             // process creation failed
		printf("fork() failed. (pid == -1)\n" );
		exit(EXIT_FAILURE);
	}
	
	else if(pid == 0){

//		char *step1_arguments[20];
//		char *step2_arguments[20];
//		int a = 0, b = 0;
//		step1_arguments[a++] = "/bin/ls";
//		step1_arguments[a++] = NULL;		
//		step2_arguments[b++] = "/usr/bin/cal";
//		step2_arguments[a++] = NULL;
//		step2_arguments[b++] = "-y";
//	        execv(step1_arguments[0],step1_arguments);
//	        execv(step2_arguments[0],step2_arguments);
		bool hasslash = containsslash(t->argv[0]);
		int cmd_max_len = strlen(PATH) + strlen(t->argv[0]) + strlen("/");
		char cmd[cmd_max_len];
		if(hasslash){
			if(t->argv[0][0] == '/'){
				execv(t->argv[0],t->argv);
			}else{
				strcat(cmd,"/");
				strcat(cmd,t->argv[0]);
				strcpy(t->argv[0],cmd);
				execv(t->argv[0],t->argv);
			}
			printf("execv() failed. (pid == 0)\n");
	        	exit(EXIT_FAILURE);	
		}
		else{
			
			const char s[2] = ":";
			char *token;
			
			token = strtok(PATH,s);
			while(token != NULL){
				memset(cmd, '\0', cmd_max_len);
				strcat(cmd, token);
				strcat(cmd, "/");
				strcat(cmd, t->argv[0]);
	//			printf("%s\n",cmd);
				execv(cmd, t->argv);
				token = strtok(NULL, s);
			}
			printf("execv() failed. (pid == 0)\n");
	        	exit(EXIT_FAILURE);	
		}
	}
	else{
		int status;
		while(wait(&status) > 0){
			
		}	
		
	}
	fflush(stdout);
}


//  THIS FUNCTION SHOULD TRAVERSE THE COMMAND-TREE and EXECUTE THE COMMANDS
//  THAT IT HOLDS, RETURNING THE APPROPRIATE EXIT-STATUS.
//  READ print_shellcmd0() IN globals.c TO SEE HOW TO TRAVERSE THE COMMAND-TREE

int execute_shellcmd(SHELLCMD *t)
{
	int  exitstatus;
	if(t == NULL) {			// hmmmm, that's a problem
		exitstatus	= EXIT_FAILURE;
	}
	else if(strcmp(t->argv[0],"exit") == 0){ // if exit is typed
		int tmp_exitstatus = 0;// the exit-status of the recently executed command should be assigned
		if(t->argv[1] == NULL){ // if there was no argument
			exitstatus	= EXIT_SUCCESS;	
			exit(tmp_exitstatus); //exit-status of the recently executed command is used
		}
		else{ // if argument is typed
			if(strcmp(t->argv[1],"EXIT_SUCCESS") == 0) strcpy(t->argv[1],"0\0");
			// if EXIT_SUCCESS is typed at argv[1], then t->argv[1] is replaced as (string)0.

			if(strcmp(t->argv[1],"EXIT_FAILURE") == 0) strcpy(t->argv[1],"1\0");
			// if EXIT_FAILURE is typed at argv[1], then t->argv[1] is replaced as (string)1.
			
			if(isnumber(t->argv[1])){ // if the argument is a number
				tmp_exitstatus = atoi(t->argv[1]); // convert from string to int
				exitstatus	= EXIT_SUCCESS;	
				exit(tmp_exitstatus);
			}
			else{ // if the argument was incorrect
				printf("The argument is incorrect.\n");
				exitstatus	= EXIT_FAILURE;	
			}
		}
	}
	else if(strcmp(t->argv[0], "cd")==0){
		if(t->argv[1] == NULL){
			chdir(HOME);	
		}
		else{
			int cmd_max_len = strlen(CDPATH) + strlen(t->argv[0]);
			char cmd[cmd_max_len];
			bool hasslash = containsslash(t->argv[1]);
			if(!hasslash){
				const char s[2] = ":";
				char *token;
			
				token = strtok(CDPATH,s);
				while(token != NULL){
					memset(cmd, '\0', cmd_max_len);
					strcat(cmd, token);
					strcat(cmd, t->argv[0]);
//					printf("%s\n",cmd);
					chdir(cmd);
					token = strtok(NULL, s);
				}	
			}
		}
		exitstatus	= EXIT_SUCCESS;	
	}
	else {				// normal, exit commands
		printf("hello");
		each_cmd(t);
		exitstatus	= EXIT_SUCCESS;
	}

//	prev_exitstatus = exitstatus;
	return exitstatus;
}
