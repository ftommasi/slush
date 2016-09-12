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

void sighandler(int signum){
  printf("^C\n");
}


int parse(char* commands){
  
  int readfd;
  int fd[2];
  if(commands){
    

    char* current_command = strtok(commands,"(");
    char* new_commands = strtok(NULL,"/0");
    char first = 0;
    char last = 0;
    if(current_command[0] == '>'){  
      printf("last command: %s\n",current_command);
      last = 1; 
    }
    if(current_command[strlen(current_command)-1] == '<'){ 
      printf("first command: %s\n",current_command);
      first = 1;
    }
    
    //must remove < and > sentinel chars

    printf("Current: %s\nRecurse: %s\n"
      ,current_command,new_commands);
    readfd = parse(new_commands);
    // if has leading space then first - oh no bad smell
    // if has trailing newline then last
    if(!first){ //not first
      printf("%s is not first command\n",current_command);
      dup2(readfd,stdin);
    }
    if(!last){ //not last
      printf("%s is not last command\n",current_command);
      dup2(fd[1],stdout);
    }
    //parse current_command & args to execute
    char* cmd  = strtok(current_command," ");    
    char* my_argv[256*256];
    int i=0;
    while(cmd){
      my_argv[i] = cmd;
      cmd = strtok(NULL," ");
      i++;
    }
    my_argv[i] = '\0';
    int j =0;
    while(my_argv[j] != '\0'){
      printf("%s ",my_argv[j]);
      j++;
    }
    char* first_arg = (char*)malloc(sizeof(char)*BUFFSIZE);
    strcpy(first_arg,my_argv[0]);
    
    if(!strcmp(first_arg,"exit")){
      printf("exiting...\n");
      exit(-1);
    }
    if(!strcmp(first_arg,"cd")){
      printf("changing directory\n");
      chdir(my_argv[1]);
    }
    
    debugDump(i,my_argv);    
    //AFTER FORK CLOSE WRITE END OF PIPE GIVEN
    
//    int fd[2];
    pipe(fd);

    int pid = fork();
    if(pid != 0){

      wait(pid,NULL,0);
    }
    else{
      
      dup2(fd[1], STDOUT_FILENO);
      close(fd[0]);
      int exec_result = execvp(my_argv[0],my_argv);
      if(exec_result == -1){
        perror("Error: ");
	exit(-1); 
      }
    }
  }
  else{
    printf("returning...\n");
    close(readfd);
    close(fd[1]);
    return fd[0];
  }
}

int main(int argc, char** argv){
   
  while(1){
    //signal(2,sighandler);
    char buf[BUFFSIZE];
    //Extra Credit
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
   printf("buf: %s\n",buf);
   char* new_buf = strtok(buf,"\n");//)char*)malloc(BUFFSIZE*sizeof(char));
   printf("new_buf: %s\n",new_buf);
   char marked_buf[256] = ">";
   strcat(marked_buf,new_buf);
   printf("after cat\n");
   char* end_sentinel = "<";
   strcat(marked_buf,end_sentinel);
   //strcat(new_buf," ");
   // strcpy(new_buf,buf);// ;
    printf("parse\n");
    parse(marked_buf);
  }

  printf("successful exit\n"); 
  return 0;
}



