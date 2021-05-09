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
//    auto server = self->home_system().middleman().spawn_server(temp_server, 3232, caf::actor_cast<caf::actor>(self));
    self->attach_functor([=](const caf::error& reason) {
        std:: cout << "Im down";
        self->send_exit(caf::actor_cast<caf::actor>(self->state.server),caf::exit_reason::remote_link_unreachable);
    });
    return{
        [](int a){
                     std::cout << "ok\n";
        },
        [=](initialize_atom,std::string name) {
            self->state.setName(name);
            std::cout<<self->state.getName() + " has been spawned \n";
            self->state.client = self->current_sender();
            self->anon_send(caf::actor_cast<caf::actor>(self->current_sender()), initialize_atom_v);
        },[=](set_front_id, int32_t newID) {
            return self->state.setFrontId(newID);
        },
        [=](which_id_atom) {
            return self->state.getId();
        },
        [=](which_front_id_atom) {
            return self->state.getFrontId();
        },
        [=](get_new_id_atom, int32_t newID) {
            self->state.setId(newID);
            self->send(caf::actor_cast<caf::actor>(self->current_sender()), is_master_atom_v);
        },
        [=](you_are_master_atom) {
            std::cout << "I am the new master\n";
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
//            std::cout<< val << "New conn";
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
            std::cout << "I am the new master\n";
            self->become(master(self));
            self->state.setId(64);
            self->send(self, update_id_behind_atom_v);
            self->send(self, tell_back_im_master_atom_v);
//            let's wait a bit, we dont want to overflow the buffer
            self->delayed_send(self,std::chrono::seconds(2) ,update_port_host_atom_v);
        },[=](set_server_atom){
            self->state.server = self->current_sender();
            self->attach_functor([=](const caf::error& reason) {
                std:: cout << "Server down";
                self->send_exit(caf::actor_cast<caf::actor>(self->state.server),caf::exit_reason::remote_link_unreachable);
            });
        
        },[=](update_id_behind_atom) {
            self->anon_send(caf::actor_cast<caf::actor>(self->state.server), update_id_behind_atom_v,self->state.getId()-1);
        },
        [=](tell_back_im_master_atom) {
            self->send(caf::actor_cast<caf::actor>(self->state.server), tell_back_im_master_atom_v);
        },[=](update_port_host_atom) {
//            self->send(caf::actor_cast<caf::actor>(self->state.server), update_port_host_atom_v);
        },[=](update_master_atom, std::string host, uint16_t port) {
            auto impl = self->home_system().middleman().spawn_client(TruckClient, host, port, caf::actor_cast<caf::actor>(self->address()));
        
        },[=](increment_number_trucks_atom, uint32_t newPlatoon){
            self->anon_send(caf::actor_cast<caf::actor>(self->state.client), increment_number_trucks_atom_v);
            self->state.tqPlatoon += newPlatoon;
            std::cout << "PLATOON: "<<self->state.tqPlatoon<<"\n";
        
        },[=](update_truck_numbers_atom, uint32_t a ) {
            self->state.tqPlatoon = a;
            std::cout << "PLATOON : " << self->state.tqPlatoon;
        },
        [=](cascade_port_host_atom, uint16_t newPort, std::string newHost, truck_quantity stopID) {
            if (stopID == self->state.getId()) {
//                place holder
                std::cout << "Hey, that's it\n";
            }else{
                self->anon_send(caf::actor_cast<caf::actor>(self->state.server), cascade_port_host_atom_v, newPort, newHost, stopID);
            }
        },[=](get_truck_numbers_atom) {
            return self->state.tqPlatoon;
        },
    };
    
}



caf::behavior master(caf::stateful_actor<Truck>* self){
    
    
    return {
        [=](become_master_atom){
            std::cout << "I am the new master\n";
            self->anon_send(caf::actor_cast<caf::actor>(self->current_sender()), you_are_master_atom_v);
        },
        [=](assign_id_atom){
            return self->state.getId() - 1;
        },[=](update_id_behind_atom) {
            self->anon_send(caf::actor_cast<caf::actor>(self->state.server), update_id_behind_atom_v,uint32_t{self->state.getId()-uint32_t(1)});
        },
        [=](tell_back_im_master_atom) {
            self->send(caf::actor_cast<caf::actor>(self->state.server), tell_back_im_master_atom_v);
        },[=](update_port_host_atom) {
            self->send(caf::actor_cast<caf::actor>(self->state.server),update_truck_behind_port_host_atom_v, self->state.getPort(), self->state.getHost());
        },[=](get_host_port_atom) {
            return std::make_pair(int32_t(self->state.getPort()),self->state.getHost());
        },[=](increment_number_trucks_atom, truck_quantity platoon) {
            self->state.tqPlatoon = platoon;
            std::cout << "PLATOON : " << self->state.tqPlatoon;
        },[=](increment_number_trucks_atom) {
            self->state.tqPlatoon = self->state.tqPlatoon+1;
            std::cout << "PLATOON : " << self->state.tqPlatoon;
        },[=](get_truck_numbers_atom) {
            return self->state.tqPlatoon;
        },[=](update_truck_numbers_atom, uint32_t a ) {
            self->state.tqPlatoon = a;
            std::cout << "PLATOON : " << self->state.tqPlatoon;
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
