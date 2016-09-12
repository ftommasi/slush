#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define BUFFSIZE 200

typedef struct string_list{
  char** string_array;
  int size;
}string_list;

char* slice_str(char* string,int start, int end){
  if(start < end){
    char* sliced = (char*)malloc(sizeof(char)*(start+end));
    int i = 0;
    for(start; start < end+1; start++){
      char first_arg = string[start];
      sliced[i] = first_arg;
      i++;
    }
   sliced[i] = '\0';
   return sliced; 
  }
  else{
    //implement string reversal?
    printf("INVALID START/END FLAGS");
    return string;
  }
}

string_list tokenize(char* string){
  char** tokenized_string = (char**)malloc(sizeof(char)*BUFFSIZE*BUFFSIZE);
  int end = 0;
  int start = 0;
  int i = 0;
  int tokens = 0;
  while (string[i] != '\0'){ //guard for buffer overflow??
   
     if(string[i] == ' ' || string[i] == '(' || string[i] == ')'){
       //we should keep reading for consecutive spaces/parens/other token delimiters here
       char* token = slice_str(string,start,end);
       tokenized_string[tokens] = token;
       end++;
       start = end;
       tokens++;
     }

     i++;
     end = i;
     
  }
  tokenized_string[tokens] = slice_str(string,start,end-1);
  tokens++;
  string_list tokenized;
  
  tokenized.string_array = tokenized_string;
  tokenized.size = tokens;
  return tokenized;

}

void debugDump(int i,char** my_argv){
    //Debug Dump
    int j =0;
    printf("[ ");
    for(j; j < i+1; j++){
      printf("%s",my_argv[j]);
      if(j != i)
        printf(", ");
      else
        printf(" ");
    }
    printf("]\n");
    //end Debug dump

}


/*
void pipe_commands(stack commands){
  if(commands.empty)
    return;
  tokenized = tokenize();
  stack.addAll(tokenized);
  pid = fork();
  if(pid == parent)
    wait(child_pid);
    close(fd[0]);
  else
    dup2(fd[0],STDOUT_FILENO);
    pipe_commands(stack.pop);
  //def parse():
    //parse()
    //fork()
    //exec()
}
*/

void pipe_commands(){
  int fd[2];
  pipe(fd);

  int pid = fork();
  if(!pid){
    waitpid(pid,NULL,0);
  } 
  else {
    dup2(fd[1], STDOUT_FILENO);
    close(fd[0]);
    //child 1 exec
    char* cmd = "./program1";
    char* myargv[] = {"program1", '\0'};
    int ret = execvp( cmd, myargv );
    if( ret == -1 ) perror("Error exec'ing program1");
  }

  pid = fork();
  if(!pid){
    waitpid(pid,NULL,0);
  } 
  else {
    dup2(fd[0], STDIN_FILENO);
    close(fd[1]);
    //child 2 exec
    char* cmd = "./program2";
    char* myargv[] = {"program2", '\0'};
    int ret = execvp( cmd, myargv );
    if( ret == -1 ) perror("Error exec'ing program2");
  }

  close(fd[1]);
  close(fd[0]);

}


int main(int argc, char** argv){
   
  while(1){
    char buf[BUFFSIZE];
    //Extra Credit
    //TODO BUG ODD NUMBER PWD shows dirs in reverse
    //i.e. EXPECTED studios/studio6 RESULT studio6/studios
    //     PWD = /student/ftommasi/csci3500/studios/studio6
    char * cwd = get_current_dir_name();
    char* tail_cwd[2];
    int current = 0;
    char* new_cwd = strtok(cwd,"/");
    while(new_cwd){
      tail_cwd[0] = tail_cwd[1];
      tail_cwd[1] = new_cwd;
      current= (current+1)%2;
      new_cwd = strtok(NULL,"/");

    }
    char display_cwd[256];
    strcpy(display_cwd,tail_cwd[0]);
    strcat(display_cwd,"/");
    strcat(display_cwd,tail_cwd[1]);
    strcat(display_cwd,"/");
    int prompt = write(STDOUT_FILENO,"SLUSH|",6);
    prompt = write(STDOUT_FILENO,display_cwd,
      strlen(display_cwd));
    prompt = write(STDOUT_FILENO,"> ",2);
    //End Extra Credit
    int read_result = read(STDIN_FILENO,buf,BUFFSIZE);
    if(read_result == -1){
      perror("Error:");
      return -1;
    }
    
    if(read_result == 0){
      printf("Found EOF\n");
      return 0;
   }
    char* new_buf = strtok(buf,"\n");
    if(new_buf){
      char** my_argv[BUFFSIZE*BUFFSIZE];
      printf("BEFORE FIRST STRTOK newbuf:%s\n",new_buf);
      char* cmd  = strtok(new_buf," ");    
      int i=0;
      while(cmd){
        my_argv[i] = cmd;
        cmd = strtok(NULL," ");
        //TODO add piping logic
        i++;
      }
      my_argv[i] = '\0';
      printf("MADE IT OUT\n") ;
      char* first_arg = (char*)malloc(sizeof(char)*BUFFSIZE);
      printf("BEFORE STRCPY\n");
      strcpy(first_arg,my_argv[0]);
      printf("MADE IT OUT\n");
      if(!strcmp(first_arg,"exit")){
        printf("exiting...\n");
        return -1;
      }
      if(!strcmp(first_arg,"cd")){
        printf("changing directory\n");
        chdir(my_argv[1]);
      }
      printf("BEFORE DEBUG DUMP\n");
      debugDump(i,my_argv);    
      printf("MADE IT OUT\n");
      int pid = fork();
      if(pid != 0){
        waitpid(pid,NULL,0);
      }
      else{
        int exec_result = execvp(my_argv[0],my_argv);
	if(!exec_result){
	  perror("Error: ");
	}
      }
    }
  }

  //END PARSE
  
  printf("successful exit\n"); 
  return 0;
}



