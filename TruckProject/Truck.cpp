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
    
    return{
        /*
                Initialize the truck with a name and we also save the reference to the client.
        */
        [=](initialize_atom,std::string name) {
            self->state.setName(name);
            std::cout<<self->state.getName() + " has been spawned \n";
            self->state.client = self->current_sender();
            
//            self->anon_send(caf::actor_cast<caf::actor>(self->current_sender()), initialize_atom_v);
        },
        
        /*
                Initialize the truck with a name and we also save the reference to the client.
                In this version, a server for the truck behind is spawned
        */
        [=](initialize_atom,std::string name, uint16_t port) {
            self->state.setName(name);
            std::cout<<self->state.getName() + " has been spawned \n";
            self->state.client = self->current_sender();
//            spawn server for communication
            auto server = self->home_system().middleman().spawn_server(temp_server, port, caf::actor_cast<caf::actor>(self));
           
            self->state.setPort(port);
     
            self->attach_functor([=](const caf::error& reason) {
                std:: cout << "[TRUCK]: There is an error. REASON = "<< to_string(reason);
                self->send_exit(caf::actor_cast<caf::actor>(self->state.server),caf::exit_reason::remote_link_unreachable);
            });
//            save reference to the server
            send_as(*server, self, set_server_atom_v);
//            self->anon_send(caf::actor_cast<caf::actor>(self->current_sender()), initialize_atom_v);
        },
        
        /*
         Returns number of trucks in the platton
        */
        
        [=](get_truck_numbers_atom) {
            return self->state.tqPlatoon;
        },
        
        /*
         check wether the connection is to a master
        */
        
        [=](is_master_atom) {
            return self->state.isMasterConnection();
            
        },
        /*
         return the port
        */
        
        [=](get_port_atom) {
            return self->state.getPort();
        },
        
        /*
         return the ip address of the host AS A C STRING
        */
        
        [=](get_host_atom) {
            return self->state.getHostC();
        },
        
        /*
         returns truck id
        */
        
        [=](which_id_atom) {
            return self->state.getId();
        },
        
        /*
         returns front truck id
        */
        
        [=](which_front_id_atom) {
            return self->state.getFrontId();
        },
        
        /*
         return the port and the host AS A C++ STRING
        */
        
        [=](get_host_port_atom) {
            return std::make_pair(int32_t(self->state.getPort()),self->state.getHost());
        },
        /*
         return the port and the host PREVIOUS TRUCK  AS A C++ STRING
        */
        [=](get_port_host_previous_atom) {
            return std::make_pair(int32_t(self->state.getPreviousPort()),self->state.getPreviousHost());
        },
        
        /*
         return the port and the host PREVIOUS TRUCK  AS A C++ STRING
        */
        [=](get_port_host_back_up_atom) {
            return std::make_pair(int32_t(self->state.getBackUpPort()),self->state.getBackUpHost());
        },
        /*
         assign new atom and ask if it's a master connection
        */
        
        [=](get_new_id_atom, int32_t newID) {
            self->state.setId(newID);
            self->send(caf::actor_cast<caf::actor>(self->current_sender()), is_master_atom_v);
        },
        
        /*
         saves the id of the truck in front
        */
        
        [=](set_front_id, int32_t newID) {
            self->state.setFrontId(newID);
        },
        
        /*
         sends back to whoever sent this message that he's is the master (THIS IS JUST TO BE SURE EVERYTHING WORKS)
        */
        
        [=](you_are_master_atom) {
            self->send(caf::actor_cast<caf::actor>(self->current_sender()), you_are_master_atom_v);
        },
        
        /*
         cascade back im the master now
        */
        
        
        [=](tell_back_im_master_atom) {
            self->send(caf::actor_cast<caf::actor>(self->state.server), tell_back_im_master_atom_v);
           
        },
        
        /*
         saves that im connected to the master
        */
        
        [=](set_master_connection_atom, bool val) {
            self->state.setMasterConnection(val);
//            std::cout<< val << "New conn";
        },
        
        /*
                Iupdates the port and host (AS A C++ STRING)
        */
        
        [=](update_port_host_atom, uint16_t newPort, std::string newHost) {
            self->state.setPort(newPort);
            self->state.setHost(newHost);
    
        },
        
        /*
         receives and interpretes a new command
        */
        
        
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
        
        /*
                Routine to become the new master
        */
        
        [=](become_master_atom){
            std::cout << "[TRUCK]: I am the new master\n";
            self->become(master(self));
            self->state.setId(MAX_TRUCKS);
//            self->send(self, update_id_behind_atom_v);
            self->send(caf::actor_cast<caf::actor>(self->state.server), become_master_atom_v);
            self->send(self, tell_back_im_master_atom_v);
//            let's wait a bit, we dont want to overflow the buffer
            self->delayed_send(self,std::chrono::seconds(1) ,update_port_host_atom_v);
        },
        
        /*
         sets the new server
        */
        
        [=](set_server_atom){
            self->state.server = self->current_sender();
            self->attach_functor([=](const caf::error& reason) {
                std:: cout << "Server down";
                self->send_exit(caf::actor_cast<caf::actor>(self->state.server),caf::exit_reason::remote_link_unreachable);
            });
        
        },
        
        /*
         sends to the server to update the id behind
        */
        
        [=](update_id_behind_atom) {
            self->anon_send(caf::actor_cast<caf::actor>(self->state.server), update_id_behind_atom_v,self->state.getId()-1);
        },
        
        /*
         sends back that he's the master now
        */
        
        [=](tell_back_im_master_atom) {
            self->send(caf::actor_cast<caf::actor>(self->state.server), tell_back_im_master_atom_v);
        },
        
        /*
         Spawn the server required to communicate with the truck behind
        */
        
        [=](spawn_server_atom, uint16_t port) {
            auto server = self->home_system().middleman().spawn_server(temp_server, port, caf::actor_cast<caf::actor>(self));
            self->attach_functor([=](const caf::error& reason) {
                std:: cout << "[TRUCK]: There is an error. REASON = "<< to_string(reason);
                self->send_exit(caf::actor_cast<caf::actor>(self->state.server),caf::exit_reason::remote_link_unreachable);
            });
        },
        
        /*
         updates master, connects to the new one basically
        */
        
        [=](update_master_atom, std::string host, uint16_t port) {
            auto impl = self->home_system().middleman().spawn_client(TruckClient, host, port, caf::actor_cast<caf::actor>(self->address()));
        
        },

        
        /*
         USE THIS ONE.
        */
        
        [=](increment_number_trucks_atom){
            self->state.tqPlatoon+= 1;
            self->anon_send(caf::actor_cast<caf::actor>(self->state.client), update_truck_numbers_atom_v,self->state.tqPlatoon);
            self->delayed_anon_send(caf::actor_cast<caf::actor>(self->state.server),std::chrono::milliseconds(10), update_truck_numbers_atom_v, self->state.tqPlatoon);
            std::cout << "PLATOON HERE: "<<self->state.tqPlatoon<<"\n";
        },
        
        /*
         DECREASES number of trucks
        */
        
        [=](decrease_number_trucks_atom){
            self->state.tqPlatoon-=2;
            std::cout << "[TRUCK]: Platoon after decreasing is : "<<self->state.tqPlatoon<<"\n";
            
        },
        /*
         DECREASES number of trucks by a certain amount
        */
        
        [=](decrease_number_trucks_atom, truck_quantity quantity){
            self->state.tqPlatoon-=quantity;
            std::cout << "[TRUCK]: Platoon after decreasing by a certain amount is : "<<self->state.tqPlatoon<<"\n";
            
        },
        
        /*
         Sets a new value for the number of trucks
        */
        
        [=](update_truck_numbers_atom, uint32_t a ) {
            self->state.tqPlatoon = a;
            std::cout << "[TRUCK]: Platoon is now : " << self->state.tqPlatoon;
        },
        
        /*
         cascade new port and host.
        */
        
        [=](cascade_port_host_atom, uint16_t newPort, std::string newHost, truck_quantity stopID) {
            if (stopID == self->state.getId()) {
//                place holder
                std::cout << "Hey, that's it\n";
            }else{
                
                self->anon_send(caf::actor_cast<caf::actor>(self->state.server), cascade_port_host_atom_v, newPort, newHost, stopID);
            }
        },
        /*
         Update the port and the host for the truck in front
        */
        
        [=](update_port_host_previous_atom, uint16_t port, std::string host){
            self->state.setPreviousHost(host);
            self->state.setPreviousPort(port);
//            std::cout << "PREVIOUS TRUCK:" << self->state.getPreviousPort() << " " << self->state.getPreviousHost() << std::endl ;
        },

        /*
         Update the port and the host for the truck in front of the truck in front
        */

        [=](update_back_up_atom, uint16_t port, std::string host){
            self->state.setBackUpHost(host);
            self->state.setBackUpPort(port);
            
//            std::cout << "BACKUP TRUCK:" << self->state.getBackUpHost() << " " << self->state.getBackUpPort() << std::endl ;
        },
        
        /*
         truck in front left or dead
        */
        
        [=](truck_left_or_dead_atom) {
            auto im = self->home_system().middleman().spawn_client(TruckClient, self->state.getBackUpHost(), self->state.getBackUpPort(), self);
            if(!im)
                std::cerr << "failed to spawn "<< self->state.getName() << "'s client: " << to_string(im.error()) <<"\n"<< "\n\n"<< std::endl;
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
            std::cout << "[TRUCK]: Platoon is now  : "<<self->state.tqPlatoon<<"\n";
        },[=](update_truck_numbers_atom, uint32_t a ) {
            self->state.tqPlatoon = a;
            std::cout << "[TRUCK]: Platoon is now  : " << self->state.tqPlatoon << "\n";
        },
        [=](get_port_atom) {
            return self->state.getPort();
        },
        
        [=](decrease_number_trucks_atom, truck_quantity quantity){
            self->state.tqPlatoon-=quantity;
            std::cout << "[TRUCK]: Platoon after decreasing by a certain amount is : "<<self->state.tqPlatoon<<"\n";
            
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
std::string Truck::getPreviousHost(){
    return sPreviousTruckHost_;
}
uint16_t Truck::getPreviousPort(){
    return uPreviousTruckPort;
}
std::string Truck::getBackUpHost(){
    return sBackUpHost_;
}
uint16_t Truck::getBackUpPort(){
    return uBackUpPort;
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

void Truck::setPreviousHost(std::string host){
    sPreviousTruckHost_ = host;
}


void Truck::setPreviousPort(uint16_t port){
    uPreviousTruckPort = port;
}
void Truck::setBackUpHost(std::string host){
    sBackUpHost_ = host;
}
void Truck::setBackUpPort(uint16_t port){
    uBackUpPort = port;
}

