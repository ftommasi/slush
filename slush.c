#include <unistd.h>

#define BUFFSIZE 200

char** tokenize(char* string){
  char** tokenized_string;
  int end = 0;
  int beginning = 0;
  int i = 0;
  while (string[i] != '\0'){
   
     if(string[i] == ' ' || string[i] == '(' || string[i] == ')'){
       
     }
     i++;
     end = i;
  }
  return tokenized_string;
}

int main(int argc, char** argv){
  while(1){
    char* buf;
    int read_result = read(STDIN_FILENO,buf,BUFFSIZE);
    char** tokenize(buf);
    int write_result = write(STDOUT_FILENO,buf,BUFFSIZE);
  }

}
