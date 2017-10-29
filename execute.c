#include "myshell.h"
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
/*
 CITS2002 Project 2 2017
 Name(s):		student-name1 (, student-name2)
 Student number(s):	student-number-1 (, student-number-2)
 Date:		date-of-submission
 */


// -------------------------------------------------------------------

#define MODE_CREATE (O_CREAT | O_WRONLY | O_TRUNC)
#define MODE_APPEND (O_CREAT | O_WRONLY | O_APPEND)

char *subshell_infile, *subshell_outfile;
bool subshell_append;

bool isnumber(char *str){
    bool judge = true;
    for(int i = 0; i < strlen(str); i++){
        if(str[i] < 48 || str[i] > 57){
            judge = false;
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

int step3_cd(SHELLCMD *t, int exitstatus){
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
                printf("%s: No such file or directory.\n", t->argv[1]);
                exitstatus = EXIT_FAILURE;
            }
        }
        else{	// if the command commence with "/" (i.g. "cd /desktop")
            memset(cd_cmd, '\0', cd_cmd_max_len);
            strcat(cd_cmd, HOME);
            strcat(cd_cmd, t->argv[1]);
            check_change_dir = chdir(cd_cmd);
            if(check_change_dir == -1){
                printf("%s: No such file or directory.\n", t->argv[1]);
                exitstatus = EXIT_FAILURE;
            }
        }
    }
    return exitstatus;
}

int step3_time(SHELLCMD *t, int exitstatus){
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
        
        diff = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec; // to check later
        fprintf(stderr,"%ldmsec\n", diff); //one last step to report to stderr stream
        
        exitstatus    = EXIT_SUCCESS;
    }
    (t->argv)--;
    return exitstatus;
}

//check if ask for append and call open()
int check_append(char *ch, SHELLCMD *t){
    int fd;
    if(t->append == false){     // i.e.(/bin/ls > e.txt) good!
        fd = open(ch, MODE_CREATE);
    }else{                      // i.e.(/bin/ls >> e.txt) good!
        fd = open(ch, MODE_APPEND);
    }
    return fd;
}

void step6_newarg(SHELLCMD *t,int num,int fd) {//not very good
    char *new_argv[num];
    for(int i = 0; i < t->argc; i++){
        new_argv[i] = t->argv[i];
    }
    if(subshell_infile != NULL){
        new_argv[t->argc] = subshell_infile;
    }else{
        new_argv[t->argc] = t->infile;
    }
    new_argv[t->argc + 1] = NULL;
    dup2(fd, 1);
    execv(t->argv[0], new_argv);
}

void step6_newargsub(SHELLCMD *t,int fd,char *cmd){
    char *new_argv[t->argc + 2];
    for(int i = 0; i < t->argc; i++){
        //new_argv[i] = t->argv[i];
        if(i == 0){new_argv[0] = cmd;}
        else{new_argv[i] = t->argv[i];}
    }
    if(subshell_infile != NULL){
        new_argv[t->argc] = subshell_infile;
    }else{
        new_argv[t->argc] = t->infile;
    }
    new_argv[t->argc + 1] = NULL;
    dup2(fd, 1);
    execv(new_argv[0], new_argv);
}



int pipeline(SHELLCMD *t){
    printf("gello1\n");
    int exitstatus = 0;
    
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe");
        exit(-1);
    }
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(-1);
    } else if (pid == 0) {
        // 子プロセス
        close(pipefd[1]); //書き込みをクローズ
        
        
        dup2(pipefd[0], STDIN_FILENO); //パイプの読み込みを標準入力につなぐ
        close(pipefd[0]);              //つないだらパイプはクローズする
        
//                char *hello[5] = {"sort","-k","+4", NULL};
//                execv("/usr/bin/sort",hello);
        
//        execl("/usr/bin/sort", "sort","-k","+4", NULL);
//        execl("/bin/cat", "/bin/cat", NULL); //catは標準入力のデータをそのまま出力するコマンド
    
//       printf("execute_shellcmd(t->right)\n");
        exitstatus = execute_shellcmd(t->right);
        
        perror("/bin/cat");
    } else {
        // 親プロセス
        close(pipefd[0]); //読み込みをクローズ
        dup2(pipefd[1], STDOUT_FILENO); //パイプのshutsuryokuを標準shutsuryokuにつなぐ
        
//        char *s = "send from parent";
//        write(pipefd[1], s, strlen(s));
//        execl("/bin/ls", "ls","-l", NULL);
        
//        char *hello2[4] = {"ls","-l", NULL};
//        execv("/bin/ls",hello2);
        
//        printf("gello2\n");
        exitstatus = execute_shellcmd(t->left);
        
        close(pipefd[1]);
    }
