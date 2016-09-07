#include <unistd.h>

#define BUFFSIZE 200
int main(int argc, char** argv){
  while(1){
    char* buf;
    int read_result = read(STDIN_FILENO,buf,BUFFSIZE);
    int write_result = write(STDOUT_FILENO,buf,BUFFSIZE);
  }

}
