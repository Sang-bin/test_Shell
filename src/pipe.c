#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>


int main(int argc, char** argv)
{
        char line[] = "ls -l";
        char *args[BUFSIZ];
        char *pipes[BUFSIZ];
        int pipen = 0;
        int argn = 0;
        char *save;
        char *s;
        int des_p[10];
        static const char delim_pipe[] = "|";
        static const char delim[] = " \t\n";
        int i,j;
        
        
        s = strtok_r(line, delim_pipe, &save);	/* 문자열에서 delim을 기준으로 단어를 잘라냄 */
		
		while(s!=NULL) {
			pipes[pipen++] = s;
			s = strtok_r(NULL, delim_pipe, &save);
		}
		
		//pipe[pipen] = s;		/* 인수가 더 없음을 의미하는 문자 추가 */
        
        for(i=0; i<pipen;i++){
            if(pipe(des_p + i*2) == -1) {
                perror("Pipe failed");
                exit(1);
            }
        }
        
        for(i=0 ; i< pipen ; i++){
            if(fork() == 0)        //first fork
            {
                if(i!=0){
                    close(0);
                    dup2(des_p[(i-1)*2],0);      //replacing stdout with pipe read 
                }
                
                if(i!=pipen-1){
                    close(1);          //closing stdout
                    dup2(des_p[i*2+1],1);      //replacing stdout with pipe write 
                }
                
                for(j=0;j<pipen*2;j++)
                    close(des_p[j]);
                    
                //close(des_p[1]);argn = 0;
    	    	s = strtok_r(pipes[i], delim, &save);	/* 문자열에서 delim을 기준으로 단어를 잘라냄 */
    	    	
    	    	if(s == NULL){
    	    	   exit(1);
    	    	}
    		
    	    	while(s!=NULL) {
    	        	args[argn++] = s;
    	        	s = strtok_r(NULL, delim, &save);
    	    	}
    		
    	    	args[argn] = s;		/* 인수가 더 없음을 의미하는 문자 추가 */
    	    	
                execvp(args[0], args);
                perror("execvp of ls failed");
                exit(1);
            }
        }
        
        for(j=0;j<pipen*2;j++)
            close(des_p[j]);
        wait(0);
        
        return 0;
}