//    
//    int     fd[2];
//    pid_t   childpid;
//    
//    pipe(fd);
//    
//    if((childpid = fork()) == -1)
//    {
//        printf("gello1.1\n");
//        perror("fork");
//        exit(1);
//    }
//    
//    if(childpid == 0)
//    {
//        printf("gello1.2\n");
//        /* Child process closes up input side of pipe */
//        printf("gello1.5\n");
//        dup2(fd[0], 0);
//        printf("gello2\n");
//        close(fd[0]);
//        char *hello2[4] = {"ls","-l", NULL};
//        execv("/bin/ls",hello2);
//        exit(0);
//    }
//    else
//    {
//        /* Parent process closes up output side of pipe */
//        printf("gello3\n");
//        dup2(fd[1], 1);
//        printf("gello4\n");
//        close(fd[0]);
//        
//        /* Read in a string from the pipe */
//        
//        char *hello[5] = {"sort","-k","4", NULL};
//        execv("/usr/bin/sort",hello);
//    }
    
    
    
    
    
//    // Set up for fork
//    pid_t pid;
//
//    // Set up pipe
//    int thePipe[2];
//
//    if(pipe(thePipe) == -1)
//    {
//        printf("Pipe Failed\n");
//        exit(1);
//    }
//    
//    // Do Fork
//    if ((pid = fork()) > 0) // Parent
//    {
//        // Close pipe ends pointing stdout to the pipe input
//        close(thePipe[0]);
//        char *hello[5] = {"sort","-k","4", NULL};
//        execv("/usr/bin/sort",hello);
////        exitstatus = execute_shellcmd(t->left);
//        // Code that writes to stdout
//        
//    }
//    else if (pid == 0) // Child
//    {
//        // Close pipe ends pointing output of the pipe to stdin
//        dup2(thePipe[0], 0);
//        close(thePipe[0]);
//        close(thePipe[1]);
//        char *hello2[4] = {"ls","-l", NULL};
//        execv("/bin/ls",hello2);
////        exitstatus = execute_shellcmd(t->right);
//        // Code including system calls that read from stdin
//    }
    
//    printf("hello1\n");
//    printf("hello222\n");
//    printf("hello223\n");
//    int exitstatus = 0;
//    printf("hello224\n");
//    //int waitstatus= -10;
//    int	writepipe[2] = {-1,-1},	readpipe[2] = {-1,-1};	/* parent->child child -> parent */
//    pid_t childpid;
//    printf("hello225\n");
//    /*------------------------------------------------------------------------
//     * CREATE THE PAIR OF PIPES
//     *
//     * Pipes have two ends but just one direction: to get a two-way
//     * conversation you need two pipes. It's an error if we cannot make
//     * them both, and we define these macros for easy reference.
//     */
//    writepipe[0] = -1;
//    printf("hello226\n");
//    if (pipe(readpipe) < 0  ||  pipe(writepipe) < 0)
//    {
//        perror("Failed ro reset pipeline");
//        /* close readpipe[0] & [1] if necessary */
//    }
//    printf("hello227\n");
//#define	PARENT_READ	readpipe[0]
//#define	CHILD_WRITE	readpipe[1]
//#define CHILD_READ	writepipe[0]
//#define PARENT_WRITE	writepipe[1]
//    printf("hello228\n");
//    if((childpid = fork()) < 0)
//    {
//        perror("Failed to redirect stdout of t->left\n");
//    }
//     if(childpid == 0 )	/* in the child */
//    {
//        printf("hello229\n");
//        close(PARENT_WRITE);
//        printf("hello230\n");
//        close(PARENT_READ);
//        printf("hello231\n");
//        dup2(CHILD_READ,  0);
//        close(CHILD_READ);
//        printf("hello232\n");
//        dup2(CHILD_WRITE, 1);
//        close(CHILD_WRITE);
//        exitstatus = execute_shellcmd(t->left);
//
//        
//    }
//    else //if ( childpid != 0 )				/* in the parent */
//    {
//        //wait(&waitstatus);
//        printf("hello233\n");
//        close(CHILD_READ);
//        printf("hello234\n");
//        close(CHILD_WRITE);
//        printf("hello235\n");
//        exitstatus = execute_shellcmd(t->right);
//
//    }
    
    return exitstatus;
}

