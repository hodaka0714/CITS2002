#include "myshell.h"
#include <sys/time.h>
#include <fcntl.h>
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

void step1_1(){
    char *step1_arguments[20];
    int a = 0;
    step1_arguments[a++] = "/bin/ls";
    step1_arguments[a++] = NULL;
    execv(step1_arguments[0],step1_arguments);
}
void step1_2(){
    char *step2_arguments[20];
    int b = 0;
    step2_arguments[b++] = "/usr/bin/cal";
    step2_arguments[b++] = NULL;
    step2_arguments[b++] = "-y";
    execv(step2_arguments[0],step2_arguments);
}


//void step6_1(SHELLCMD *t){//read filecommand, return a string
//
//
//}


//This function create child process.
int cmd_command(SHELLCMD *t){
    int exitstatus;
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
            if(isnumber(t->argv[1])){ // if the argument is a number
                tmp_exitstatus = atoi(t->argv[1]); // convert from string to int
                exitstatus	= EXIT_SUCCESS;
                exit(tmp_exitstatus);
            }
            else{ // if the argument was incorrect
                printf("%s: exit: %s numeric argument required.\n",argv0,t->argv[1]);
                exitstatus	= EXIT_FAILURE;
            }
        }
    }
    else if(strcmp(t->argv[0], "cd")==0){
        int check_change_dir;
        exitstatus = EXIT_SUCCESS;
        if(t->argv[1] == NULL){
            chdir(HOME);
        }
        else{
            int cd_cmd_max_len = strlen(CDPATH) + strlen(t->argv[0]);
            char cd_cmd[cd_cmd_max_len];
            if(t->argv[1][0] != '/'){               // if the command doesn't commence with "/" (i.g. cd Desktop)
                char CDPATH_array[strlen(CDPATH)];  // convert from *CDPATH to CDPATH_array[]
                strcpy(CDPATH_array,CDPATH);
                memset(cd_cmd, '\0', cd_cmd_max_len);
                strcat(cd_cmd, HOME);
                strcat(cd_cmd, "/");
                strcat(cd_cmd, t->argv[1]);
                check_change_dir = chdir(cd_cmd);
                if(check_change_dir == -1){
                    printf("%s: No such file or directory.\n",t->argv[1]);
                    exitstatus = EXIT_FAILURE;
                }
            }
            else{	// if the command commence with "/" (i.g. "cd /desktop")
                memset(cd_cmd, '\0', cd_cmd_max_len);
                strcat(cd_cmd, HOME);
                strcat(cd_cmd, t->argv[1]);
                check_change_dir = chdir(cd_cmd);
                if(check_change_dir == -1){
                    printf("%s: No such file or directory.\n",t->argv[1]);
                    exitstatus = EXIT_FAILURE;
                }
            }
        }
    }
    else if(strcmp(t->argv[0], "time") == 0){
        struct	timeval	start;
        struct	timeval	end;
        
        //for time difference
        unsigned long diff;
        
        (t->argc)--;
        (t->argv)++;
        
        if(t->argc == 0){
            exitstatus = EXIT_FAILURE;
        }else{
            gettimeofday(&start, NULL);
            exitstatus = execute_shellcmd(t);
            gettimeofday(&end, NULL);
            
            diff = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
            fprintf(stderr,"%ldmsec\n", diff); //one last step to report to stderr stream
            
            exitstatus    = EXIT_SUCCESS;
        }
        (t->argv)--;
    }
    
    
    
    
    else {			// command not including "exit" "cd" "time"
        int pid = fork();
        
        // ensure that a new process was created
        if(pid == -1) {                             // process creation failed
            printf("fork() failed. (pid == -1)\n" );
            exit(EXIT_FAILURE);
        }
        else if(pid == 0){
            
            printf("!!!%s!!!\n",t->outfile);
            
            bool hasslash = containsslash(t->argv[0]);
            int cmd_max_len = strlen(PATH) + strlen(t->argv[0]) + strlen("/");
            char cmd[cmd_max_len];
            int fd, backup;
            backup = dup(1);
            
            if(hasslash){
                if(t->argv[0][0] == '/'){
                    if(t->infile != NULL){
                        char *new_argv[t->argc + 1];
                        
                        for(int i = 0; i < t->argc; i++){
                            new_argv[i] = t->argv[i];
                        }
                        new_argv[t->argc] = t->infile;
                        new_argv[t->argc + 1] = NULL;
                        execv(t->argv[0], new_argv);
                    }
                    
                    if(t->outfile != NULL){
                        if(t->append == false){
                            fd = open(t->outfile, O_CREAT | O_WRONLY | O_TRUNC);
                        }else{
                            fd = open(t->outfile, O_CREAT | O_WRONLY | O_TRUNC | O_APPEND);
                        }
                        dup2(fd, 1);
                        execv(t->argv[0],t->argv);
                    }else{
                        execv(t->argv[0],t->argv);
                    }
                    
                }else{
                    strcat(cmd,"/");
                    strcat(cmd,t->argv[0]);
                    strcpy(t->argv[0],cmd);
                    if(t->outfile != NULL){
                        if(t->append == false){
                            fd = open(t->outfile, O_CREAT | O_WRONLY | O_TRUNC);
                        }else{
                            fd = open(t->outfile, O_CREAT | O_WRONLY | O_TRUNC | O_APPEND);
                        }
                        dup2(fd, 1);
                        execv(t->argv[0],t->argv);
                    }else{
                        execv(t->argv[0],t->argv);
                    }
                }
                dup2(backup,1);
                close(backup);
                
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
                    if(t->infile != NULL){          // (i.g. sort -k 2 < z.txt)
                        char *new_argv[t->argc + 2];

                        for(int i = 0; i < t->argc; i++){
                            if(i == 0){new_argv[0] = cmd;}
                            else{new_argv[i] = t->argv[i];}
                        }

                        new_argv[t->argc] = t->infile;
                        new_argv[t->argc + 1] = NULL;

                        execv(new_argv[0],new_argv);
                    }
                    
                    if(t->outfile != NULL){
                        if(t->append == false){     // (i.g. cal -y > a.txt)
                            fd = open(t->outfile, O_CREAT | O_WRONLY | O_TRUNC);
                        }else{                      // (i.g. cal -y >> a.txt)
                            fd = open(t->outfile, O_CREAT | O_WRONLY | O_TRUNC | O_APPEND);
                        }
                        dup2(fd, 1);
                        execv(cmd,t->argv);
                    }else{                          // (i.g. cal -y)
                        execv(cmd,t->argv);
                    }
                    
                    token = strtok(NULL, s);
                }
                dup2(backup,1);
                close(backup);
                
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
        exitstatus	= EXIT_SUCCESS;
    }
    return exitstatus;
}

