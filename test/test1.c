#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
    // glob_t result;
    // int ret;

    // //expand the pattern .c
    // ret = glob("*.c", 0, NULL, &result);

    // if(ret == 0){
    //     for(size_t i = 0; i < result.gl_pathc; i++){
    //         printf("%s\n", result.gl_pathv[i]);
    //     }
    // }
    // else{
    //     fprintf(stderr, "Not found\n");
    // }

    // globfree(&result);

    pid_t pid = fork();

    if(pid < 0) perror("fork");
    else if(pid == 0){
        execvp("ls *.c", "ls *.c");
    }
    else
        printf("this is parent process\n");

    return 0;
}