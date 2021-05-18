
#include "Truck.hpp"

///WIP of a new way of thinking at the platoon
///instead of using linked-lists as a base data structure,  a tree might be more flexible
///However, the three presents more challanges
///First of all, we need to have brokers that take care of each connections with each truck (which is already been taken care of, more or less, by saving them in a vectr)
///Second of all, Let's say truck A is the master and 4 trucks, namely B, C, D, E, are connected to it
///and truck A dies, how do we decide which one becomes the master? (one solution might be selection algorithms).
///Moreover,  what happens to the trucks which are not chosen to be the master?
///We can store the ip address of the truck behind and tell them to connect to it. However, we also need to modify how the TruckServer handles new connections.
///This can be done later once the main features are terminated. 


caf::behavior TruckMasterMultiplexer(caf::io::broker *self, caf::io::connection_handle hdl, const caf::actor& buddy){
    
    self->monitor(buddy);
    self->set_down_handler([=](caf::down_msg& dm) {
        if (dm.source == buddy) {
            std::cout << "[MASTER_MULTIPLEXER]:My Mate is down" << std::endl;
            self->quit(dm.reason);
        }
    });
    std::cout << "[MASTER_MULTIPLEXER]: I have been spawned.Connections: "<<self->num_connections() << std::endl;
    self->delayed_send(self, std::chrono::seconds(2), ask_for_input_atom_v);
    self->configure_read(hdl, caf::io::receive_policy::exactly(sizeof(uint8_t)+sizeof(uint32_t)));
    //    Become master

    //    self->send(buddy, set_server_atom_v);
    self->request(buddy, std::chrono::seconds(2), assign_id_atom_v).then([=](int32_t newId){
            write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
            write_int(self, hdl, newId);
            self->flush(hdl);
        });

//    self->delayed_send(self, std::chrono::milliseconds(10), initialiaze_truck_platoon_atom_v);
    self->delayed_send(self, std::chrono::milliseconds(30), update_port_host_previous_atom_v);
    
    
    return {
        [=](initialiaze_truck_platoon_atom){
            self->send(buddy, increment_number_trucks_atom_v, uint32_t(1));
            self->request(buddy, std::chrono::seconds(1), get_truck_numbers_atom_v).then([=](truck_quantity a ){
                write_int(self, hdl, static_cast<uint8_t>(operations::update_number_trucks));
                write_int(self, hdl, uint32_t(a));
                self->flush(hdl);
            });

        },
        ///requests the port and host from the truck (its own port and host) and
        ///sends them to the truck at the back, so that it can save them
        ///as previous host and previous port
        [=](update_port_host_previous_atom){
            std::string Host = "ciao";
            char temp[20] = {'\0'};
            uint32_t message = 0;
            self->request(buddy, std::chrono::seconds(2), get_host_port_atom_v).then([=](std::pair<int32_t, std::string> pPortHost) mutable {
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
        ///Message received by the main behavior.
        ///This is not supposed to happen here, so we keep it empty

        [=](ask_for_input_atom){
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
                    std::cout << "[MASTER_MULTIPLEXER]:invalid value for op_val, stop" << std::endl;
//                        self->quit(caf::sec::invalid_argument);
                    break;
            };
        }
    };
}