//  THIS FUNCTION SHOULD TRAVERSE THE COMMAND-TREE and EXECUTE THE COMMANDS
//  THAT IT HOLDS, RETURNING THE APPROPRIATE EXIT-STATUS.
//  READ print_shellcmd0() IN globals.c TO SEE HOW TO TRAVERSE THE COMMAND-TREE

int execute_shellcmd(SHELLCMD *t)
{
    int exitstatus = 0;
    int exitstatus1, exitstatus2;
    if(t->type == CMD_COMMAND){
        exitstatus = cmd_command(t);
    }else if(t->type == CMD_SEMICOLON){
        exitstatus = execute_shellcmd(t->left);
        exitstatus = execute_shellcmd(t->right);
    }else if(t->type == CMD_AND){
        exitstatus1 = execute_shellcmd(t->left);
        exitstatus2 = execute_shellcmd(t->right);
        exitstatus = exitstatus1 * exitstatus2;
    }else if(t->type == CMD_OR){
        exitstatus1 = execute_shellcmd(t->left);
        exitstatus2 = execute_shellcmd(t->right);
        if(exitstatus1 == 1 || exitstatus2 == 1){
            exitstatus = 1;
        }else{
            exitstatus = 0;
        }
    }else if(t->type == CMD_SUBSHELL){
        exitstatus = execute_shellcmd(t->left);
    }
    
    //	prev_exitstatus = exitstatus;
    return exitstatus;
}
