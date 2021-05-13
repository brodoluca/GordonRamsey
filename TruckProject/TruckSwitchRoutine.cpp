//
//  TruckSwitchRutine.cpp
//  Truck2
//
//  Created by Luca on 13/05/21.
//

#include "Truck.hpp"

///This behavior here has the purpose of switching the position of the trucks
///Basically, when there is a truck that wants to connect to another one, but this one already has a conenction,
///it will switch position with the truck asking to enter, and then connect to that one.
///  To have a better idea of how this works, check our github
///   https://github.com/brodoluca/GordonRamsey
caf::behavior TruckSwitchRoutine(caf::io::broker *self, caf::io::connection_handle hdl,const caf::actor& buddy){
    
    self->configure_read(hdl, caf::io::receive_policy::at_most(sizeof(uint8_t)+sizeof(uint32_t)+sizeof(char)*21));
    return{
        
        ///This truck can not accept new connections
        [=](const caf::io::new_connection_msg& msg){
            std::cout << "[TRUCK_SWITCH_ROUTINE]: I can not accept any connection "<<std::endl;
            self->close(msg.handle);
        },
        
        ///When the conenction is closed, we quit and
        /// send this to buddy and tell him to do the old switcherooo(create a new client with previousHost and previous Port)
        [=](const caf::io::connection_closed_msg& msg){
            std::cout << "[TRUCK_SWITCH_ROUTINE]: It's time for the good old switcheroo"<<std::endl;
            self->delayed_send(buddy, std::chrono::seconds(1),create_new_client_connection_previous_hostport_atom_v);
            self->quit(caf::sec::redundant_connection);
        },
        
        ///send previous truck port host
        ///These allow the new truck to connect with the truck in front
        /// Necessary for the old switcheroo
        [=](update_port_host_previous_atom){
            std::cout << "[TRUCK_SWITCH_ROUTINE]: It's time to update the coordinates"<<std::endl;
            std::string Host = "ciao";
            char temp[20] = {'\0'};
            uint32_t message = 0;
            self->request(buddy, std::chrono::seconds(1), get_port_host_previous_atom_v).then([=](std::pair<int32_t, std::string> pPortHost) mutable {
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
            self->delayed_send(self, std::chrono::milliseconds(5),request_for_port_host_atom_v);
        },
        
        /// send request for his port and host
        [=](request_for_port_host_atom){
            std::cout << "[TRUCK_SWITCH_ROUTINE]: Can I have your shit please?"<<std::endl;
            write_int(self, hdl, static_cast<uint8_t>(operations::request_for_host_port));
            self->flush(hdl);
        },
        
        /// Tells the new client to close this connection and connect to the truck behind
        [=](allow_switcheroo_atom){
            std::cout << "DO it, pussy"<<std::endl;
            write_int(self, hdl, static_cast<uint8_t>(operations::connect_new_server));
            self->flush(hdl);
        },
        
        [=](const caf::io::new_data_msg& msg) mutable {
            auto rd_pos = msg.buf.data();
            // Read the operation value as uint8_t from the buffer.
            auto op_val = uint8_t{0};
            read_int(rd_pos, op_val);
            ++rd_pos;
            auto val = uint32_t{0};
            read_int(rd_pos, val);
    
//            This is insanely ugly. But gets things done.
            ///this is useful only in case ports and hosts are sent. In case they are not, this does no harm.
            ///A bit of overhead, but come on, new computers should be able to handle this in no time. 
            uint32_t temp_length = val & 0b11111111111111110000000000000000; //Masks the first half of the number
            uint16_t temp = temp_length>>16; //Shifts it to the first two bits, so we get a nice number after conversion
            uint16_t temp_port = val & 0xFFFFFFFF>>16; //masks the last part of the bumber
            
            char cstr[22] = {'\0'}; //initialize translation buffer
            std::string ip; //declare conversion string
            auto tqStopId = uint32_t{0}; //declare variable in which we save the Stop ID (ID at which we stop)
        
            switch (static_cast<operations>(op_val)) {
                    ///Assigns the id to the truck by requesting it from the buddy
                case operations::request_for_host_port:
                    //                    copy the buffer into a char buffer.
                    while (strlen(cstr) < temp+3) {
                        memcpy(&cstr, ++rd_pos, sizeof(char)*(temp+3));
                    }
                    ip = cstr;
                    memset(cstr, '0', sizeof(cstr));
                    ip.erase(ip.begin(), ip.begin()+3);
                    cstr[21] = ip.back();
                    ip.pop_back();
                    cstr[20] = ip.back();
                    ip.pop_back();
                    cstr[19] = ip.back();
                    ip.pop_back();
                    tqStopId =atoi(cstr);
                    while(ip.back() == '-') ip.pop_back();
                    std::cout << "[TRUCK_SWITCH_ROUTINE]: I possess your data"<<std::endl;
                    self->delayed_send(buddy, std::chrono::milliseconds(10), update_port_host_previous_atom_v, temp_port, ip );
                    self->delayed_send(buddy,std::chrono::milliseconds(200),close_client_connection_atom_v);
                    break;
                    ///error handling.
              default:
                    std::cout << "[TRUCK_SWITCH_ROUTINE]:invalid value for op_val, stop" << std::endl;
                    break;
            };
        },
        
        
        
    };
}
