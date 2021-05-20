//
//  TruckMasterClient.cpp
//  Truck2
//
//  Created by Luca on 18/05/21.
//

#include "Truck.hpp"

caf::behavior TruckMasterClient(caf::io::broker *self, caf::io::connection_handle hdl, const caf::actor& buddy){
    ///buddy is the truck actor. this function tells him to set this one as client
    self->send(buddy, set_client_atom_v);
    ///THIS IS FOR DEBUG PURPOSES
    ///pings the server to check if it works
    self->delayed_send(self, std::chrono::seconds(3), try_luca_atom_v);
    
    ///defines how much we want to read from the buffer
    self->configure_read(hdl, caf::io::receive_policy::at_most(sizeof(uint8_t)+sizeof(uint32_t)+sizeof(char)*21));

    return{
        ///THIS IS FOR DEBUG PURPOSES
        ///pings the server to check if it works
        [=](try_luca_atom){
            write_int(self, hdl, static_cast<uint8_t>(operations::try_luca));
            write_int(self, hdl, static_cast<uint32_t>(7));
            self->flush(hdl);
            self->delayed_send(self, std::chrono::seconds(3), try_luca_atom_v);
        },
        ///THIS IS FOR DEBUG PURPOSES
        ///THis is a placeholder, is not necessary anymore
        ///should be deleted
        ///USELESS
        [=](which_front_id_atom){
            write_int(self, hdl, static_cast<uint8_t>(operations::decrease_number_trucks));
            write_int(self, hdl, static_cast<uint32_t>(7));
            self->flush(hdl);
        },
        ///for the time beings, does nothing.
        [=](const caf::io::connection_closed_msg& msg){
            std::cout << "[MASTER_CLIENT]:Ah, stoppede connection aah" << std::endl;
        },
        
        [=](update_truck_numbers_atom, truck_quantity q) {
            write_int(self, hdl, static_cast<uint8_t>(operations::update_number_trucks));
            write_int(self, hdl,static_cast<uint32_t>(q));
            self->flush(hdl);
        },
        
        [=](const caf::io::new_data_msg& msg) mutable {
            auto rd_pos = msg.buf.data();
            auto op_val = uint8_t{0};
            read_int(rd_pos, op_val);
            ++rd_pos;
            auto val = uint32_t{0};
            read_int(rd_pos, val);
            //            This is insanely ugly. But gets things done.
            uint32_t temp_length = MASK_FIRST_HALF(val);
            uint16_t temp = SHIFT_TO_FIRST_BITS(temp_length);
            uint16_t temp_port = MASK_SECOND_HALF(val);
                        
            char cstr[22] = {'\0'}; //initialize translation buffer
            std::string ip; //declare conversion string
            switch (static_cast<operations>(op_val)) {
                    ///the server will try to assign an id to this client
                    ///This is obviously useless because the master already has an ID
                case operations::get_id:
                std::cout << "[MASTER_CLIENT]: Dont want a new ID"<< std::endl;
//                    write_int(self, hdl, static_cast<uint8_t>(operations::update_master_previous_host_port));
//                    write_int(self, hdl,static_cast<uint32_t>(0));
//                    self->flush(hdl);
//                    self->delayed_send(self, std::chrono::milliseconds(10), which_front_id_atom_v);
                    break;
                
                case operations::update_port_host_previous:
                    std::cout << "[MASTER_CLIENT]: he"<< std::endl;
//                    copy the buffer into a char buffer.
                    while (strlen(cstr) < temp+3) memcpy(&cstr, ++rd_pos, sizeof(char)*(temp+3));
            
                    factorHostPort(ip, cstr);

                    self->send(buddy, update_port_host_previous_atom_v, temp_port, ip );
                    break;
                    
                case operations::update_port_host_back_up:
//                    copy the buffer into a char buffer.
                    while (strlen(cstr) < temp+3) memcpy(&cstr, ++rd_pos, sizeof(char)*(temp+3));
                    
                    factorHostPort(ip, cstr);

                    self->send(buddy, update_back_up_atom_v, temp_port, ip );
                    break;
              default:
//                    std::cout << "[MASTER_CLIENT]:invalid value for op_val, stop" << std::endl;
                    break;
            };
        },
    
        ///This behavior is the link in chain when it comes to count the trucks
        ///It saves it in a string and sends the string.
        [=](count_trucks_atom, std::pair<uint32_t, uint32_t> pStopIdCount) {
            write_int(self, hdl, static_cast<uint8_t>(operations::count_trucks));
            std::string s = std::to_string(pStopIdCount.first);
            s += std::to_string(pStopIdCount.second);;
            write_int(self, hdl, int32_t(3));
            self->write(hdl, sizeof(char)*s.length(),s.c_str());
//            std::cout << pStopIdCount.second;
            self->flush(hdl);
        },
        
    };
}

