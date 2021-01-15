#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#define CMDLINE_MAX 512
            

void exec1(int pipe1[2], char *const cmd[], int out) 
{
    if(out == 1) 
    {
        dup2(pipe1[1], 1);
    }

    close(pipe1[0]);
    close(pipe1[1]);

    execvp(cmd[0], cmd);

    perror("bad exec cmd1");
    _exit(1);
}

void exec2(int pipe1[2], int pipe2[2], char *const cmd[], int out) 
{
    dup2(pipe1[0], 0);
    if(out == 1)
    {
        dup2(pipe2[1], 1);
    }

    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    execvp(cmd[0], cmd);

    perror("bad exec cmd2");
    _exit(1);
}

void exec3(int pipe1[2], int pipe2[2], int pipe3[2], 
           char *const cmd[], int out) 
{
    dup2(pipe2[0], 0);
    if(out == 1)
    {
        dup2(pipe3[1], 1);
    }

    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    close(pipe3[0]);
    close(pipe3[1]);

    execvp(cmd[0], cmd);

    perror("bad exec cmd3");
    _exit(1);
}

void exec4(int pipe1[2], int pipe2[2], 
           int pipe3[2], char *const cmd[]) 
{
    dup2(pipe3[0], 0);

    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    close(pipe3[0]);
    close(pipe3[1]);
    
    execvp(cmd[0], cmd);

    perror("bad exec cmd4");
    _exit(1);
}

int exec_line(char *const cmd1[], char *const cmd2[], 
               char *const cmd3[], char *const cmd4[], int n) 
{
    int pid;
    int pipe1[2];
    int pipe2[2];
    int pipe3[2];
    int out = 1;

    if (pipe(pipe1) == -1) 
    {
        perror("bad pipe1");
        exit(1);
    }

    if ((pid = fork()) == -1) 
    {
        perror("bad fork1");
        exit(1);
    } 
    else if (pid == 0) 
    {
        exec1(pipe1, cmd1, out);
    }

    if (n > 1)
    {
        if (n == 2) 
        {
            out = 0;
        }

        if (pipe(pipe2) == -1) 
        {
            perror("bad pipe2");
            exit(1);
        }

        if ((pid = fork()) == -1) 
        {
            perror("bad fork2");
            exit(1);
        } 

        else if (pid == 0) 
        {
            exec2(pipe1, pipe2, cmd2, out);
        }
        
        close(pipe1[0]);
        close(pipe1[1]);
    }
  
    if (n > 2)
    {
        if (n == 3) 
        {
            out = 0;
        }

        if (pipe(pipe3) == -1) 
        {
            perror("bad pipe3");
            exit(1);
        }

        if ((pid = fork()) == -1) 
        {
            perror("bad fork3");
            exit(1);
        } 
        else if (pid == 0) 
        {
            exec3(pipe1, pipe2, pipe3, cmd3, out);
        }
        
        close(pipe2[0]);
        close(pipe2[1]);
    }
  
    if (n > 3)
    {
        out = 0;

        if ((pid = fork()) == -1) 
        {
            perror("bad fork4");
            exit(1);
        } 
        else if (pid == 0) 
        {
            exec4(pipe1, pipe2, pipe3, cmd4);
        }

        close(pipe3[0]);
        close(pipe3[1]);
    }

    
    int status;
    int returnCodes[10];
    for (int i = 0; i < n; i++)
    {
        wait(&status);
        returnCodes[i] = WEXITSTATUS(status);
    }

    char str0[2]; 
    char str1[2]; 
    char str2[2];   
    char str3[2];
    char str4[2]; 
    for (int i = 0; i < n; i++) {
        if (n == 2) { 
            sprintf(str0, "%d", 1);
            sprintf(str1, "%d", returnCodes[0]);     
            sprintf(str2, "%d", returnCodes[1]);

            strcat(str0, str1);
            strcat(str0, str2);
        }
        else if (n == 3) { 
            sprintf(str0, "%d", 1);
            sprintf(str1, "%d", returnCodes[0]);     
            sprintf(str2, "%d", returnCodes[1]);
            sprintf(str3, "%d", returnCodes[2]);

            strcat(str0, str1);
            strcat(str0, str2);
            strcat(str0, str3);
        }
        else if (n == 4) { 
            sprintf(str0, "%d", 1);
            sprintf(str1, "%d", returnCodes[0]);     
            sprintf(str2, "%d", returnCodes[1]);
            sprintf(str3, "%d", returnCodes[2]);
            sprintf(str4, "%d", returnCodes[3]);

            strcat(str0, str1);
            strcat(str0, str2);
            strcat(str0, str3);
            strcat(str0, str4);
        }
    }
    int formattedReturnCode = atoi(str0);

    return formattedReturnCode;
}

