//
//  Truck.hpp
//  TruckProject
//
//  Created by Luca on 21/04/21.
//

#ifndef Truck_hpp
#define Truck_hpp

#include <stdio.h>
#include <string>
#include <iostream>

#include "caf/all.hpp"
#include "caf/io/all.hpp"

#include "InputMonitor.hpp"
#include "utils.h"

CAF_BEGIN_TYPE_ID_BLOCK(truck_block, first_custom_type_id)
CAF_ADD_ATOM(truck_block, initialize_atom)
CAF_ADD_ATOM(truck_block, send_server_atom)
CAF_ADD_ATOM(truck_block, get_new_id_atom)
CAF_ADD_ATOM(truck_block, stop_server_atom)
CAF_ADD_ATOM(truck_block, become_client_atom)
CAF_ADD_ATOM(truck_block, assign_id_atom)
CAF_ADD_ATOM(truck_block, get_new_command)
CAF_ADD_ATOM(truck_block, become_master_atom)
CAF_ADD_ATOM(truck_block, send_new_command_atom)
CAF_ADD_ATOM(truck_block, set_server_atom)
CAF_ADD_ATOM(truck_block, which_id_atom)
CAF_END_TYPE_ID_BLOCK(truck_block)




enum class operations : uint8_t {
    assign_id,
    get_id,
    command
};

enum class commands : uint32_t{
    stop,
    accellerate,
    decellerate,
    start
};

struct Truck{
    std::string sName_ = "Name";
    int32_t iId_;
    float fSpeed;
    std::map<int32_t, caf::strong_actor_ptr> mPlatoon;
    
public:
    caf::strong_actor_ptr server;
    std::string getName();
    int32_t getId();
    float getSpeed();
    
    void setName(std::string name);
    void setId(int32_t id);
    void setSpeed(float speed);
};

caf::behavior truck(caf::stateful_actor<Truck>* self);
caf::behavior master(caf::stateful_actor<Truck>* self);

caf::behavior TruckClient(caf::io::broker *self, caf::io::connection_handle hdl, const caf::actor& buddy);

caf::behavior TruckServer(caf::io::broker *self, caf::io::connection_handle hdl, const caf::actor& buddy);


caf::behavior TruckBehind(caf::io::broker *self, caf::io::connection_handle hdl,const caf::actor& buddy);
caf::behavior temp_server(caf::io::broker *self,const caf::actor& buddy);





#endif /* Truck_hpp */
