#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

// ANSI color codes
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define NUM 1024
#define OPT_NUM 64

char lineCommand[NUM];  // Store user input command
char *myargv[OPT_NUM];  // Store command arguments
int lastCode = 0;      // Exit status code of the last command
int lastSig = 0;       // Signal code of the last command

// Print the shell prompt
void print_prompt() {
    char *username = getlogin();  // Get the current login username
    if (username == NULL) {
        perror("getlogin");
        exit(1);
    }

    char hostname[HOST_NAME_MAX];
    if (gethostname(hostname, sizeof(hostname)) != 0) {  // Get the hostname
        perror("gethostname");
        exit(1);
    }

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {  // Get the current working directory
        perror("getcwd");
        exit(1);
    }

    char *basename_cwd = basename(cwd);  // Extract the directory name from the full path

    // Print a colored prompt
    printf(ANSI_COLOR_RED "[%s" ANSI_COLOR_RESET "@" ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET " %s]$ ", username,
           hostname, basename_cwd);
    fflush(stdout);
}

// Handle input and output redirection
void handle_redirection() {
    int input_fd, output_fd;

    if (strstr(lineCommand, "<")) {  // If the command contains the "<" symbol
        char *input_file = strtok(lineCommand, "<");  // Split the input file
        input_file = strtok(NULL, "<");  // Get the input file name
        input_file = strtok(input_file, " ");  // Remove extra spaces
        input_file = strtok(input_file, " ");
        input_file = strtok(input_file, " ");
        input_fd = open(input_file, O_RDONLY);  // Open the input file
        if (input_fd == -1) {
            perror("open");
            exit(1);
        }
        dup2(input_fd, 0);  // Replace standard input with the input file
        close(input_fd);
    }

    if (strstr(lineCommand, ">")) {  // If the command contains the ">" symbol
        char *output_file = strtok(lineCommand, ">");  // Split the output file
        output_file = strtok(NULL, ">");  // Get the output file name
        output_file = strtok(output_file, " ");  // Remove extra spaces
        output_file = strtok(output_file, " ");
        output_file = strtok(output_file, " ");
        output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);  // Open the output file
        if (output_fd == -1) {
            perror("open");
            exit(1);
        }
        dup2(output_fd, 1);  // Replace standard output with the output file
        close(output_fd);
    }
}

// Add a color option to the "ls" command
void add_ls_color_option() {
    int i = 0;
    while (myargv[i] != NULL) {
        if (strcmp(myargv[i], "ls") == 0) {  // If the command contains "ls"
            int j = 0;
            while (myargv[j] != NULL) {
                j++;
            }
            myargv[j] = (char *) "--color=auto";  // Add the color option to "ls"
            myargv[j + 1] = NULL;
            break;
        }
        i++;
    }
}

int main() {
    while (1) {
        print_prompt();

        char *s = fgets(lineCommand, sizeof(lineCommand) - 1, stdin);
        if (s == NULL) {
            perror("fgets");
            exit(1);
        }
        lineCommand[strlen(lineCommand) - 1] = '\0';

        myargv[0] = strtok(lineCommand, " ");  // Split the command and arguments
        int i = 1;
        while (myargv[i++] = strtok(NULL, " "));

        if (myargv[0] != NULL) {
            if (strcmp(myargv[0], "cd") == 0) {  // Handle the "cd" command
                if (myargv[1] != NULL) {
                    if (chdir(myargv[1]) != 0) {
                        perror("chdir");
                    }
                } else {
                    fprintf(stderr, "cd: missing argument\n");
                }
                continue;
            }

            if (strcmp(myargv[0], "exit") == 0) {  // Handle the "exit" command
                exit(0);
            }

            if (myargv[0] != NULL && myargv[1] != NULL && strcmp(myargv[0], "echo") == 0) {  // Handle the "echo" command
                if (strcmp(myargv[1], "$?") == 0) {
                    printf("%d, %d\n", lastCode, lastSig);  // Output the exit status code and signal code of the last command
                } else {
                    printf("%s\n", myargv[1]);
                }
                continue;
            }

            pid_t id = fork();  // Create a child process
            if (id == -1) {
                perror("fork");
                exit(1);
            }

            if (id == 0) {  // In the child process
                handle_redirection();  // Handle input and output redirection
                add_ls_color_option();  // Add a color option to "ls" command
                execvp(myargv[0], myargv);  // Execute the command
                perror("execvp");
                exit(1);
            }

            int status = 0;
            pid_t ret = waitpid(id, &status, 0);  // Wait for the child process to finish
            if (ret < 0) {
                perror("waitpid");
                exit(1);
            }
            lastCode = WEXITSTATUS(status);  // Get the exit status code of the child process
            lastSig = WTERMSIG(status);  // Get the signal code of the child process
        }
    }
}
