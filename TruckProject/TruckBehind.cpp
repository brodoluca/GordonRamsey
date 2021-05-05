//
//  TruckBehind.cpp
//  Truck2
//
//  Created by Luca on 02/05/21.
//
#include "Truck.hpp"
#include "string.h"
#include <cstdint>
#include <iomanip>
caf::behavior TruckBehind(caf::io::broker *self, caf::io::connection_handle hdl,const caf::actor& buddy){
    self->monitor(buddy);
    self->set_down_handler([=](caf::down_msg& dm) {
        if (dm.source == buddy) {
            std::cout << "My Mate is down" << std::endl;
//            self->quit(dm.reason);
        }
    });
    self->link_to(buddy);
    self->set_exit_handler([=](caf::exit_msg& ms){
        if (ms.source == buddy) {
            std::cout << "My Mate is dead" << std::endl;
//            self->quit(ms.reason);
        }
    });

    self->send(buddy, set_server_atom_v);
    self->configure_read(hdl, caf::io::receive_policy::at_least(sizeof(uint8_t)+sizeof(uint32_t)));
    return{
        [=](const caf::io::connection_closed_msg& msg) {
          if (msg.handle == hdl) {
            std::cout << "[SERVER]: Connection closed" << std::endl;
//              self->send_exit(buddy, caf::exit_reason::remote_link_unreachable);
//              self->quit(caf::exit_reason::remote_link_unreachable);
          }
        
        },[=](const caf::io::new_connection_msg& msg) {
            std::cout << "[SERVER]: New Connection_Accepted" << std::endl;
            write_int(self, hdl, static_cast<uint8_t>(operations::get_port_host));
            write_int(self, hdl, uint32_t{1});
        
      },[=](send_new_command_atom, uint32_t command){
          std::cout<<"Started\n";
          write_int(self, hdl, static_cast<uint8_t>(operations::command));
          write_int(self, hdl, command);
          self->flush(hdl);
      },[=](const caf::io::new_data_msg& msg) {
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
                      self->flush(hdl);
                
                  });
                  
                  break;
              case operations::update_truck_behind:
//                receive host
                  char cstr[17];
                  while (strlen(cstr) < 15) {
                      memcpy(&cstr, ++rd_pos,sizeof(char)*17);
                  }
                  break;
                  
              case operations::master:
                    write_int(self, hdl, static_cast<uint8_t>(operations::master));
                    write_int(self, hdl, int32_t(0));
                    self->flush(hdl);
                  
                  break;
            default:
                  std::cout << "invalid No for op_val, stop" << std::endl;
                  self->quit(caf::sec::invalid_argument);
          };
    
          
        
      },[=](send_server_atom){
          std::cout<<"Send Server\n";
          self->request(buddy, std::chrono::seconds(1), which_id_atom_v).await([=](int32_t Id){
              write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
              write_int(self, hdl, Id-1);
              self->flush(hdl);
        
          });
        
//          self->request(buddy, std::chrono::seconds(1), which_front_id_atom_v).await([=](int32_t Id){
//              write_int(self, hdl, static_cast<uint8_t>(operations::front_id));
//              write_int(self, hdl, Id);
//              self->flush(hdl);
//          });
      },[=](update_id_behind_atom, uint32_t newId){
          std::cout<<"Upsate Id\n";
          write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
          write_int(self, hdl, newId);
          self->flush(hdl);
      },[=](tell_back_im_master_atom){
          std::cout<<"Telle back\n";
          write_int(self, hdl, static_cast<uint8_t>(operations::master));
          write_int(self, hdl, uint32_t(1));
          self->flush(hdl);
//          self->request(buddy, std::chrono::seconds(2), get_host_port_atom_v).then(
//                  [=](std::pair<int32_t, std::string> pHostPort){
//                      char temp[17];
//                      std::strcpy(temp, pHostPort.second.c_str());
//                      for(unsigned long i=pHostPort.second.length();i<17;i++)
//                          temp[i] = '-';
//                      write_int(self, hdl, static_cast<uint8_t>(operations::update_truck_behind));
//                      write_int(self, hdl, pHostPort.first);
//                      self->write(hdl, sizeof(char)*17, temp);
//                      self->flush(hdl);
//        });
        
      },[=](update_truck_behind_port_host_atom, uint16_t port, std::string Host){
          std::cout<<"Update port host Id\n";
          uint16_t length = Host.length();
          uint32_t message = port;
          message |= length<<16;
          char temp[Host.length()];
          std::strcpy(temp, Host.c_str());
          write_int(self, hdl, static_cast<uint8_t>(operations::update_truck_behind));
          write_int(self, hdl, message);
          self->write(hdl, sizeof(char)*(length), temp);
          self->flush(hdl);
      }
    };
};
caf::behavior temp_server(caf::io::broker *self,const caf::actor& buddy){
    return{
        [=](const caf::io::new_connection_msg& msg) {
            std::cout << "[SERVER]: New Connection_Accepted" << std::endl;
            auto impl = self->fork(TruckBehind, msg.handle,buddy);
            self->send(impl, send_server_atom_v);
            self->quit(caf::sec::invalid_argument);
      },[=](send_new_command_atom, int32_t command){
          std::cout<<"NO ONE TO SEND COMMANDS TO\n";
      },
        
    };
}


