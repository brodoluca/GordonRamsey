//
//  main.cpp
//  pipe1
//
//  Created by Luca on 30/05/21.
//

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <array>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>



void pipcommnand(std::string command){
    std::array<char, 80> buffer;
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr<<"Cannot open the pipe"<<std::endl;
        return;
    }
    int c = 0;
    while (fgets(buffer.data(), 80, pipe)!=NULL) {
        c++;
        std::cout << c << " "<<buffer.data()<<std::endl;
    }
    pclose(pipe);
}
void pipe_write(){
    int fd;
      
        // FIFO file path
    std::string myfifo = "/Users/brodie/Desktop/new";
      
        // Creating the named file(FIFO)
        // mkfifo(<pathname>, <permission>)
        mkfifo(myfifo.c_str(), 0666);
      
        char arr1[80], arr2[80];
        while (1)
        {
            // Open FIFO for write only
            fd = open(myfifo.c_str(), O_WRONLY);
      
            // Take an input arr2ing from user.
            // 80 is maximum length
            fgets(arr2, 80, stdin);
      
            // Write the input arr2ing on FIFO
            // and close it
            write(fd, arr2, strlen(arr2)+1);
            close(fd);
      
            // Open FIFO for Read only
            fd = open(myfifo.c_str(), O_RDONLY);
      
            // Read from FIFO
            read(fd, arr1, sizeof(arr1));
      
            // Print the read message
            printf("User2: %s\n", arr1);
            close(fd);
        }
        return;
}


int main(int argc, const char * argv[]) {

    int fd;
      
        // FIFO file path
    std::string temp ="/Users/brodie/Desktop/new/myfifo";
        auto myfifo = temp.c_str();
      
        // Creating the named file(FIFO)
        // mkfifo(<pathname>, <permission>)
        mkfifo(myfifo, 0666);
      
        char arr1[80];
        while (1)
        {
            fd = open(myfifo, O_RDONLY);
            read(fd, arr1, sizeof(arr1));
            printf("User2: %s\n", arr1);
            close(fd);
        }
        return 0;
    

}
