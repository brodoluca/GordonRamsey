//
//  TruckMasterClient.cpp
//  Truck2
//
//  Created by Luca on 18/05/21.
//

#include "Truck.hpp"


caf::behavior TruckMasterClient(caf::io::broker *self, caf::io::connection_handle hdl, const caf::actor& buddy){
    
    self->send(buddy, set_client_atom_v);
    self->delayed_send(self, std::chrono::seconds(3), try_luca_atom_v);
    ///defines how much we want to read from the buffer
    self->configure_read(hdl, caf::io::receive_policy::at_most(sizeof(uint8_t)+sizeof(uint32_t)+sizeof(char)*21));
//    self->delayed_send(self, std::chrono::seconds(1), which_front_id_atom_v);
    return{
        [=](try_luca_atom){
            write_int(self, hdl, static_cast<uint8_t>(operations::try_luca));
            write_int(self, hdl, static_cast<uint32_t>(7));
            self->flush(hdl);
            self->delayed_send(self, std::chrono::seconds(3), try_luca_atom_v);
        },
        [=](which_front_id_atom){
            write_int(self, hdl, static_cast<uint8_t>(operations::decrease_number_trucks));
            write_int(self, hdl, static_cast<uint32_t>(7));
            self->flush(hdl);
        },
        [=](const caf::io::connection_closed_msg& msg){
            std::cout << "[MASTER_CLIENT]:Ah, stoppede connection aah" << std::endl;
            
        },
        [=](const caf::io::new_data_msg& msg) mutable {
            auto rd_pos = msg.buf.data(); //keep track of the position in the buffer
            auto op_val = uint8_t{0}; //initialize a variable to check the operation for the switchcase
            read_int(rd_pos, op_val); //read the value
            ++rd_pos; //increment the position of the buffer
            auto val = uint32_t{0}; //used to store the value
            read_int(rd_pos, val); //read the value
            switch (static_cast<operations>(op_val)) {
                case operations::get_id:
                std::cout << "[MASTER_CLIENT]: Dont want a new ID"<< std::endl;
                    self->delayed_send(self, std::chrono::milliseconds(10), which_front_id_atom_v);
                    break;
              default:
//                    std::cout << "[MASTER_CLIENT]:invalid value for op_val, stop" << std::endl;
                    break;
            };
        },
        ///Asks server to count trucks
        [=](count_trucks_atom, std::pair<uint32_t, uint32_t> pStopIdCount) {
            write_int(self, hdl, static_cast<uint8_t>(operations::count_trucks));
//            write_int(self, hdl, static_cast<uint32_t>(pStopIdCount.first));
//            write_int(self, hdl,  static_cast<uint32_t>(pStopIdCount.second));
            std::string s = std::to_string(pStopIdCount.first);
            s += std::to_string(pStopIdCount.second);;
            write_int(self, hdl, int32_t(3));
            self->write(hdl, sizeof(char)*s.length(),s.c_str());
            std::cout << pStopIdCount.second;
            self->flush(hdl);
        },
        
    };
}