int pipelineLoop(const char **process1, const char **process2, 
                 const char **process3, const char **process4, 
                 int numProcesses)
{
    char * const*commandLines[4];
    int allReturnCodes;

    for (int i = 0; i < 4; i++)
    {       
        commandLines[i] = NULL;
    }

    if (numProcesses == 2) 
    {
        commandLines[0] = (char * const*)process1;
        commandLines[1] = (char * const*)process2;
    }
    else if (numProcesses == 3) 
    {
        commandLines[0] = (char * const*)process1;
        commandLines[1] = (char * const*)process2;
        commandLines[2] = (char * const*)process3;
    }
    else if (numProcesses == 4) 
    {
        commandLines[0] = (char * const*)process1;
        commandLines[1] = (char * const*)process2;
        commandLines[2] = (char * const*)process3;
        commandLines[3] = (char * const*)process4;
    }
    
    allReturnCodes = exec_line(commandLines[0], commandLines[1], 
              commandLines[2], commandLines[3], numProcesses);

    return allReturnCodes;
}

int launcherOutputRedirection(char **args, char *fileName, int flag_redirect_StdErr)
{
    pid_t pid;
    int status;
    int exitStatus;

    pid = fork();

    if (pid == 0)
    {
        int fd = open(fileName, O_CREAT | O_WRONLY | O_TRUNC, 0777);

        if (flag_redirect_StdErr == 0) {
            close(STDOUT_FILENO);
        } else {
            close(STDERR_FILENO);
        }
        
        dup(fd);
        close(fd);
        execvp(args[0], args);

        perror("execvp output redir");
        exit(1);
    }
    else if (pid > 0)
    {
        waitpid(-1, &status, 0);
        exitStatus = WEXITSTATUS(status);
    }
    else
    {
        perror("forking error");
        //exit(1);
        exitStatus = 1;
    }

    return exitStatus;
}

int launcher(char **args)
{
    pid_t pid;
    int status;
    int exitStatus;

    pid = fork();

    if (pid == 0)
    {
        if (!strcmp(args[0], "pwd"))
        {
            char buffer[512];

            if (getcwd(buffer, sizeof(buffer)) != NULL)
            {
                printf("%s\n", buffer);
                exit(0);
            }
            else
            {
                perror("getcwd");
                exit(1);
            }
        }
        else if (!strcmp(args[0], "cd"))
        {
            exit(0);
        }
        else
        {
            execvp(args[0], args);

            perror("execvp");
            exit(1);
        }
    }
    else if (pid > 0)
    {
        waitpid(-1, &status, 0);
        exitStatus = WEXITSTATUS(status);

        if (!strcmp(args[0], "cd"))
        {
            if (chdir(args[1]) != 0)
            {
                perror("chdir");
                //exit(1);
                exitStatus = 1;
            }
            else
            {
                exitStatus = 0;
            }
        }
    }
    else
    {
        perror("forking error");
        exit(1);
    }

    return exitStatus;
}

