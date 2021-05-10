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
        std:: cout << "[TRUCK]: There is an error. REASON = "<< reason.category();
        self->send_exit(caf::actor_cast<caf::actor>(self->state.server),caf::exit_reason::remote_link_unreachable);
    });
    return{
//        Initialize the truck with a name and we also save the reference to the client
        [=](initialize_atom,std::string name) {
            self->state.setName(name);
            std::cout<<self->state.getName() + " has been spawned \n";
            self->state.client = self->current_sender();
//            Returns number of trucks in the platton
        },[=](get_truck_numbers_atom) {
            return self->state.tqPlatoon;
        },
//        check wether the connection is to a master
        [=](is_master_atom) {
            return self->state.isMasterConnection();
//            return the ip address of the host AS A C STRING
        },[=](get_host_atom) {
            return self->state.getHostC();
        },
//        returns truck id
        [=](which_id_atom) {
            return self->state.getId();
        },
//        returns front truck id
        [=](which_front_id_atom) {
            return self->state.getFrontId();
        },
//        return the port and the host AS A C++ STRING
        [=](get_host_port_atom) {
            return std::make_pair(int32_t(self->state.getPort()),self->state.getHost());
        },
//        assign new atom and ask if it's a master connection
        [=](get_new_id_atom, int32_t newID) {
            self->state.setId(newID);
            self->send(caf::actor_cast<caf::actor>(self->current_sender()), is_master_atom_v);
//            saves the id of the truck in front
        },[=](set_front_id, int32_t newID) {
            self->state.setFrontId(newID);
        },
//        sends back to whoever sent this message that he's is the master (THIS IS JUST TO BE SURE EVERYTHING WORKS)
        [=](you_are_master_atom) {
            self->send(caf::actor_cast<caf::actor>(self->current_sender()), you_are_master_atom_v);
        },
//        cascade back im the master now
        [=](tell_back_im_master_atom) {
            self->send(caf::actor_cast<caf::actor>(self->state.server), tell_back_im_master_atom_v);
//           saves that im connected to the master
        },[=](set_master_connection_atom, bool val) {
            self->state.setMasterConnection(val);
//            std::cout<< val << "New conn";
        },
//        updates the port and host (AS A C++ STRING)
        [=](update_port_host_atom, uint16_t newPort, std::string newHost) {
            self->state.setPort(newPort);
            self->state.setHost(newHost);
        },
//        receives and interpretes a new command
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
//        become the master
        [=](become_master_atom){
            std::cout << "[TRUCK]:I am the new master\n";
//            self->send_exit(caf::actor_cast<caf::actor>(self->state.server), caf::exit_reason::remote_link_unreachable);
            
            
            self->become(master(self));
            self->state.setId(MAX_TRUCKS);
            self->send(caf::actor_cast<caf::actor>(self->state.server), become_master_atom_v);
            self->send(self, tell_back_im_master_atom_v);
//            let's wait a bit, we dont want to overflow the buffer
            self->delayed_send(self,std::chrono::seconds(1) ,update_port_host_atom_v);
            
//            sets the new server
        },[=](set_server_atom){
            self->state.server = self->current_sender();
            self->attach_functor([=](const caf::error& reason) {
                std:: cout << "Server down";
                self->send_exit(caf::actor_cast<caf::actor>(self->state.server),caf::exit_reason::remote_link_unreachable);
            });
//        sends to the server to update the id behind
        },[=](update_id_behind_atom) {
            self->anon_send(caf::actor_cast<caf::actor>(self->state.server), update_id_behind_atom_v,self->state.getId()-1);
        },
//        sends back that he's the master now
        [=](tell_back_im_master_atom) {
            self->send(caf::actor_cast<caf::actor>(self->state.server), tell_back_im_master_atom_v);
//            Prototyping
        },[=](update_port_host_atom) {
//            self->send(caf::actor_cast<caf::actor>(self->state.server), update_port_host_atom_v);
            
//            updates master, connects to the new one basically
        },[=](update_master_atom, std::string host, uint16_t port) {
            auto impl = self->home_system().middleman().spawn_client(TruckClient, host, port, caf::actor_cast<caf::actor>(self->address()));
//        increments the numbers of trucks and tells back and fron to do it. DONT USE THIS FUNCTION
        },[=](increment_number_trucks_atom, uint32_t newPlatoon){
            self->state.tqPlatoon += newPlatoon;
            self->anon_send(caf::actor_cast<caf::actor>(self->state.client), increment_number_trucks_upwards_atom_v, uint32_t(self->state.tqPlatoon));
            std::cout << "PLATOON: "<<self->state.tqPlatoon<<"\n";
            self->delayed_send(caf::actor_cast<caf::actor>(self->state.server),std::chrono::milliseconds(50), increment_number_trucks_backwards_atom_v,uint32_t(self->state.tqPlatoon));
//            USE THIS ONE.
        },[=](increment_number_trucks_atom){
            self->state.tqPlatoon+= 1;
            self->anon_send(caf::actor_cast<caf::actor>(self->state.client), update_truck_numbers_atom_v,self->state.tqPlatoon);
            self->delayed_anon_send(caf::actor_cast<caf::actor>(self->state.server),std::chrono::seconds(1), update_truck_numbers_atom_v, self->state.tqPlatoon);
            std::cout << "PLATOON HERE: "<<self->state.tqPlatoon<<"\n";
//            DECREASES number of trucks
        },[=](decrease_number_trucks_atom){
            self->state.tqPlatoon-=2;
            std::cout << "PLATOON HERE: "<<self->state.tqPlatoon<<"\n";
//            Sets a new value for the number of trucks
        },[=](update_truck_numbers_atom, uint32_t a ) {
            self->state.tqPlatoon = a;
            std::cout << "PLATOON : " << self->state.tqPlatoon;
        },
        
//        cascade new port and host.
        [=](cascade_port_host_atom, uint16_t newPort, std::string newHost, truck_quantity stopID) {
            if (stopID == self->state.getId()) {
//                place holder
                std::cout << "Hey, that's it\n";
            }else{
                self->anon_send(caf::actor_cast<caf::actor>(self->state.server), cascade_port_host_atom_v, newPort, newHost, stopID);
            }
        },
    };
}



