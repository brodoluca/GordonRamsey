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

#include "string.h"
#include <cstdint>
#include <iomanip>

#include "caf/all.hpp"
#include "caf/io/all.hpp"

#include "InputMonitor.hpp"
#include "utils.h"
using truck_quantity = int32_t;

struct Truck{
public:
    
    caf::strong_actor_ptr server;
    caf::strong_actor_ptr client;
    truck_quantity tqPlatoon = 1;
    bool isMasterConnection();
    std::string getName();
    int32_t getId();
    int32_t getFrontId();
    uint16_t getPort();
    const char* getHostC();
    std::string getHost();
    float getSpeed();
    
    void setMasterConnection(bool res);
    void setName(std::string name);
    void setId(int32_t id);
    void setFrontId(int32_t id);
    void setSpeed(float speed);
    void setPort(uint16_t port);
    void setHost(std::string host);
    
    
private:
    
    std::string sName_ = "Name";
    int32_t iId_;
    int32_t iFrontId_;
    float fSpeed;
    uint16_t uPort = 4242;
    std::string sHost = "localhost";
    bool bMasterConnection; //are we connected to the master directly?
//    Old way of thinking, should be changed into ip addres and port.
    std::map<int32_t, caf::strong_actor_ptr> mPlatoon;
    
    
};

caf::behavior truck(caf::stateful_actor<Truck>* self);
caf::behavior master(caf::stateful_actor<Truck>* self);

caf::behavior TruckServerMaster(caf::io::broker *self, caf::io::connection_handle hdl, const caf::actor& buddy);
caf::behavior temp_master_server(caf::io::broker* self, const caf::actor& buddy);

caf::behavior TruckBehind(caf::io::broker *self, caf::io::connection_handle hdl,const caf::actor& buddy);
caf::behavior temp_server(caf::io::broker *self,const caf::actor& buddy);

caf::behavior TruckClient(caf::io::broker *self, caf::io::connection_handle hdl, const caf::actor& buddy);



#endif /* Truck_hpp */