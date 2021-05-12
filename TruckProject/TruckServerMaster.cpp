//
//  TruckServer.cpp
//  Truck2
//
//  Created by Luca on 02/05/21.
//


#include "Truck.hpp"

/*------------------------------------------------------------------------------------------------
//Create a different brokers to handle more connection
 This proker needs to be special, it only sends the commands coming from the main broker
 The messages received will be like handled like the main broker.
//Save them into a vector
//After that create a behavior of the main broker that sends the required messages to the  other brokers
---------------------------------------------------------------------------------------------------*/

/*
 YOu can handle the connection switch by declaring the handle invalid.
 And only in that case, you quit
 */

caf::behavior TruckServerMaster(caf::io::broker *self, caf::io::connection_handle hdl, const caf::actor& buddy){
    //    Should be more than one,
    //    Change later
//        assert(self->num_connections() == 1);
        self->monitor(buddy);
        self->set_down_handler([=](caf::down_msg& dm) {
            if (dm.source == buddy) {
                std::cout << "[MASTER]:My Mate is down" << std::endl;
                self->quit(dm.reason);
            }
        });
    
    std::cout << "[MASTER]: I have been spawned.Connections: "<<self->num_connections() << std::endl;
    auto a = self->add_tcp_doorman(3232);
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
    self->delayed_send(self, std::chrono::milliseconds(20), update_port_host_previous_atom_v);
        return {
            [=](ask_for_input_atom){
                auto input = self->home_system().spawn(InputMonitor);
                self->send(input, 1);
//                poll for input every 4 seconds
                
                self->delayed_send(self, std::chrono::seconds(10), ask_for_input_atom_v);
            },
        
            [=](initialiaze_truck_platoon_atom){
                self->send(buddy, increment_number_trucks_atom_v, uint32_t(1));
                self->request(buddy, std::chrono::seconds(1), get_truck_numbers_atom_v).then([=](truck_quantity a ){
                    write_int(self, hdl, static_cast<uint8_t>(operations::update_number_trucks));
                    write_int(self, hdl, uint32_t(a));
                    self->flush(hdl);
                });
                
            },
            
        
            [=](update_port_host_previous_atom){
                std::string Host = "ciao";
                char temp[20] = {'\0'};
                uint32_t message = 0;
                self->request(buddy, std::chrono::seconds(1), get_host_port_atom_v).then([&](std::pair<int32_t, std::string> pPortHost) mutable {
                    Host = pPortHost.second;
                    message = pPortHost.first;
                    //std::cout << Host << message;
                    uint16_t length = Host.length();
                    while(Host.length()<15) Host.append("-");
                    Host.append("064");
                    length = Host.length();
                    message |= length<<16;
                    std::strcpy(temp, Host.c_str());
                    write_int(self, hdl, static_cast<uint8_t>(operations::update_port_host_previous));
                    write_int(self, hdl, message);
                    self->write(hdl, sizeof(char)*(length), temp);
                    self->flush(hdl);
                });
                
            },
            
            [=](you_are_master_atom) {
//                write_int(self, hdl, static_cast<uint8_t>(operations::ready));
//                write_int(self, hdl, 1);
//                self->flush(hdl);
            },

            [=](tell_back_im_master_atom){
                std::cout<<"Telle back\n";
                write_int(self, hdl, static_cast<uint8_t>(operations::master));
                write_int(self, hdl, uint32_t(1));
                self->flush(hdl);
            },
        
            [=](const caf::io::connection_closed_msg& msg) {
              if (msg.handle == hdl) {
                std::cout << "[MASTER]: Connection closed" << std::endl;
              }
//                self->close(hdl_p);
//                self->close(hdl);
                self->send(buddy, decrease_number_trucks_atom_v, truck_quantity(1));
                
                auto a = self->add_tcp_doorman(3232);
//                std::cout << self->num_doormen();
                
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
                        write_int(self, hdl, static_cast<uint8_t>(operations::update_port_host_previous));
                        write_int(self, hdl, message);
                        self->write(hdl, sizeof(char)*(length), temp);
                        break;
                    default:
                        
                        break;
                }
                self->flush(hdl);
                
            },
            
            [=](const caf::io::new_data_msg& msg) mutable {
//                hdl = msg.handle;
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

                        break;
                    
                  default:
                        std::cout << "[MASTER]:invalid value for op_val, stop" << std::endl;
//                        self->quit(caf::sec::invalid_argument);
                        break;
                };
            },[=, &hdl](const caf::io::new_connection_msg& msg) {
                std::cout << "[MASTER]: New Connection_Accepted" << std::endl;
                
                if (self->num_connections() <=1 ) {
                    self->fork(TruckServerMaster, msg.handle, std::move(buddy));
                    std::cout << "[MASTER]: Im gonna die and fork to a new broker. Connections: "<<self->num_connections() << std::endl;
                    self->quit(caf::sec::feature_disabled);
                }
                
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
    },[=](tell_back_im_master_atom){
//        std::cout<<"NO ONE TO SEND COMMANDS TO\n";
    
    },[=](update_truck_behind_port_host_atom, uint16_t p, std::string s){
//        std::cout<<"NO ONE TO SEND COMMANDS TO\n";
    },
      
      [=](ask_for_input_atom){
    },
      
      [=](fork_to_master_atom, caf::io::connection_handle hdl){
          std::cout << "DO I work";
        auto impl = self->fork(TruckServerMaster,hdl, std::move(buddy));
  //        self->send(buddy, increment_number_trucks_atom_v);
        self->quit();
    },
  };
}
