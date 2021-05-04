//
//  Truck.cpp
//  TruckProject
//
//  Created by Luca on 21/04/21.
//

#include "Truck.hpp"

//
// Implementation of the Truck Actor
//

caf::behavior truck(caf::stateful_actor<Truck>* self){
    
    
    auto server = self->home_system().middleman().spawn_server(temp_server, 3232, caf::actor_cast<caf::actor>(self));
    
    return{
        [=](initialize_atom,std::string name) {
            self->state.setName(name);
            aout(self)<<self->state.getName() + " has been spawned \n";
            self->anon_send(caf::actor_cast<caf::actor>(self->current_sender()), initialize_atom_v);
        },
        [=](set_front_id, int32_t newID) {
            return self->state.setFrontId(newID);
        },
        [=](which_id_atom) {
            return self->state.getId();
        },
        [=](get_new_id_atom, int32_t newID) {
            self->state.setId(newID);
        },
        [=](get_host_port_atom) {
            return std::make_pair(int32_t(self->state.getPort()),self->state.getHost());
        },
        [=](get_host_atom) {
            return self->state.getHostC();
        },
        [=](update_port_host_atom, uint16_t newPort, std::string newHost) {
            self->state.setPort(newPort);
            self->state.setHost(newHost);
        },
        [=](get_new_command, int32_t command) {
            switch (static_cast<commands>(command)) {
                case commands::stop:
                    self->state.setSpeed(0);
                    std::cout << self->state.getName()+" stops: " + std::to_string(self->state.getSpeed()) +"\n";
                    if (self->state.server)
                        self->anon_send(caf::actor_cast<caf::actor>(self->state.server), send_new_command_atom_v, static_cast<uint32_t>(command));
        
                    break;
                case commands::start:
                    self->state.setSpeed(100);
                    std::cout << self->state.getName()+" starts: " + std::to_string(self->state.getSpeed()) +" \n";
                    if (self->state.server)
                        self->anon_send(caf::actor_cast<caf::actor>(self->state.server), send_new_command_atom_v,static_cast<uint32_t>(command));
                    break;  
                case commands::accellerate:
                    self->state.setSpeed(self->state.getSpeed()+10);
                    std::cout << self->state.getName()+" accellerates: " + std::to_string(self->state.getSpeed()) +" \n";
                    if (self->state.server)
                        self->anon_send(caf::actor_cast<caf::actor>(self->state.server), send_new_command_atom_v, static_cast<uint32_t>(command));
                    break;
                case commands::decellerate:
                    self->state.setSpeed(self->state.getSpeed()-10);
                    std::cout << self->state.getName()+" decellerates: " + std::to_string(self->state.getSpeed()) +" \n" ;
                    if (self->state.server)
                        self->anon_send(caf::actor_cast<caf::actor>(self->state.server), send_new_command_atom_v, static_cast<uint32_t>(command));
                    break;
                default:
                    break;
            
            }
            
        },
        [=](become_master_atom){
            self->become(master(self));
            self->state.setId(64);
        },[=](set_server_atom){
            self->state.server = self->current_sender();
        }
    };
    
}
caf::behavior master(caf::stateful_actor<Truck>* self){;
    return {
        [=](assign_id_atom){
            return self->state.getId() - 1;
        }
        
    };
}


//
//    Getters
//
std::string Truck::getName(){
    return sName_;
}
int32_t Truck::getId(){
    return iId_;
}
int32_t Truck::getFrontId(){
    return iFrontId_;
}
float Truck::getSpeed(){
    return fSpeed;
}
std::string Truck::getHost(){
    return sHost;
}
uint16_t Truck::getPort(){
    return uPort;
}
const char* Truck::getHostC(){
    char* temp=new char(17);
    std::strcpy(temp, sHost.c_str());
    for(unsigned long i=sHost.length();i<17;i++)
        temp[i] = '-';
    return temp;
}

//
//    Setters
//

void Truck::setName(std::string name){
    sName_ = std::move(name);
};
void Truck::setId(int32_t id){
    iId_ = id;
};
void Truck::setFrontId(int32_t id){
    iFrontId_ = (id == 1)? 64 : id;
};
void Truck::setSpeed(float speed){
    fSpeed = speed;
};

void Truck::setHost(std::string host){
    sHost = host;
}

void Truck::setPort(uint16_t port){
    uPort = port;
}

