//
//  TruckServer.cpp
//  Truck2
//
//  Created by Luca on 02/05/21.
//
#include "Truck.hpp"
caf::behavior TruckServerMaster(caf::io::broker *self, caf::io::connection_handle hdl, const caf::actor& buddy){
    //    Should be more than one,
    //    Change later
        assert(self->num_connections() == 1);
        self->monitor(buddy);
        self->set_down_handler([=](caf::down_msg& dm) {
            if (dm.source == buddy) {
                std::cout << "[MASTER]:My Mate is down" << std::endl;
                self->quit(dm.reason);
                
            }
        });
    self->delayed_send(self, std::chrono::seconds(2), ask_for_input_atom_v);
    self->configure_read(hdl, caf::io::receive_policy::exactly(sizeof(uint8_t)+sizeof(uint32_t)));
//    Become master
    
    self->send(buddy, become_master_atom_v);
//    self->send(buddy, set_server_atom_v);
    self->request(buddy, std::chrono::seconds(2), assign_id_atom_v).then([=](int32_t newId){
            write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
            write_int(self, hdl, newId);
            self->flush(hdl);
        });
    
//    self->send(buddy, update_id_behind_atom_v);
    self->delayed_send(self, std::chrono::milliseconds(10), initialiaze_truck_platoon_atom_v);
    
        return {
            [=](ask_for_input_atom){
                auto input = self->home_system().spawn(InputMonitor);
                self->send(input, 1);
//                poll for input every 4 seconds
                self->delayed_send(self, std::chrono::seconds(4), ask_for_input_atom_v);
            
            },[=](initialiaze_truck_platoon_atom){
                self->send(buddy, increment_number_trucks_atom_v, uint32_t(1));
                self->request(buddy, std::chrono::seconds(1), get_truck_numbers_atom_v).then([=](truck_quantity a ){
                    write_int(self, hdl, static_cast<uint8_t>(operations::update_number_trucks));
                    write_int(self, hdl, uint32_t(a));
                    self->flush(hdl);
                });
            },[=](you_are_master_atom) {
                std::cout << "I am the new master now\n";
//                write_int(self, hdl, static_cast<uint8_t>(operations::ready));
//                write_int(self, hdl, 1);
//                self->flush(hdl);
            },
            [=](const caf::io::connection_closed_msg& msg) {
              if (msg.handle == hdl) {
                std::cout << "[SERVER]: Connection closed" << std::endl;
//                  self->send_exit(buddy, caf::exit_reason::remote_link_unreachable);
//                  self->quit(caf::exit_reason::remote_link_unreachable);
              }
            },[=](update_id_behind_atom, uint32_t newId){
                std::cout<<"Upsate Id\n";
                write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
                write_int(self, hdl, newId);
                self->flush(hdl);
            },
            [=](uint32_t a) {
//                YOU CAN USE THIS PART TO SEND COMMANDS TO THE TRUCK
                std::string Host = "localhost";
                char temp[20] = {'\0'};
                uint16_t length = Host.length();
                uint32_t message = 4242;
               
            
                switch (a) {
                    case 1:
                        write_int(self, hdl, static_cast<uint8_t>(operations::command));
                        write_int(self, hdl, static_cast<uint32_t>(commands::stop));
                    break;
                    case 2:
                        write_int(self, hdl, static_cast<uint8_t>(operations::command));
                        write_int(self, hdl, static_cast<uint32_t>(commands::accellerate));
                        break;
                    case 3:
                        write_int(self, hdl, static_cast<uint8_t>(operations::command));
                        write_int(self, hdl, static_cast<uint32_t>(commands::decellerate));
                        break;
                    case 4:
                        write_int(self, hdl, static_cast<uint8_t>(operations::command));
                        write_int(self, hdl, static_cast<uint32_t>(commands::start));
                        break;
                    case 5:
                        while(Host.length()<15) Host.append("-");
                        Host.append("064");
                        length = Host.length();
                        message |= length<<16;
                        std::strcpy(temp, Host.c_str());
                        write_int(self, hdl, static_cast<uint8_t>(operations::cascade_port_host));
                        write_int(self, hdl, message);
                        self->write(hdl, sizeof(char)*(length), temp);
                
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
                switch (static_cast<operations>(op_val)) {
                    case operations::assign_id:
                        self->request(buddy, std::chrono::seconds(2), assign_id_atom_v).then([=](int32_t newId){
                            write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
                            write_int(self, hdl, newId);
                            self->flush(hdl);
                        });
//                        self->delayed_send(self,std::chrono::seconds(1),initialiaze_truck_platoon_atom_v);
                        break;
                    case operations::master:
                        write_int(self, hdl, static_cast<uint8_t>(operations::master));
                        write_int(self, hdl, int32_t(1));
                        self->flush(hdl);
                        break;
                    case operations::update_number_trucks:
//                        self->send(buddy, increment_number_trucks_atom_v,1);
                        break;
                    case operations::update_number_trucks_from_client:
                        self->send(buddy, update_truck_numbers_atom_v,val);
                        std::cout << "IM HERE: "<< val;
                        break;
                    
                  default:
                        std::cout << "[MASTER]:invalid value for op_val, stop" << std::endl;
                        self->quit(caf::sec::invalid_argument);
                };
               
            },[=](const caf::io::new_connection_msg& msg) {
                std::cout << "[MASTER]: New Connection_Accepted" << std::endl;
                self->request(buddy, std::chrono::seconds(2), assign_id_atom_v).then([=](int32_t newId){
                    write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
                    write_int(self, hdl, newId);
                    self->flush(hdl);
                });
        
            },
        
        };
}

caf::behavior temp_master_server(caf::io::broker* self, const caf::actor& buddy) {
  std::cout << "[TEMP_SERVER]: running" << std::endl;
//    self->send(buddy, become_master_atom_v);
  return {
    [=](const caf::io::new_connection_msg& msg) {
      std::cout << "[TEMP_SERVER]: New Connection_Accepted" << std::endl;
      auto impl = self->fork(TruckServerMaster, msg.handle, std::move(buddy));
//        self->send(buddy, increment_number_trucks_atom_v);
        
      self->quit();
    },
  };
}
