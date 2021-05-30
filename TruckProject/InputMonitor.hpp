//
//  InputMonitor.hpp
//  Testing_brokers
//
//  Created by Luca on 22/04/21.
//

#ifndef InputMonitor_hpp
#define InputMonitor_hpp

#include <stdio.h>
#include "caf/all.hpp"
#include "caf/io/all.hpp"
#include "Truck.hpp"
//Do timeout
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <string>

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

///This is legit the most broken, ghetto, stupid solution I have ever designed.
///This works 30% of the time and it's ugly as hell
///basically, it's meant to be like an input monitor function with a certain timeout. But it doesnt work very well. I'm not going to modify it.
/// 30% of the time is good enough
struct InputMonitorState{
public:
    uint32_t iInput;
    static inline const char* name = "InputMonitor";
};


inline caf::behavior InputMonitor(caf::stateful_actor<InputMonitorState>* self) {
    std::cout << "INput monitor spawn\n";
    return{
        [&](int32_t a){
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
                uint32_t a = arr1[0] - '0';
                self->send(caf::actor_cast<caf::actor>(self->current_sender()),a);
            }
        }
    };
};


//    struct pollfd pfd = { STDIN_FILENO, POLLIN, 0 };
//    std::string line;
//    int ret = 0;
//    return{
//        [&](int32_t a){
//            ret = poll(&pfd, 1, 10000);  // timeout of 10000ms
//            if(ret == 1) // there is something to read
//            {
//                std::cout << "New Input:" << "\n";
//                std::getline(std::cin, line);
//                if(line.length()==1){
//                    uint32_t b =stoi(line);
//                    self->send(caf::actor_cast<caf::actor>(self->current_sender()),b);
//                }else{
////                    self->quit();
//                }
//            }
//
//            else if(ret == 0)
//            {
//                std::cout << "Done:" << "\n";
////                self->quit();
//            }
//
//        }

#endif /* InputMonitor_hpp */
