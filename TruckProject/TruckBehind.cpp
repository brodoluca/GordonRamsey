//
//  TruckBehind.cpp
//  Truck2
//
//  Created by Luca on 02/05/21.
//

#include "Truck.hpp"
#include "string.h"
caf::behavior TruckBehind(caf::io::broker *self, caf::io::connection_handle hdl,const caf::actor& buddy){
    self->monitor(buddy);
    self->set_down_handler([=](caf::down_msg& dm) {
        if (dm.source == buddy) {
            aout(self) << "My Mate is down" << std::endl;
            self->quit(dm.reason);
        }
    });
    self->link_to(buddy);
    self->set_exit_handler([=](caf::exit_msg& ms){
        if (ms.source == buddy) {
            aout(self) << "My Mate is dead" << std::endl;
            self->quit(ms.reason);
        }
    });
    
    self->send(buddy, set_server_atom_v);
    self->configure_read(hdl, caf::io::receive_policy::at_least(sizeof(uint8_t)+sizeof(uint32_t)));
    return{
        [=](const caf::io::connection_closed_msg& msg) {
          if (msg.handle == hdl) {
            aout(self) << "[SERVER]: Connection closed" << std::endl;
              self->send_exit(buddy, caf::exit_reason::remote_link_unreachable);
              self->quit(caf::exit_reason::remote_link_unreachable);
          }
            
        },[=](const caf::io::new_connection_msg& msg) {
            aout(self) << "[SERVER]: New Connection_Accepted" << std::endl;
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
                  self->request(buddy, std::chrono::seconds(4), which_id_atom_v).await([=](int32_t Id){
                      write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
                      write_int(self, hdl, Id-1);
                      self->flush(hdl);
                
                  });
                  self->request(buddy, std::chrono::seconds(4), which_id_atom_v).await([=](int32_t Id){
                      write_int(self, hdl, static_cast<uint8_t>(operations::front_id));
                      write_int(self, hdl, Id);
                      self->flush(hdl);
                  });
                
                  
                  break;
              case operations::update_truck_behind:
                  std::cout << "ok";
                  std::cout << val;
//                receive host
                  char cstr[17];
                  while (strlen(cstr) < 15) {
                      memcpy(&cstr, ++rd_pos,sizeof(char)*17);
                  }
                  
                  break;
            default:
                  aout(self) << "invalid No for op_val, stop" << std::endl;
                  self->quit(caf::sec::invalid_argument);
          };
    
          
          
      },[=](update_id_behind_atom, uint32_t newId){
          std::cout<<"Started\n";
          write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
          write_int(self, hdl, newId);
          self->flush(hdl);
      },[=](tell_back_im_master_atom){
          std::cout<<"Started\n";
          write_int(self, hdl, static_cast<uint8_t>(operations::master));
          write_int(self, hdl, int32_t(1));
          self->flush(hdl);
      },[=](const caf::io::new_datagram_msg& msg) {
          auto rd_pos = msg.buf.data();
          char cstr[3];
          *cstr = 'v';
          memcpy(cstr, rd_pos, 1);
          std::cout << cstr;
      }

    };
};
caf::behavior temp_server(caf::io::broker *self,const caf::actor& buddy){
    return{
        [=](const caf::io::new_connection_msg& msg) {
            aout(self) << "[SERVER]: New Connection_Accepted" << std::endl;
            auto impl = self->fork(TruckBehind, msg.handle,buddy);
            self->quit(caf::sec::invalid_argument);
      },[=](send_new_command_atom, int32_t command){
          std::cout<<"NO ONE TO SEND COMMANDS TO\n";
      },
        
    };
}



//static char* util(const void* data){
//    char* cstr = new char[16];
//    std::cout << cstr;
//    std::cout << strlen(cstr);
//    std::cout << "\n";
//    for(int i =0;i<16;i++){
//        if (i<4) {
//            cstr[i] = 'v';
//        }else
//            cstr[i] = 'b';
//    }
//    std::cout << cstr;
//    std::cout << strlen(cstr);
//    std::cout <<"\n";
//    return cstr;
//}
