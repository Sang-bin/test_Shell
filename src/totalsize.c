#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char type(mode_t);
char *perm(mode_t);
void printStat(char*, char*, struct stat*);
unsigned long directoryOnFileSize(DIR *,char *);

int main(int argc, char **argv)
{
   char *dir;
   unsigned long size = 0;

   if (argc == 1){
      dir = ".";
   }else if(argc == 2)
      dir = argv[1];
	else
		perror(dir);


	size = directoryOnFileSize(opendir(dir),dir);
	
	printf("Total Size : %zu\n",size);
   exit(0);
}

unsigned long directoryOnFileSize(DIR *dp,char * path){
	struct dirent *d;
	struct stat st;
	char path_temp[BUFSIZ+1];
	unsigned long sum_size = 0;
	unsigned long temp = 0;
	
	if (dp == NULL) // 디렉터리 열기
   	   return 0;
	  
	while ((d = readdir(dp)) != NULL) { // 디렉터리의 각 파일에 대해
    		sprintf(path_temp, "%s/%s", path, d->d_name); // 파일경로명 만들기
		printf("%-15s",path_temp);
     	if (lstat(path_temp, &st) < 0) // 파일 상태 정보 가져오기
       		return 0;
		 
	if(S_ISDIR(st.st_mode)){
		printf(" D");
		if((d->d_name[0]) == '.'&& ((d->d_name[1]) == '.' || (d->d_name[1]) == '\0'))	
		   printf(" not Found") ;
		else{
			printf("\n");
			sum_size += directoryOnFileSize(opendir(path_temp),path_temp);
			printf("%27s","") ;
		}
	}else{
		printf("%12s","");
	}
	sum_size += st.st_size;

	printf(" %10zu",st.st_size);
	printf(" %10zu\n",sum_size);
   }
   
   closedir(dp);
   return sum_size;
}