//This function create child process.
int cmd_command(SHELLCMD *t){
    int exitstatus = 0;
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
    // step 3 . cd
    else if(strcmp(t->argv[0], "cd")==0){
       exitstatus= step3_cd(t,exitstatus);
    }
    //step 3 time
    else if(strcmp(t->argv[0], "time") == 0){
        exitstatus= step3_time(t,exitstatus);
    }
    
    //step 6.
    else {			// command not including "exit" "cd" "time"
        int pid = fork();
        // ensure that a new process was created
        if(pid == -1) {                             // process creation failed
            printf("fork() failed. (pid == -1)\n" );
            exit(EXIT_FAILURE);
        }
        else if(pid == 0){
            int cmd_max_len = strlen(PATH) + strlen(t->argv[0]) + strlen("/");
            char cmd[cmd_max_len];
            int fd, backup;
            backup = dup(1);
            //start with '/'
            if(t->argv[0][0] == '/'){
                // i.e.(/usr/bin/sort -k 2),(/usr/bin/sort -k 2 < a.txt),(/sort -k 2 < a.txt)
                if(t->outfile != NULL || subshell_outfile != NULL){ //i.e.(/bin/ls > e.txt),(/bin/ls >> e.txt)
                    fd = check_append(t->outfile,t);
//                    //if(t->append == false){     // i.e.(/bin/ls > e.txt) good!
                    if(t->infile != NULL || subshell_infile != NULL){           // i.e.(/usr/bin/sort -k 2 < z.txt > e.txt) good!
                        step6_newarg(t,t->argc + 1,fd);
                    }else{
                        printf("hello2\n");
                        dup2(fd, 1);
                        execv(t->argv[0],t->argv);
                        printf("%s: comment not found: %s .\n",argv0,t->argv[0]);
                    }
                }else if(t->outfile == NULL && subshell_outfile == NULL){
                    // subshell_outfile == t->outfile == NULL (i.e. (/bin/ls),(/usr/bin/sort < z.txt))
                    if(t->infile != NULL || subshell_infile != NULL){           // i.e.(/usr/bin/sort -k 2 < z.txt)
                        char *new_argv[t->argc + 1];
                        
                        for(int i = 0; i < t->argc; i++){
                            new_argv[i] = t->argv[i];
                        }
                        
                        if(subshell_infile != NULL){
                            new_argv[t->argc] = subshell_infile;
                        }else{
                            new_argv[t->argc] = t->infile;
                        }
                        
                        new_argv[t->argc + 1] = NULL;
                        execv(t->argv[0], new_argv);
                        printf("%s: comment not found: %s .\n",argv0,t->argv[0]);
                    }else{          // i.e. (/bin/ls),(/usr/bin/cal -y)
                        printf("hello3\n");
                        execv(t->argv[0],t->argv);
                        printf("%s: comment not found: %s .\n",argv0,t->argv[0]);
                    }
                }
                dup2(backup,1);
                close(backup);
                printf("execv() failed. (pid == 0)\n");
                exit(EXIT_FAILURE);
            }else{
                // Not start with '/' i.e.( usr/bin/sort -k 2),(sort -k 2 < z.txt),(sort < z.txt)
                const char s[2] = ":";
                char *token;
                token = strtok(PATH,s);
                while(token != NULL){
                    memset(cmd, '\0', cmd_max_len);
                    strcat(cmd, token);
                    strcat(cmd, "/");
                    strcat(cmd, t->argv[0]);
                    printf("%s",subshell_infile);
                    if(subshell_outfile != NULL){
                        fd = check_append(subshell_outfile,t);//check append
                        if(t->infile != NULL || subshell_infile != NULL){
                            // i.g. (sort -k 2 < z.txt) good!
                            step6_newargsub(t, fd,cmd);
                        }else{  // i.e. ((ls) > c.txt)good!
                            dup2(fd, 1);
                            execv(cmd,t->argv);
                        }
                    }else if(t->outfile != NULL){
                        fd = check_append(t->outfile,t);                    // (i.g. cal -y >> a.txt) good!
                        if(t->infile != NULL || subshell_infile != NULL){
                            // (i.g. sort -k 2 < z.txt > dd2.txt) good!
                            step6_newargsub(t, fd,cmd);
                        }else if(t->infile == NULL && subshell_infile == NULL){      // (i.e. ls > c.txt) good!
                            dup2(fd, 1);
                            execv(cmd,t->argv);
                        }
                    }
                    else{                   // subshell_outfile == t->outfile == NULL (i.g. (ls),(sort -k 2 < z.txt),(cal -y))
                        if(t->infile != NULL || subshell_infile != NULL){
                            // (i.g. sort -k 2 < z.txt)
                            char *new_argv[t->argc + 2];
                            for(int i = 0; i < t->argc; i++){
                                if(i == 0){new_argv[0] = cmd;}
                                else{new_argv[i] = t->argv[i];}
                            }
                            if(subshell_infile != NULL){
                                new_argv[t->argc] = subshell_infile;
                            }else{
                                new_argv[t->argc] = t->infile;
                            }
                            new_argv[t->argc + 1] = NULL;
                            execv(new_argv[0], new_argv);
                        }else{          // (i.e. (ls),(cal -y))
//                            char *hello[2] = {"ls","-l",NULL};
//                            execv("/bin/ls",hello);
                            execv(cmd, t->argv);
                        }
                    }
                    token = strtok(NULL, s);
                }
                printf("%s: comment not found: %s .\n", argv0, t->argv[0]);
                dup2(backup,1);
                close(backup);
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
        subshell_infile = t->infile;
        subshell_outfile = t->outfile;
        subshell_append = t->append;
        exitstatus = execute_shellcmd(t->left);
    }else if(t->type == CMD_PIPE){
        printf("hello %d\n",t->right->argc);
        exitstatus = pipeline(t);
    }
    
    //	prev_exitstatus = exitstatus;
    return exitstatus;
}








