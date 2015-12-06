#include <unistd.h>
#include <stdlib.h>


int main(int argc, char** argv)
{
        int des_p[];
        int i;
        
        
        
        if(pipe(des_p) == -1) {
          perror("Pipe failed");
          exit(1);
        }
        
        if(pipe(des_p + 2) == -1) {
          perror("Pipe failed");
          exit(1);
        }
        
        for(i=0; i<3;i++)

        if(fork() == 0)        //first fork
        {
            close(1);          //closing stdout
            dup2(des_p[1],1);      //replacing stdout with pipe write 
            close(des_p[0]);   //closing pipe read
            close(des_p[1]); 
            close(des_p[2]); 
            close(des_p[3]); 
            //close(des_p[1]);

            const char* prog1[] = { "ls", "-l", 0};
            execvp(prog1[0], prog1);
            perror("execvp of ls failed");
            exit(1);
        }
        
        if(fork() == 0)        //creating 2nd child
        {
            close(0);          //closing stdin
            close(1);          //closing stdout
            dup2(des_p[0],0);     //replacing stdin with pipe write
            dup2(des_p[3],1);     //replacing stdout with pipe read 
            close(des_p[0]);   //closing pipe read
            close(des_p[1]); 
            close(des_p[2]); 
            close(des_p[3]); 
            //close(des_p[0]);
            //close(des_p[1]);   //closing pipe write

            const char* prog2[] = { "grep", "list",  0};
            execvp(prog2[0], prog2);
            perror("execvp of wc failed");
            exit(1);
        }
        
        if(fork() == 0)        //creating 2nd child
        {
            close(0);          //closing stdin
            dup2(des_p[2],0);      //replacing stdin with pipe read
            close(des_p[0]);   //closing pipe read
            close(des_p[1]); 
            close(des_p[2]); 
            close(des_p[3]); 

            const char* prog3[] = { "sort", 0};
            execvp(prog3[0], prog3);
            perror("execvp of wc failed");
            exit(1);
        }

        close(des_p[0]);
        close(des_p[1]);
        close(des_p[2]);
        close(des_p[3]);
        wait(0);
        return 0;
}