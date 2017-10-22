#include "myshell.h"

/*
   CITS2002 Project 2 2017
   Name(s):		student-name1 (, student-name2)
   Student number(s):	student-number-1 (, student-number-2)
   Date:		date-of-submission
 */

// -------------------------------------------------------------------

//This function create child process.
void each_cmd(SHELLCMD *t){
	int pid = fork();

	// ensure that a new process was created
	if(pid == -1) {                             // process creation failed
		printf("pid == -1 -> fork() failed\n" );
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
	
		if(t->argv[0][0] == '/'){
			execv(t->argv[0],t->argv);
			printf("pid == 0 -> execv() failed\n");
	        	exit(EXIT_FAILURE);	
		}
		else{
			int cmd_max_len = strlen(PATH) + strlen(t->argv[0]) + strlen("/");
			char cmd[cmd_max_len];
			
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
			printf("pid == 0 -> execv() failed\n");
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
//	else if(strcmp(t->argv[0], "exit")){
//		int tmp_exitstatus = 0;//prev_exitstatus;
//		printf("\'exit\' is executed.");
//		if(strcmp(t->argv[1],"EXIT_SUCCESS") == 0) strcpy(t->argv[1],"0\0");
//		if(strcmp(t->argv[1],"EXIT_FAILURE") == 0) strcpy(t->argv[1],"1\0");
//		if(t->argv[1][0] == '0' || t->argv[1][0] == '1'){ 
//		// if an appropriate argument was provided
//			int argv1 = atoi(t->argv[1]);
//			printf("%d\n",argv1);
//			tmp_exitstatus = argv1;
//			exitstatus	= EXIT_SUCCESS;	
//			exit(tmp_exitstatus);
//		}
//		else if(strcmp(t->argv[1],"\0") == 0){ // if there was no argument
//			exitstatus	= EXIT_SUCCESS;	
//			exit(tmp_exitstatus); //exit-status of the recently executed command is used
//		}
//		else{ // if the argument was wrong
//			exitstatus	= EXIT_FAILURE;	
//			exit(tmp_exitstatus); //exit-status of the recently executed command is used
//		}
//	}
//	else if(strcmp(t->argv[0], "cd")){
//		printf("\'cd\' is executed.");
//		if(t->argv[1] > 0){
//			
//		}
//		exitstatus	= EXIT_SUCCESS;	
//	}
	else {				// normal, exit commands
		each_cmd(t);
//		printf("%d\n",EXIT_FAILURE);
		exitstatus	= EXIT_SUCCESS;
	}

//	prev_exitstatus = exitstatus;
	return exitstatus;
}
