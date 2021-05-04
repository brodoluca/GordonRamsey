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
//USE BLOCKING ACTORS



struct InputMonitorState{
public:
    uint32_t iInput;
    
    static inline const char* name = "InputMonitor";
};

inline caf::behavior InputMonitor(caf::stateful_actor<InputMonitorState>* self) {
    return{
        [=](int32_t a){
            while(1){
                
                std::cout << "New Input " << "\n";
                std::cin >> self->state.iInput;
                self->send(caf::actor_cast<caf::actor>(self->current_sender()),self->state.iInput);
                
//                switch (self->state.iInput) {
//                    case 1:
//
//                        break;
//                    case 2:
//                        self->send(caf::actor_cast<caf::actor>(self->current_sender()),commands::accellerate);
//                        break;
//                    case 3:
//                        self->send(caf::actor_cast<caf::actor>(self->current_sender()),commands::decellerate);
//                        break;
//                    case 4:
//                        self->send(caf::actor_cast<caf::actor>(self->current_sender()),commands::start);
//                        break;
//
//                    default:
//                        break;
//                }
               
            }
        }
    };
}



#endif /* InputMonitor_hpp */
