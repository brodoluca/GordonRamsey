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
    auto a = self->remote_port(hdl);
    assert(self->num_connections() == 1);
    self->monitor(buddy);
    self->set_down_handler([=](caf::down_msg& dm) {
        if (dm.source == buddy) {
            std::cout << "My Mate is down" << std::endl;
            // Temporarely quit for this reason
            self->quit(dm.reason);
            
        }
    });
//    configure to exactly receive this much data
    self->configure_read(hdl, caf::io::receive_policy::at_most(sizeof(uint8_t)+sizeof(uint32_t)+sizeof(char)*17));
    std::cout << "Hey, Im alive\n";
    self->send(buddy, 1);
    return {
        [=](const caf::io::connection_closed_msg& msg){
          if (msg.handle == hdl) {
                std::cout << "connection closed" << std::endl;
                self->request(buddy, std::chrono::seconds(2), is_master_atom_v).then(
                [=](bool res){
                        if(res){
                            self->home_system().middleman().spawn_server(temp_master_server, a, buddy);
                            self->send(buddy, become_master_atom_v);
                            self->quit(caf::exit_reason::remote_link_unreachable);
                        }else{
//                            self->send_exit(buddy, caf::exit_reason::remote_link_unreachable);
//                            self->quit(caf::exit_reason::remote_link_unreachable);
                        }
                });
          }
            
        },
        [=](is_master_atom) {
            write_int(self, hdl, static_cast<uint8_t>(operations::master));
            write_int(self, hdl, static_cast<uint32_t>(2));
            self->flush(hdl);
        
        },
        [=](initialize_atom) {
            std::cout << "[CLIENT]: Send Server: " << "" << std::endl;
            write_int(self, hdl, static_cast<uint8_t>(operations::assign_id));
            write_int(self, hdl,static_cast<int32_t>(2));
            self->flush(hdl);
        
        },[=](update_master_atom, std::string host, uint16_t port) {
            //            auto impl = self->home_system().middleman().spawn_client(TruckClient, host, port, std::move(buddy));
            //            self->quit();
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
            uint32_t temp_length = val & 0b11111111111111110000000000000000;
            uint16_t temp = temp_length>>16;
            uint16_t temp_port = val & 0xFFFFFFFF>>16;
            char cstr[17] = {'\0'};
            std::string ip;
            switch (static_cast<operations>(op_val)) {
                case operations::front_id:
                    self->send(buddy, set_front_id_v, int32_t(val));
                    break;
                case operations::get_id:
                std::cout << "[CLIENT]: Received new ID: "<<val << std::endl;
                    self->send(buddy, get_new_id_atom_v, int32_t(val));
                    break;
                
//                case operations::update_truck_behind:
//                    while (strlen(cstr) < 15) {
//                        memcpy(&cstr, ++rd_pos,sizeof(char)*17);
//                    }
//                    self->home_system().middleman().spawn_client(TruckClient, cstr, uint16_t(val), buddy);
//                    self->quit();
//                    break;
                case operations::master:
                    self->anon_send(buddy, set_master_connection_atom_v, bool(val));
                    std::cout << "master"  << val << "\n";
                    break;
                case operations::update_id_behind:
                    self->send(buddy, update_id_behind_atom_v);
                    break;
                case operations::command:
                    std::cout << "[CLIENT]: Received new command" << std::endl;
                    self->send(buddy, get_new_command_v, int32_t(val));
                    break;
                case operations::get_port_host:
                    self->request(buddy, std::chrono::seconds(2), get_host_port_atom_v).then(
                            [=](std::pair<int32_t, std::string> pHostPort){
                                char* temp=new char(17);
                                std::strcpy(temp, pHostPort.second.c_str());
                                for(unsigned long i=pHostPort.second.length();i<17;i++)
                                    temp[i] = '-';
                                write_int(self, hdl, static_cast<uint8_t>(operations::update_truck_behind));
                                write_int(self, hdl, pHostPort.first);
                        
                                self->write(hdl, sizeof(char)*17, temp);
                                self->flush(hdl);
                                });
                    break;
                case operations::ready:
                    std::cout << val;
                    write_int(self, hdl, static_cast<uint8_t>(operations::assign_id));
                    write_int(self, hdl, 1);
                    self->flush(hdl);
                    break;
                case operations::update_truck_behind:
                    while (strlen(cstr) < temp+1) {
                        memcpy(&cstr, ++rd_pos,sizeof(char)*(temp+4));
                    }
        
                    ip = cstr;
                    ip.erase(ip.begin(), ip.begin()+3);
                    self->send(buddy, update_master_atom_v,ip, temp_port);
                    self->quit();
                    break;
              default:
                std::cout << "invalid value for op_val, stop" << std::endl;
                    self->quit(caf::sec::invalid_argument);
                    break;
            };

        }

    };
}
