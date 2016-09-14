#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define BUFFSIZE 200
#define READ_PIPE 0
#define WRITE_PIPE 1


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
    printf("INVALID START/END FLAGS");
    return string;
  }
}

string_list tokenize(char* string,char * delimiter){
  char** tokenized_string = (char**)malloc(sizeof(char)*BUFFSIZE*BUFFSIZE);
  int end = 0;
  int start = 0;
  int i = 0;
  int tokens = 0;
  while (string[i] != '\0'){ //guard for buffer overflow??
   
     if(!strcmp(string[i],delimiter)){
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

//GRAISON CHECK THIS OUT!! 
int parse(char* commands){
//  printf("Begin Parse: %s\n",commands); 
    
  char* current_command = strtok(commands,"(");
//  printf("current: %s\n",current_command);
  //char new_commands[256] = strtok(commands,"(");
  char* new_commands = strtok(NULL,"\0");
  
  int readfd;
//  printf("passed tokenizing\n");
  if(!current_command){
    //printf("%s\n",current_command);
    return NULL;
  }
  else{
//    printf("not null current command: %s\n",current_command);
    char first = 0;
    char last = 0;
    if(current_command[0] == '>'){  
      last = 1; 
    }
    if(current_command[strlen(current_command)-1] == '<'){ 
      first = 1;
    }
    
    //remove < and > sentinel chars
     if(last){
      current_command = slice_str(current_command,1,strlen(current_command));
 //     printf("fixed command : %s\n",current_command);
    }
    
    if(first){
      current_command = slice_str(current_command,0,strlen(current_command)-2);
  //    printf("fixed command : %s\n",current_command);
    }
    
   // printf("Current: %s\nRecurse: %s\n"      ,current_command,new_commands);
    readfd = parse(new_commands);
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
    
    //debugDump(i,my_argv);    
    //AFTER FORK CLOSE WRITE END OF PIPE GIVEN

    printf("\npiping\n");
    int fd[2];
    pipe(fd);
    int pid = fork();
    if(pid != 0){ //if parent
     // close(fd[0]);
     // close(fd[1]);
      printf("PARENT HAS READ: %d WRITE: %d\n",fd[READ_PIPE],fd[WRITE_PIPE]);
      printf("Parent pid %d\n",getpid());
      printf("Waiting for child PID: %d to execute %s\n",pid,my_argv[0]);
      //wait(pid,NULL,0);
      wait(NULL);
      printf("CHILD DONE EXECING\n");
      //close(fd[0]);
      //close(fd[1]);

    }
    else{ //if child

    printf("CHILD COMMAND: %s PIPE READ: %d PIPE WRITE: %d\n",current_command,fd[READ_PIPE],fd[WRITE_PIPE]);
    printf("\nCLOSING PIPE %d\n",fd[READ_PIPE]);
    
    //CLOSE WRITE RETURN READ
    int new_READ;// = dup2(fd[READ_PIPE],readfd);
    close(fd[READ_PIPE]);
    if(new_READ == -1 ) perror("ERROR");
    printf("Replacing %d with %d\n",fd[READ_PIPE],readfd);
    //close(fd[READ_PIPE]);  
    //A pipe automatically overtakes stdin and stdout    
    //if not first then change read from stdin to pipe
    //if not last change write from from stdout to pipe
    
    //dup2(readfd,fd[READ_PIPE]) ;

    if(//last
    !first
    ){
      //change write
      new_READ = dup2(readfd,STDIN_FILENO);
      close(fd[WRITE_PIPE]);
     // close(fd[READ_PIPE]);
      if(new_READ == -1 ) perror("ERROR");
  //    close(fd[WRITE_PIPE]);
      printf("COMMAND %s replacing %d with stdin\n",current_command,fd[READ_PIPE]);

    }

    if(//first
    !last
    ){ 
      //change read
      new_READ = dup2(fd[WRITE_PIPE],STDOUT_FILENO);
      //close(fd[READ_PIPE]);//fd[READ_PIPE]
      close(readfd);
      if(new_READ == -1 ) perror("ERROR");
    //  close(readfd);
      printf("COMMAND %s replacing %d with stdin\n",readfd);
    }
    //close(fd[READ_PIPE]);  
     //dup2(fd[WRITE_PIPE], STDOUT_FILENO);
     //close(fd[READ_PIPE]);
      printf("\nEXEC'ING <%s> READING FROM: %d WRITING TO: %d\n",my_argv[0],fd[READ_PIPE],fd[WRITE_PIPE]);
      int exec_result = execvp(my_argv[0],my_argv);
      if(exec_result == -1){
        perror("Error: ");
	exit(-1); 
      }
      printf("DONE EXECING\n");
      close(fd[0]);
      close(fd[1]);
      close(readfd);
      _exit(0);
    }
    
    //close(fd[0]);
    close(fd[1]);
    //close(readfd);

    //close();
    //printf("closing %d\n",readfd);
    //close(fd[WRITE_PIPE]);
    printf("returning %d %s from %d\n",fd[READ_PIPE],current_command,getpid());
    return fd[READ_PIPE];

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
  // printf("buf: %s\n",buf);
   char* new_buf = strtok(buf,"\n");//)char*)malloc(BUFFSIZE*sizeof(char));
//   printf("new_buf: %s\n",new_buf);
   char marked_buf[256] = ">";
   strcat(marked_buf,new_buf);
  // printf("after cat\n");
   char* end_sentinel = "<\0";
   strcat(marked_buf,end_sentinel);
   // strcpy(new_buf,buf);// ;
   //strcat(new_buf," ");
//    printf("parse\n");
    parse(marked_buf);
  }

  printf("successful exit\n"); 
  return 0;
}



