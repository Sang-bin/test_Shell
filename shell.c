#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#define MAXARG 7

int main()
{
    char buf[256];
    char path[BUFSIZ];
    char *args[MAXARG];
    char *s;
    char *save;
    int argn;
    static const char delim[] = " \t\n"; /* 공백, 탭, 개행으로 이루어진 구분자 선언 */
    int pid, status;

    while(1) {  			/* 무한 반복 */
		getcwd(path,BUFSIZ);
		printf("[shell] %s : ",path);		/* 프롬프트 출력 */
		gets(buf);
		argn = 0;
		
		s = strtok_r(buf, delim, &save);	/* 문자열에서 delim을 기준으로 단어를 잘라냄 */
		while(s!=NULL) {
			args[argn++] = s;
			s = strtok_r(NULL, delim, &save);
		}
       
		args[argn] = '\0';		/* 인수가 더 없음을 의미하는 문자 추가 */	

		if(!strcmp(args[0],"cd")){
			if(chdir(args[1]))
			printf("디렉토리 이동 실패\n");	
			continue;
		}
		
		if (!strcmp(args[0], "quit")) 	/* 프롬프트로 입력받은 단어가‘quit’이면 while 문 벗어남 */
			break;

		if ((pid=fork()) == -1)      	/* fork 호출에 실패하면 */
			perror("fork failed");
		else if (pid != 0) {         	/* 부모 프로세스는 자식 프로세스가 종료되기를 기다림 */
			pid = wait(&status);
       
		} else {                     	/* 자식 프로세스는 execvp를 이용하여 arg[0] 실행 */
			execvp(args[0], args);
			_exit(0);
		}
	}
    _exit(0);
}
