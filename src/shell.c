#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define MAXARG 7

char path[BUFSIZ];

unsigned long directoryOnFileSize(DIR *,char path[],int mode);
char* order(char *args);
int customCommend(char *args[]);
void ForkCommend(char buf[]);

int main()
{
    char buf[256];
    char *args[MAXARG];
    char *s;
    char *save;
    int argn;
	unsigned long size;
    static const char delim[] = " \t\n"; /* 공백, 탭, 개행으로 이루어진 구분자 선언 */
    int pid, status;

    while(1) {  			/* 무한 반복 */
		getcwd(path,BUFSIZ);
		printf("[shell] %s : ",path);		/* 프롬프트 출력 */
		
		gets(buf);
		
		argn = 0;
		
		if(strchr(buf,'|')!=NULL){
			ForkCommend(buf);
			continue;
		}else if((s = strtok_r(buf, delim, &save)) == NULL){
			printf("명령어를 입력해주세요.\n");
			continue;
		}
		
		while(s!=NULL) {
			args[argn++] = s;
			s = strtok_r(NULL, delim, &save);
		}
		
		args[argn] = s;		/* 인수가 더 없음을 의미하는 문자 추가 */
		
		args[0] = order(args[0]);  // 한글 명령어 변경
		
		if (!strcmp(args[0], "exit")) 	/* 프롬프트로 입력받은 단어가‘exit’이면 while 문 벗어남 */
			break;
			
		if(customCommend(args))
			continue;

		if ((pid=fork()) == -1)      	/* fork 호출에 실패하면 */
			perror("fork failed");
		else if (pid != 0) {         	/* 부모 프로세스는 자식 프로세스가 종료되기를 기다림 */
			pid = wait(&status);
		} else {                     	/* 자식 프로세스는 execvp를 이용하여 arg[0] 실행 */
			execvp(args[0], args);
			perror(args[0]);
			_exit(0);
		}
	}
    _exit(0);
}

unsigned long directoryOnFileSize(DIR *dp,char path[],int mode){//mode 1 마스터 mode 0 슬레이브
	struct dirent *d;
	struct stat st;
	char path_temp[BUFSIZ+1];
	unsigned long sum_size = 0;
	unsigned long temp = 0;
	static int count = 0;
	
	if (dp == NULL){ // 디렉터리 열기
   		fprintf(stderr,"위치에 폴더가 없습니다.");
   	   return 0;
	}
	  
	while ((d = readdir(dp)) != NULL) { // 디렉터리의 각 파일에 대해
    		sprintf(path_temp, "%s/%s", path, d->d_name); // 파일경로명 만들기
     	if (lstat(path_temp, &st) < 0) // 파일 상태 정보 가져오기
       		return 0;
		 
	if(S_ISDIR(st.st_mode)){
		if((d->d_name[0]) == '.'&& ((d->d_name[1]) == '.' || (d->d_name[1]) == '\0'));
		else{
			sum_size += directoryOnFileSize(opendir(path_temp),path_temp,0);
		}
	}
	sum_size += st.st_size;
	count++;
   }
   
   if(mode == 1){
   		fprintf(stdout,"Total count : %d\nTotal Size : %zu\n", count,sum_size);
		count = 0;
   }
   
   closedir(dp);
   
   return sum_size;
}

char* order(char *args){
        if(!strcmp(args,"나가기")){
			return "exit";
        }else if(!strcmp(args,"목록")){
			return "ls";
        }else if(!strcmp(args,"이동")){
			return "cd";
        }else if(!strcmp(args,"삭제")){
			return "rm";
        }else if(!strcmp(args,"폴더삭제")){
			return "rmdir";
		}else if(!strcmp(args,"폴더생성")){
			return "mkdir";
		}else if(!strcmp(args,"내용")){
			return "cat";
		}else if(!strcmp(args,"복사")){
			return "cp";
		}else if(!strcmp(args,"이동")){
			return "mv";
		}else if(!strcmp(args,"파일찾기")){
			return "find";
		}else
			return args;
}

void ForkCommend(char buf[]){
        char line[BUFSIZ];
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
        int status,pid;
        
        s = strtok_r(buf, delim_pipe, &save);	/* 문자열에서 delim을 기준으로 단어를 잘라냄 */
		
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
            if((pid = fork()) == 0)        //first fork
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
    	    	   break;
    	    	}
    		
    	    	while(s!=NULL) {
    	        	args[argn++] = s;
    	        	s = strtok_r(NULL, delim, &save);
    	    	}
    	    	
    		
    	    	args[argn] = s;		/* 인수가 더 없음을 의미하는 문자 추가 */
    	    	
				args[0] = order(args[0]);  // 한글 명령어 변경
				
				if(customCommend(args))
					continue;
				
                execvp(args[0], args);
                perror(args[0]);
                break;
            }
        }
        
        for(j=0;j<pipen*2;j++)
            close(des_p[j]);
            
        while( (pid=wait(&status)) > 0 );/*	
	    {
    		 fprintf(stderr, "i:%i process ID:%ld parent ID:%ld\n", (status>>8), (long)pid, (long)getpid());
    	}*/
}

int customCommend(char *args[]){
		if(!strcmp(args[0],"dirsize")){
			if(args[1] != NULL){
				directoryOnFileSize(opendir(args[1]),args[1],1);				
			}else
				directoryOnFileSize(opendir(path),path,1);
			return 1;
   	   	}else if(!strcmp(args[0],"cd")){
			if(chdir(args[1]))
				printf("디렉토리 이동 실패\n");	
			return 1;
		}else
   	   		return 0;
}