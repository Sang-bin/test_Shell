

char* order(char *args){
        if(!strcmp(args,"나가기")){
			return "exit";
        }else if(!strcmp(args[0],"목록")){
			return "ls";
        }else if(!strcmp(args[0],"이동")){
			return "cd";
        }else if(!strcmp(args[0],"삭제")){
			return "rm";
        }else if(!strcmp(args[0],"폴더삭제")){
			return "rmdir";
		}else if(!strcmp(args[0],"폴더생성")){
			return "mkdir";
		}else if(!strcmp(args[0],"내용")){
			return "cat";
		}else if(!strcmp(args[0],"복사")){
			return "cp";
		}else if(!strcmp(args[0],"이동")){
			return "mv";
		}else if(!strcmp(args[0],"파일찾기")){
			return "find";
		}else
			return args;
}