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
      char temp = string[start];
      sliced[i] = temp;
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

int main(int argc, char** argv){
   
  while(1){
    char buf[BUFFSIZE];
    int prompt = write(STDOUT_FILENO,"SLUSH> ",7);
    int read_result = read(STDIN_FILENO,buf,BUFFSIZE);
//    write(STDOUT_FILENO,buf,read_result);
    if(read_result == -1){
      perror("Error:");
      break;
    }
    char* new_buf = strtok(buf,"\n");
    
    char** my_argv[BUFFSIZE];
    char* cmd  = strtok(new_buf," ");    
    int i=0;
    while(cmd){
      argv[i] = cmd;
      if(strcmp(argv[i],"exit"))
        return -1;
      cmd = strtok(NULL," ");
      //cmd = strtok(NULL,"(");
      i++;
    }
    my_argv[i] = '\0';    
    int j =0;
    printf("[ ");
    for(j; j < i+1; j++){
      printf("%s",argv[j]);
      if(j != i)
        printf(", ");
      else
        printf(" ");
    }
    printf("]\n");
  }
    
  return 0;
}
