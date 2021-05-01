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
                    break;
                case commands::start:
                    self->state.setSpeed(100);
                    std::cout << self->state.getName()+" starts: " + std::to_string(self->state.getSpeed()) +" \n";
                    break;  
                case commands::accellerate:
                    self->state.setSpeed(self->state.getSpeed()+10);
                    std::cout << self->state.getName()+" accellerates: " + std::to_string(self->state.getSpeed()) +" \n";
                    break;
                case commands::decellerate:
                    self->state.setSpeed(self->state.getSpeed()-10);
                    std::cout << self->state.getName()+" decellerates: " + std::to_string(self->state.getSpeed()) +" \n" ;
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


caf::behavior TruckClient(caf::io::broker *self, caf::io::connection_handle hdl, const caf::actor& buddy){
//    Should be more than one,
//    Change later
    assert(self->num_connections() == 1);
    self->monitor(buddy);
    self->set_down_handler([=](caf::down_msg& dm) {
        if (dm.source == buddy) {
            aout(self) << "My Mate is down" << std::endl;
            // Temporarely quit for this reason
            self->quit(dm.reason);
        }
    });
    
//    configure to exactly receive this much data
    self->configure_read(hdl, caf::io::receive_policy::at_most(sizeof(uint8_t)+sizeof(uint32_t)));
    return {
        [=](const caf::io::connection_closed_msg& msg) {
          if (msg.handle == hdl) {
            aout(self) << "connection closed" << std::endl;
            // force buddy to quit
              self->send_exit(buddy, caf::exit_reason::remote_link_unreachable);
              self->quit(caf::exit_reason::remote_link_unreachable);
          }
        },
        [=](initialize_atom) {
            self->send(self, send_server_atom_v, 2);
        
        },[=](send_server_atom, int32_t i) {
            aout(self) << "[CLIENT]: Send Server: " << i << "" << std::endl;
            write_int(self, hdl, static_cast<uint8_t>(operations::assign_id));
            write_int(self, hdl, i);
            self->flush(hdl);
          },
        
        [=](const caf::io::new_data_msg& msg) {
            // Keeps track of our position in the buffer.
            auto rd_pos = msg.buf.data();
            // Read the operation value as uint8_t from the buffer.
            auto op_val = uint8_t{0};
            read_int(rd_pos, op_val);
            ++rd_pos;
            auto val = uint32_t{0};
            read_int(rd_pos, val);
            // Show some output.

            // Send composed message to our buddy.
            switch (static_cast<operations>(op_val)) {
                case operations::assign_id:
                aout(self) << "received response "<< val << std::endl;
                break;
                case operations::get_id:
                aout(self) << "[CLIENT]: Received new ID: "<<val << std::endl;
                    self->send(buddy, get_new_id_atom_v, int32_t(val));
                break;
                case operations::command:
                    aout(self) << "[CLIENT]: Received new command" << std::endl;
                    self->send(buddy, get_new_command_v, int32_t(val));
                break;
              default:
                aout(self) << "invalid value for op_val, stop" << std::endl;
                    self->quit(caf::sec::invalid_argument);
            };

        }

    };
}



caf::behavior TruckServer(caf::io::broker *self, caf::io::connection_handle hdl, const caf::actor& buddy){
    //    Should be more than one,
    //    Change later
        assert(self->num_connections() == 1);
        self->monitor(buddy);
        self->set_down_handler([=](caf::down_msg& dm) {
            if (dm.source == buddy) {
                aout(self) << "My Mate is down" << std::endl;
                self->quit(dm.reason);
            }
        });
    self->configure_read(hdl, caf::io::receive_policy::exactly(sizeof(uint8_t)+sizeof(uint32_t)));
//    Become master
    self->send(buddy, become_master_atom_v);
        return {
            [=](const caf::io::connection_closed_msg& msg) {
              if (msg.handle == hdl) {
                aout(self) << "[SERVER]: Connection closed" << std::endl;
                  self->send_exit(buddy, caf::exit_reason::remote_link_unreachable);
                  self->quit(caf::exit_reason::remote_link_unreachable);
                  
              }
            },
            [=](uint32_t a) {
//                YOU CAN USE THIS PART TO SEND COMMANDS TO THE TRUCK
                write_int(self, hdl, static_cast<uint8_t>(operations::command));
                
                switch (a) {
                    case 1:
                        write_int(self, hdl, static_cast<uint32_t>(commands::stop));
                    break;
                    case 2:
                        write_int(self, hdl, static_cast<uint32_t>(commands::accellerate));
                        break;
                    case 3:
                        write_int(self, hdl, static_cast<uint32_t>(commands::decellerate));
                        break;
                    case 4:
                        write_int(self, hdl, static_cast<uint32_t>(commands::start));
                        break;
                
                    default:
                        break;
                }
                self->flush(hdl);
                
            },
            [=](const caf::io::new_data_msg& msg) {
                auto rd_pos = msg.buf.data();
                auto op_val = uint8_t{0};
                read_int(rd_pos, op_val);
                ++rd_pos;
                auto val = uint32_t{0};
                read_int(rd_pos, val);
                auto input = self->home_system().spawn(InputMonitor);
                self->send(input, 1);
                switch (static_cast<operations>(op_val)) {
                    case operations::assign_id:
                        self->request(buddy, std::chrono::seconds(2), assign_id_atom_v).then([=](int32_t newId){
                            write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
                            write_int(self, hdl, newId);
                            self->flush(hdl);
                        });
                        break;
                  default:
                        aout(self) << "invalid value for op_val, stop" << std::endl;
                        self->quit(caf::sec::invalid_argument);
                };
                self->send_exit(input, caf::exit_reason::remote_link_unreachable);
            },
            
            [=](const caf::io::new_connection_msg& msg) {
                aout(self) << "[SERVER]: New Connection_Accepted" << std::endl;
                self->request(buddy, std::chrono::seconds(2), assign_id_atom_v).then([=](int32_t newId){
                    write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
                    write_int(self, hdl, newId);
                    self->flush(hdl);
                });
            
            },
        
        };
}