int commandLineParser(void)
{
    char cmd[CMDLINE_MAX];
    char *nl;
    int processCompletionStatus;
    char *commandline[CMDLINE_MAX];
    char *commandLineOutputRedNoPipe[CMDLINE_MAX];
    char fileName[512] = "";
    char fileName2[512] = "";
    char commandWithArgs[512] = "";
    char commandOnly[512] = "";
    char argumentsOnly[512] = "";
    int shellExitStatus = 1;
    const char *commandline_1[CMDLINE_MAX]; 
    const char *commandline_2[CMDLINE_MAX];
    const char *commandline_3[CMDLINE_MAX];
    const char *commandline_4[CMDLINE_MAX];
    int numCommandLinesPipe = 0;
    int pipeReturnStatus = 0;

    while (shellExitStatus)
    {
        // print prompt
        printf("sshell$ ");
        fflush(stdout);

        // get input command line
        fgets(cmd, CMDLINE_MAX, stdin);

        // print command line if stdin is not provided by terminal
        if (!isatty(STDIN_FILENO))
        {
            printf("%s", cmd);
            fflush(stdout);
        }
        
        // if empty line
        if(cmd[0] == '\n')
        {
            continue;
        }

        // Remove trailing newline \n from command line
        nl = strchr(cmd, '\n');
        if (nl)
        {
            *nl = '\0';
        }

        if (!strcmp(cmd, "exit"))
        {
            //fprintf(stderr, "Bye...\n");
            shellExitStatus = 0;
            break;
        }

        char *duplicateCMD;
        duplicateCMD = strdup(cmd);
        int strtok_Index1 = 0;
        int launcherFlag = 0;
        int outputRedir_stdErr_flag = 0;
        char *metachar_outputRed = strchr(cmd, '>');
        char *metachar_pipe = strchr(cmd, '|');
        char *metachar_and = strchr(cmd, '&');
        int commandlineLength = (int)strlen(cmd);
        char cmd2[100] = "";
        char cmd3[100] = "";
        char cmd4[100] = "";
        char *line1 = (char *)malloc(100);
        char *line2 = (char *)malloc(100);
        char *line3 = (char *)malloc(100);
        char *line4 = (char *)malloc(100);
        char *metacharsInOrderOfOccurence[100];
        int metachar_index = 0;
        int savedIndex = 0;

        memset(commandline, 0, sizeof commandline);
        memset(commandLineOutputRedNoPipe, 0, sizeof commandLineOutputRedNoPipe);
        memset(fileName, 0, sizeof fileName);
        memset(fileName2, 0, sizeof fileName2);
        memset(commandWithArgs, 0, sizeof commandWithArgs);
        memset(commandOnly, 0, sizeof commandOnly);
        memset(argumentsOnly, 0, sizeof argumentsOnly);

        if ((cmd[0] == 'c') && (cmd[1] == 'd') && cmd[2] == ' ')
        {
            launcherFlag = 3;

            int indexToDelete = 0;
            memmove(&cmd[indexToDelete], &cmd[indexToDelete + 1], strlen(cmd) - indexToDelete);
            memmove(&cmd[indexToDelete], &cmd[indexToDelete + 1], strlen(cmd) - indexToDelete);
            memmove(&cmd[indexToDelete], &cmd[indexToDelete + 1], strlen(cmd) - indexToDelete);

            char argStrWithoutBackslash[CMDLINE_MAX] = "";
            char *ptr = strtok(cmd, "\\");
            while (ptr != NULL)
            {
                strcat(argStrWithoutBackslash, ptr);
                strtok_Index1++;
                ptr = strtok(NULL, "\\");
            }

            commandline[0] = "cd";
            commandline[1] = argStrWithoutBackslash;
        }
        else if ((metachar_outputRed != NULL) && (metachar_pipe == NULL))
        {
            launcherFlag = 1;

            if (metachar_and != NULL)
            {   
                outputRedir_stdErr_flag = 1;
            }

            if (metachar_and == NULL)
            {
                char *ptr = strtok(cmd, ">");
                while (ptr != NULL)
                {
                    if (strtok_Index1 == 0) 
                    {
                        strcat(commandWithArgs, ptr);
                        strtok_Index1++;
                        ptr = strtok(NULL, " ");
                    }
                    else 
                    {
                        strcat(fileName, ptr);
                        strcat(fileName, " ");
                        strtok_Index1++;
                        ptr = strtok(NULL, " ");
                    }
                }
            }
            else if (metachar_and != NULL)
            {
                char *ptr = strtok(cmd, ">&");
                while (ptr != NULL)
                {
                    if (strtok_Index1 == 0)
                    {
                        strcat(commandWithArgs, ptr);
                        strtok_Index1++;
                        ptr = strtok(NULL, ">&");
                    }
                    else
                    {
                        strcat(fileName, ptr);
                        strcat(fileName, " ");
                        strtok_Index1++;
                        ptr = strtok(NULL, " ");
                    }
                }
            }

            strtok_Index1 = 0;
            char *ptr1 = strtok(fileName, " ");
            while (ptr1 != NULL)
            {
                strcat(fileName2, ptr1);
                strtok_Index1++;
                ptr1 = strtok(NULL, " ");
            }

            strtok_Index1 = 0; 
            char *ptr2 = strtok(commandWithArgs, " ");
            while (ptr2 != NULL)
            {
                if (strtok_Index1 == 0)
                { 
                    strcat(commandOnly, ptr2);
                    strtok_Index1++;
                    ptr2 = strtok(NULL, " ");
                }
                else    
                {
                    strcat(argumentsOnly, ptr2);
                    strcat(argumentsOnly, " ");
                    strtok_Index1++;
                    ptr2 = strtok(NULL, " ");
                }
            }

            argumentsOnly[strlen(argumentsOnly) - 1] = '\0';

            commandLineOutputRedNoPipe[0] = commandOnly;
            commandLineOutputRedNoPipe[1] = argumentsOnly;
        }
        else if (metachar_pipe != NULL)
        {
            launcherFlag = 2;
            
            for (int i = 0; i < CMDLINE_MAX; i ++)
            {
                commandline_1[i] = NULL;
                commandline_2[i] = NULL;
                commandline_3[i] = NULL;
                commandline_4[i] = NULL;
            }

            for (int i = 0; i <= commandlineLength; i++)
            {
                if ((cmd[i] == '|')) 
                {
                    if ((cmd[i] != '\0'))
                    {
                        if (cmd[i + 1] == '&')
                        {
                            metacharsInOrderOfOccurence[metachar_index] = "|&"; 
                        }
                        else
                        {
                            metacharsInOrderOfOccurence[metachar_index] = "|";
                        }
                    }

                    if (metachar_index == 0)
                    {
                        strncpy(line1, cmd + 0, i - 0);
                        savedIndex = i;
                    }
                    else if (metachar_index == 1)
                    {
                        strncpy(line2, cmd + savedIndex, i - savedIndex);
                        savedIndex = i;
                    }
                    else if (metachar_index == 2)
                    {
                        strncpy(line3, cmd + savedIndex, i - savedIndex);
                        savedIndex = i;
                    }
                    else if (metachar_index == 3) 
                    {
                       
                        strncpy(line4, cmd + savedIndex, i - savedIndex);
                        savedIndex = i;
                    }

                    metachar_index++;
                }
                else if ((cmd[i] == '>' && cmd[i + 1] == '&'))
                {
                    break; // not implemented
                }
                else if (cmd[i] == '\0') 
                {
                    if (metachar_index == 1)
                    {
                        strncpy(line2, cmd + savedIndex, i - savedIndex);
                        savedIndex = i;
                    }
                    else if (metachar_index == 2)
                    {
                        strncpy(line3, cmd + savedIndex, i - savedIndex);
                        savedIndex = i;
                    }
                    else if (metachar_index == 3)
                    {
                        strncpy(line4, cmd + savedIndex, i - savedIndex);
                        savedIndex = i;
                    }
                }
            }

            int strtokIndex = 0;
            char *ptr1 = strtok(line1, " ");
            while (ptr1 != NULL)
            {
                commandline_1[strtokIndex] = ptr1;
                strtokIndex++;
                ptr1 = strtok(NULL, " ");
            }
            
            memset(cmd2, 0, sizeof cmd2);
            int strtokIndex2 = 0;
            char *ptr2 = strtok(line2, metacharsInOrderOfOccurence[0]);
            while (ptr2 != NULL) 
            {
                strcat(cmd2, ptr2);
                strtokIndex2++;
                ptr2 = strtok(NULL, metacharsInOrderOfOccurence[0]);
            }
            
            strtokIndex2 = 0;
            char *ptr3 = strtok(cmd2, " ");
            while (ptr3 != NULL) 
            {
                commandline_2[strtokIndex2] = ptr3;
                strtokIndex2++;
                ptr3 = strtok(NULL, " ");
            }

            memset(cmd3, 0, sizeof cmd3);
            int strtokIndex3 = 0;
            
            if(metacharsInOrderOfOccurence[1] != NULL) 
            {
                char *ptr4 = strtok(line3, metacharsInOrderOfOccurence[1]); 
                while (ptr4 != NULL) 
                {
                    strcat(cmd3, ptr4);
                    strtokIndex3++;
                    ptr4 = strtok(NULL, metacharsInOrderOfOccurence[1]);
                }
            }
            
            strtokIndex3 = 0;
            char *ptr5 = strtok(cmd3, " ");
            while (ptr5 != NULL) 
            {
                commandline_3[strtokIndex3] = ptr5;
                strtokIndex3++;
                ptr5 = strtok(NULL, " ");
            }

            memset(cmd4, 0, sizeof cmd4);
            int strtokIndex4 = 0;
            if(metacharsInOrderOfOccurence[2] != NULL) 
            {
                char *ptr6 = strtok(line4, metacharsInOrderOfOccurence[2]);
                while (ptr6 != NULL) 
                {
                    strcat(cmd4, ptr6);
                    strtokIndex4++;
                    ptr6 = strtok(NULL, metacharsInOrderOfOccurence[2]);
                }
            }

            strtokIndex4 = 0;
            char *ptr7 = strtok(cmd4, " ");
            while (ptr7 != NULL) 
            {
                commandline_4[strtokIndex4] = ptr7;
                strtokIndex4++;
                ptr7 = strtok(NULL, " ");
            }

            if ((metachar_index + 1) == 2) 
            {
                const char **commandLines[] = {commandline_1, commandline_2};
                numCommandLinesPipe = sizeof(commandLines) / sizeof(*commandLines);
            }
            else if ((metachar_index + 1) == 3) 
            {
                const char **commandLines[] = {commandline_1, commandline_2, commandline_3};
                numCommandLinesPipe = sizeof(commandLines) / sizeof(*commandLines);
            }
            else if ((metachar_index + 1) == 4) 
            {
                const char **commandLines[] = {commandline_1, commandline_2, commandline_3, commandline_4};
                numCommandLinesPipe = sizeof(commandLines) / sizeof(*commandLines);
            }
        }
        else
        {
            launcherFlag = 3;

            char *ptr = strtok(cmd, " ");
            while (ptr != NULL)
            {
                commandline[strtok_Index1] = ptr;
                strtok_Index1++;
                ptr = strtok(NULL, " ");
            }
        }

        if (launcherFlag == 1)
        {
            processCompletionStatus = launcherOutputRedirection(commandLineOutputRedNoPipe, fileName2, outputRedir_stdErr_flag);
            fprintf(stderr, "+ completed '%s' [%d]\n", duplicateCMD, processCompletionStatus);
        }
        else if (launcherFlag == 2)
        {
            pipeReturnStatus = pipelineLoop(commandline_1, commandline_2, commandline_3, commandline_4, numCommandLinesPipe);

            char pipeReturnStatus_string[100];
            sprintf(pipeReturnStatus_string, "%d", pipeReturnStatus);

            if (numCommandLinesPipe == 2) 
            {       
                fprintf(stderr, "+ completed '%s' [%c][%c]\n", duplicateCMD, pipeReturnStatus_string[1], pipeReturnStatus_string[2]);
            }
            else if (numCommandLinesPipe == 3) 
            {
                fprintf(stderr, "+ completed '%s' [%c][%c][%c]\n", duplicateCMD, pipeReturnStatus_string[1], pipeReturnStatus_string[2], pipeReturnStatus_string[3]);
            }
            else if (numCommandLinesPipe == 4)
            {
                fprintf(stderr, "+ completed '%s' [%c][%c][%c][%c]\n", duplicateCMD, pipeReturnStatus_string[1], pipeReturnStatus_string[2], pipeReturnStatus_string[3], pipeReturnStatus_string[4]);
            }
        }
        else if (launcherFlag == 3)
        {
            processCompletionStatus = launcher(commandline);
            fprintf(stderr, "+ completed '%s' [%d]\n", duplicateCMD, processCompletionStatus);
        }
    }                                                                                                                 

    return shellExitStatus;
}

int main()
{   
    int shellExitStatus = 1;

    shellExitStatus = commandLineParser();

    fprintf(stderr, "Bye...\n");
    fprintf(stderr, "+ completed '%s' [%d]\n", "exit", shellExitStatus);

    return shellExitStatus;
}