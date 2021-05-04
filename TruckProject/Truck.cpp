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
    self->attach_functor([=](const caf::error& reason) {
        std:: cout << "Im down";
        self->send_exit(caf::actor_cast<caf::actor>(self->state.server),caf::exit_reason::remote_link_unreachable);
    });
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
            self->send(caf::actor_cast<caf::actor>(self->current_sender()), is_master_atom_v);
        },
        
        [=](you_are_master_atom) {
            aout(self) << "I am the new master\n";
            self->send(caf::actor_cast<caf::actor>(self->current_sender()), you_are_master_atom_v);
        },
        [=](tell_back_im_master_atom) {
            self->send(caf::actor_cast<caf::actor>(self->state.server), tell_back_im_master_atom_v);
        },
        [=](get_host_port_atom) {
            return std::make_pair(int32_t(self->state.getPort()),self->state.getHost());
        },
        [=](set_master_connection_atom, bool val) {
            self->state.setMasterConnection(val);
            std::cout<< val << "New conn";
        },
        [=](is_master_atom) {
            return self->state.isMasterConnection();
            
        },[=](get_host_atom) {
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
            aout(self) << "I am the new master\n";
            self->become(master(self));
            self->state.setId(64);
        },[=](set_server_atom){
            self->state.server = self->current_sender();
            self->attach_functor([=](const caf::error& reason) {
                std:: cout << "Im down";
                self->send_exit(caf::actor_cast<caf::actor>(self->state.server),caf::exit_reason::remote_link_unreachable);
            });
            
        },[=](update_id_behind_atom) {
            self->anon_send(caf::actor_cast<caf::actor>(self->state.server), update_id_behind_atom_v,self->state.getId()-1);
        }
    };
    
}





caf::behavior master(caf::stateful_actor<Truck>* self){;
    return {
        [=](assign_id_atom){
            return self->state.getId() - 1;
        },[=](update_id_behind_atom) {
            self->anon_send(caf::actor_cast<caf::actor>(self->state.server), update_id_behind_atom_v,self->state.getId()-1);
        },
        
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

bool Truck::isMasterConnection(){
    return bMasterConnection;
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

void Truck::setMasterConnection(bool res){
    bMasterConnection = res;
}
