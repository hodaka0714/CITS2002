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
	        printf("pid == 0 -> execv() failed\n");
        	exit(EXIT_FAILURE);	
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
    else {				// normal, exit commands
	each_cmd(t);
	exitstatus	= EXIT_SUCCESS;
    }

    return exitstatus;
}
