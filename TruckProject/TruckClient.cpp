//
//  TruckClient.cpp
//  Truck2
//
//  Created by Luca on 02/05/21.
//

#include "Truck.hpp"

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
            self->send(self, send_server_atom_v);
        
        },[=](send_server_atom) {
            aout(self) << "[CLIENT]: Send Server: " << "" << std::endl;
            write_int(self, hdl, static_cast<uint8_t>(operations::assign_id));
            write_int(self, hdl,static_cast<int32_t>(2));
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