caf::behavior master(caf::stateful_actor<Truck>* self){
    return {
        [=](become_master_atom){
            std::cout << "[TRUCK]:I am the new master\n";
            self->anon_send(caf::actor_cast<caf::actor>(self->current_sender()), you_are_master_atom_v);
        },[=](set_server_atom) {
            self->state.server = self->current_sender();
            self->attach_functor([=](const caf::error& reason) {
                std:: cout << "Server down";
                self->send_exit(caf::actor_cast<caf::actor>(self->state.server),caf::exit_reason::remote_link_unreachable);
            });
//            std::cout<< val << "New conn";
        },[=](assign_id_atom){
            return self->state.getId() - 1;
        },[=](get_host_port_atom) {
            return std::make_pair(int32_t(self->state.getPort()),self->state.getHost());
        },[=](update_id_behind_atom) {
            self->anon_send(caf::actor_cast<caf::actor>(self->state.server), update_id_behind_atom_v,uint32_t{self->state.getId()-uint32_t(1)});
        },[=](tell_back_im_master_atom) {
            self->send(caf::actor_cast<caf::actor>(self->state.server), tell_back_im_master_atom_v);
        },[=](update_port_host_atom) {
            self->send(caf::actor_cast<caf::actor>(self->state.server),update_truck_behind_port_host_atom_v, self->state.getPort(), self->state.getHost());
        },[&](increment_number_trucks_atom, truck_quantity platoon) {
            self->state.tqPlatoon = platoon;
        },[&](increment_number_trucks_atom) {
            self->state.tqPlatoon = self->state.tqPlatoon+1;
        },[=](get_truck_numbers_atom) {
            return self->state.tqPlatoon;
        },[=](increment_number_trucks_atom, uint32_t newPlatoon){
            self->state.tqPlatoon += newPlatoon;
            std::cout << "PLATOON: "<<self->state.tqPlatoon<<"\n";
        },[=](update_truck_numbers_atom, uint32_t a ) {
            self->state.tqPlatoon = a;
            std::cout << "PLATOON : " << self->state.tqPlatoon;
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