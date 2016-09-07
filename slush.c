#include <unistd.h>
#include <stdio.h>

#define BUFFSIZE 200

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

char** tokenize(char* string){
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
  i=0;
  for (i; i < tokens+1 ; i++){
      printf("%s\n",tokenized_string[i]);
  }

  return tokenized_string;
}

int main(int argc, char** argv){
   
 // while(1){
    char* buf;
    int prompt = write(STDOUT_FILENO,"SLUSH> ",7);
    int read_result = read(STDIN_FILENO,buf,BUFFSIZE);
    char** tokenize(buf);
    int write_result = write(STDOUT_FILENO,buf,read_result);
 // }

}
