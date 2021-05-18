//
//  TruckBehind.cpp
//  Truck2
//
//  Created by Luca on 02/05/21.
//
#include "Truck.hpp"

caf::behavior TruckBehind(caf::io::broker *self, caf::io::connection_handle hdl,const caf::actor& buddy){
    ///monitors the buddy and when it's down, we quit
    self->monitor(buddy);
    self->set_down_handler([=](caf::down_msg& dm) {
        if (dm.source == buddy) {
            std::cout << "My Mate is down" << std::endl;
            self->quit(dm.reason);
        }
    });

    
    ///Sends to the buddy to save the pointer to the server
    self->send(buddy, set_server_atom_v);
    ///Increments the platoon size
    self->send(buddy, increment_number_trucks_atom_v);
    ///defines how much we want to read from the buffer
    self->configure_read(hdl, caf::io::receive_policy::at_least(sizeof(uint8_t)+sizeof(uint32_t)));
    return{
        ///This is gonna get changed
        [=](const caf::io::connection_closed_msg& msg) {
          if (msg.handle == hdl) {
            std::cout << "[SERVER]: Connection closed" << std::endl;
          }
            ///This is gonna get changed
        },[=](const caf::io::new_connection_msg& msg) {
            std::cout << "[SERVER]: New Connection_Accepted" << std::endl;
            ///Updates the size of the platoon of the truck behind
      },[=](update_truck_numbers_atom, truck_quantity q) {
          write_int(self, hdl, static_cast<uint8_t>(operations::update_number_trucks_from_client));
          write_int(self, hdl,static_cast<uint32_t>(q));
          self->flush(hdl);
          ///Cascades the command to the truck behind
      },[=](send_new_command_atom, uint32_t command){
          write_int(self, hdl, static_cast<uint8_t>(operations::command));
          write_int(self, hdl, command);
          self->flush(hdl);
          ///Allows this broker to fork into a master situation
      },[=](become_master_atom){
          self->fork(TruckServerMaster,std::move(hdl), std::move(buddy));
      },
//      },[=](update_truck_behind_port_host_atom, uint16_t p, std::string s){
//
//      },
        
        
        ///Handler for a new message.
        ///Switch cases decides what to do
        
          [=](const caf::io::new_data_msg& msg) {
          auto rd_pos = msg.buf.data();
          auto op_val = uint8_t{0};
          read_int(rd_pos, op_val);
          ++rd_pos;
          auto val = uint32_t{0};
          read_int(rd_pos, val);
          ++rd_pos;
              
              
          switch (static_cast<operations>(op_val)) {
                  
                  
              case operations::assign_id:
                  self->request(buddy, std::chrono::seconds(1), which_id_atom_v).await([=](int32_t Id){
                      write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
                      write_int(self, hdl, Id-1);
//                      write_int(self, hdl, MAX_TRUCKS - Id-1);
                      self->flush(hdl);
                  });
                  break;
                  
              case operations::master:
                    write_int(self, hdl, static_cast<uint8_t>(operations::master));
                    write_int(self, hdl, int32_t(0));
                    self->flush(hdl);
                  break;
                  
              case operations::initialiaze_truck_platoon:
                    write_int(self, hdl, static_cast<uint8_t>(operations::master));
                    write_int(self, hdl, int32_t(0));
                    self->flush(hdl);
                  break;
            default:
                  std::cout << "invalid No for op_val, stop" << std::endl;
                  break;
          };
        },
        [=](send_server_atom){
          self->request(buddy, std::chrono::seconds(1), which_id_atom_v).await([=](int32_t Id){
              write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
              write_int(self, hdl, Id-1);
              self->flush(hdl);
          });
            
      },
        [=](update_port_host_previous_atom){
          std::string Host = "localhost";
          char temp[20] = {'\0'};
          uint32_t message = 0;
          self->request(buddy, std::chrono::seconds(1), get_host_port_atom_v).then([&](std::pair<int32_t, std::string> pPortHost) mutable {
              Host = pPortHost.second;
              message = pPortHost.first;
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
    
        [=](update_back_up_atom){
            std::string Host = "localhost";
            char temp[20] = {'\0'};
            uint32_t message = 4242;
            self->request(buddy, std::chrono::seconds(1),get_port_host_previous_atom_v ).then([&](std::pair<int32_t, std::string> pPortHost) mutable {
                Host = pPortHost.second;
                message = pPortHost.first;
                uint16_t length = Host.length();
                while(Host.length()<15) Host.append("-");
                Host.append("064");
                length = Host.length();
                message |= length<<16;
                std::strcpy(temp, Host.c_str());
                write_int(self, hdl, static_cast<uint8_t>(operations::update_port_host_back_up));
                write_int(self, hdl, message);
                self->write(hdl, sizeof(char)*(length), temp);
                self->flush(hdl);
                    
            });
        },
    };
};



//Temporary server to allow the truck behind to connect to this truck
//This actor will die as soon as a connection is created.
caf::behavior temp_server(caf::io::broker *self,const caf::actor& buddy){
    std::cout << "[SERVER]: SPAWNED" << std::endl;
    return{
    
        ///Handler a new connection
        ///If the platoon size is less than a certain size, all good
        ///otherwise we close the connection
        [=](const caf::io::new_connection_msg& msg) {
            truck_quantity number_trucks=0;
            self->request(buddy, std::chrono::seconds(2), get_truck_numbers_atom_v).await(
                [&](truck_quantity tqNumberTrucks) mutable {
                    number_trucks = tqNumberTrucks;
            });
        
            if(number_trucks<=MAX_TRUCKS){
                std::cout << "[SERVER]: New Connection_Accepted" << std::endl;
                auto impl = self->fork(TruckBehind, msg.handle,buddy);
                self->send(impl, send_server_atom_v);
                self->delayed_send(impl, std::chrono::milliseconds(10),update_port_host_previous_atom_v);
                self->delayed_send(impl, std::chrono::milliseconds(20), update_back_up_atom_v);
                self->quit();
            }else{
                std::cout << "[SERVER]: Connection_refused : too many trucks" << std::endl;
                self->close(msg.handle);
            }
        },
    
        ///Those are insanely useful. They save the program from  exploding
        ///These basically handles some messages that happen when this truck becomes the master.
        /// Without those the program will die. Dont erase them
        
        [=](send_new_command_atom, uint32_t command){
        },[=](become_master_atom){
            self->become(temp_master_server(self, buddy));
        },
        
        [=](tell_back_im_master_atom){
        },
        [=](update_truck_behind_port_host_atom, uint16_t p, std::string s){
        },
    };
}


