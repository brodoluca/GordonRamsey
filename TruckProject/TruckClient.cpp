//
//  TruckClient.cpp
//  Truck2
//
//  Created by Luca on 02/05/21.
//
#include "Truck.hpp"


caf::behavior TruckClient(caf::io::broker *self, caf::io::connection_handle hdl, const caf::actor& buddy){
    ///monitors the buddy and when it's down, we quit
    self->monitor(buddy);
    self->set_down_handler([=](caf::down_msg& dm) {
        if (dm.source == buddy) {
            std::cout << "[CLIENT] : Truck is down" << std::endl;
            // Temporarely quit for this reason
            self->quit(dm.reason);
        }
    });
    ///defines how much we want to read from the buffer
    self->configure_read(hdl, caf::io::receive_policy::at_most(sizeof(uint8_t)+sizeof(uint32_t)+sizeof(char)*21));
    return {
        ///Handles a closed connection.
        ///If it's a master connection this becomes the master
        ///Otherwise tries to use the backup connection
        [=](const caf::io::connection_closed_msg& msg){
          if (msg.handle == hdl) {
                self->request(buddy, std::chrono::seconds(2), is_master_atom_v).then(
                [=](bool res){
                        if(res){
                            std::cout << "[TRUCK]: Connection to master closed" << std::endl;
                            self->send(buddy, decrease_number_trucks_atom_v);
                            self->send(buddy, become_master_atom_v);
                            self->quit(caf::exit_reason::remote_link_unreachable);
                        }else{
                            std::cout << "[TRUCK]: Connection to truck in front closed" << std::endl;
                            self->delayed_send(buddy, std::chrono::milliseconds(10),truck_left_or_dead_atom_v);
                        }
                });
          }
        },
    
        ///asks if it's a master connection
        [=](is_master_atom) {
            write_int(self, hdl, static_cast<uint8_t>(operations::master));
            write_int(self, hdl, static_cast<uint32_t>(2));
            self->flush(hdl);
        },
        
        ///updates the number of trucks by a certain amount
        [=](increment_number_trucks_upwards_atom, uint32_t newTrucks) {
            write_int(self, hdl, static_cast<uint8_t>(operations::update_number_trucks));
            write_int(self, hdl, static_cast<uint32_t>(newTrucks));
            self->flush(hdl);
        },
        ///Asks for the id
        [=](initialize_atom) {
            write_int(self, hdl, static_cast<uint8_t>(operations::assign_id));
            write_int(self, hdl,static_cast<int32_t>(2));
            self->flush(hdl);
        },
        ///updates the number of trucks by a certain quantity
        [=](update_truck_numbers_atom, truck_quantity q) {
            write_int(self, hdl, static_cast<uint8_t>(operations::update_number_trucks_from_client));
            write_int(self, hdl,static_cast<uint32_t>(q));
            self->flush(hdl);
        },
        
        ///Handles new messages with a switch case
        [=](const caf::io::new_data_msg& msg) {
            auto rd_pos = msg.buf.data();
            // Read the operation value as uint8_t from the buffer.
            auto op_val = uint8_t{0};
            read_int(rd_pos, op_val);
            ++rd_pos;
            auto val = uint32_t{0};
            read_int(rd_pos, val);
    
//            This is insanely ugly. But gets things done.
            uint32_t temp_length = val & 0b11111111111111110000000000000000; //Masks the first half of the number
            uint16_t temp = temp_length>>16; //Shifts it to the first two bits, so we get a nice number
            uint16_t temp_port = val & 0xFFFFFFFF>>16; //masks the last part of the bumber
            
            char cstr[22] = {'\0'}; //initialize translation buffer
            std::string ip; //declare conversion string
            auto tqStopId = uint32_t{0}; //declare variable in which we save the Stop ID (ID at which we stop)
            
            
            switch (static_cast<operations>(op_val)) {
                    ///Assigns new front id
                    ///this is useless
                case operations::front_id:
                    self->send(buddy, set_front_id_v, int32_t(val));
                    break;
                    ///Assigns new id
                case operations::get_id:
                std::cout << "[CLIENT]: Received new ID: "<<val << std::endl;
                    self->send(buddy, get_new_id_atom_v, int32_t(val));
                    break;
                    ///Receives whether is a master connection or not
                    
                case operations::master:
                    self->anon_send(buddy, set_master_connection_atom_v, bool(val));
                    std::cout << "[CLIENT] :master connection : "  << bool(val) << "\n";
                    break;
                    ///Updates the id from a non master connection
                case operations::update_id_behind:
                    self->send(buddy, update_id_behind_atom_v);
                    break;
                    ///Sends the command to the truck
                case operations::command:
                    std::cout << "[CLIENT]: Received new command" << std::endl;
                    self->send(buddy, get_new_command_v, int32_t(val));
                    break;
                    ///Sends port and host
                case operations::get_port_host:
                    self->request(buddy, std::chrono::seconds(2), get_host_port_atom_v).then(
                            [=](std::pair<int32_t, std::string> pHostPort){
                                    char* temp=new char(17);
                                    std::strcpy(temp, pHostPort.second.c_str());
                                    for(unsigned long i=pHostPort.second.length();i<21;i++)
                                        temp[i] = '-';
                                    write_int(self, hdl, static_cast<uint8_t>(operations::update_truck_behind));
                                    write_int(self, hdl, pHostPort.first);
                                    self->write(hdl, sizeof(char)*17, temp);
                                    self->flush(hdl);
                                    delete temp;
                                });
                    break;
                    ///Receives the port and host of the truck in front
                    /// It stores them so that they can be sent to the truck behind to be used as a back up
                case operations::update_port_host_previous:
//                    copy the buffer into a char buffer.
                    while (strlen(cstr) < temp+3) {
                        memcpy(&cstr, ++rd_pos, sizeof(char)*(temp+3));
                    }
                
//                    convert it to a c++ string
                    ip = cstr;
//                    set the memory of the copy buffer to 0
                    memset(cstr, '0', sizeof(cstr));
//                    remove the first 3 chars of the string (There is always some garbage in the buffer)
                    ip.erase(ip.begin(), ip.begin()+3);
//                    save the 3 char at the end of the string (that's where we save the stop ID)
                    cstr[21] = ip.back();
                    ip.pop_back();
                    cstr[20] = ip.back();
                    ip.pop_back();
                    cstr[19] = ip.back();
                    ip.pop_back();
//                    convert it to an in
                    tqStopId =atoi(cstr);
//                    remove the '-' char from the string (place holders)
                    while(ip.back() == '-') ip.pop_back();
//                    prints it for pure convenience
                    self->send(buddy, update_port_host_previous_atom_v, temp_port, ip );
//                    std::cout << "ID:"<< tqStopId << "  PORT: "<<temp_port << "  LENGTH : "<<temp << "  "<<ip;
                    break;
                    
            
                    ///Stores the port and host of the truck in front of the truck in front
                    ///use them as back up in case the truck in front fucks up
                    
                case operations::update_port_host_back_up:
//                    copy the buffer into a char buffer.
                    while (strlen(cstr) < temp+3) {
                        memcpy(&cstr, ++rd_pos, sizeof(char)*(temp+3));
                    }
//                    convert it to a c++ string
                    ip = cstr;
//                    set the memory of the copy buffer to 0
                    memset(cstr, '0', sizeof(cstr));
//                    remove the first 3 chars of the string (There is always some garbage in the buffer)
                    ip.erase(ip.begin(), ip.begin()+3);
//                    save the 3 char at the end of the string (that's where we save the stop ID)
                    cstr[21] = ip.back();
                    ip.pop_back();
                    cstr[20] = ip.back();
                    ip.pop_back();
                    cstr[19] = ip.back();
                    ip.pop_back();
//                    convert it to an in
                    tqStopId =atoi(cstr);
//                    remove the '-' char from the string (place holders)
                    while(ip.back() == '-') ip.pop_back();
//                    prints it for pure convenience
                    self->send(buddy, update_back_up_atom_v, temp_port, ip );
//                    std::cout << "ID:"<< tqStopId << "  PORT: "<<temp_port << "  LENGTH : "<<temp << "  "<<ip;
                    break;
                    
                    ///increments the truck platoon by a certain amount
                case operations::initialiaze_truck_platoon:
                    self->send(buddy, increment_number_trucks_atom_v, uint32_t(val));
                    break;
                    ///Updates the platoon size
                case operations::update_number_trucks_from_client:
                    self->send(buddy, update_truck_numbers_atom_v,val);
                    break;

                    ///Updates the platoon size
                case operations::update_number_trucks:
                    self->send(buddy, update_truck_numbers_atom_v,val);
                    break;
              default:
                std::cout << "[CLIENT] : Invalid value for op_val, stop" << std::endl;
                    break;
            };
        }
    };
}
