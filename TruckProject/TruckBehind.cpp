//
//  TruckBehind.cpp
//  Truck2
//
//  Created by Luca on 02/05/21.
//

#include "Truck.hpp"
caf::behavior TruckBehind(caf::io::broker *self, caf::io::connection_handle hdl,const caf::actor& buddy){
    std::cout<<"Started\n";
    self->monitor(buddy);
    self->set_down_handler([=](caf::down_msg& dm) {
        if (dm.source == buddy) {
            aout(self) << "My Mate is down" << std::endl;
            self->quit(dm.reason);
        }
    });
    self->send(buddy, set_server_atom_v);
    self->configure_read(hdl, caf::io::receive_policy::at_most(sizeof(uint8_t)+sizeof(uint32_t)));
    return{
        [=](const caf::io::new_connection_msg& msg) {
            aout(self) << "[SERVER]: New Connection_Accepted" << std::endl;
        
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
          switch (static_cast<operations>(op_val)) {
              case operations::assign_id:
                  self->request(buddy, std::chrono::seconds(4), which_id_atom_v).await([=](int32_t Id){
                      write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
                      write_int(self, hdl, Id-1);
                      self->flush(hdl);
                  });
                  
                  break;
            default:
                  aout(self) << "invalid value for op_val, stop" << std::endl;
                  self->quit(caf::sec::invalid_argument);
          };
      },

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
