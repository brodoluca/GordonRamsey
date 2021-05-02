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
    
        [=](get_new_id_atom, int32_t newID) {
            self->state.setId(newID);
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
            self->state.setId(1);
            self->state.mPlatoon.insert(std::pair<int32_t,caf::strong_actor_ptr>(self->state.getId(), caf::actor_cast<caf::strong_actor_ptr>(self)));
            aout(self)<<"["+self->state.getName()+"]" + " has a new ID:"+ std::to_string(self->state.getId()) + "\n";
        },[=](set_server_atom){
            self->state.server = self->current_sender();
        }
    };
    
}
caf::behavior master(caf::stateful_actor<Truck>* self){;
    return {
        [=](assign_id_atom){
            return 2;
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
float Truck::getSpeed(){
    return fSpeed;
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
void Truck::setSpeed(float speed){
    fSpeed = speed;
};




