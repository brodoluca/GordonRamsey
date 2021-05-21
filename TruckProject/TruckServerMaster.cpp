

#include "Truck.hpp"

//TO DO
/*------------------------------------------------------------------------------------------------
//Create a different brokers to handle more connections
 This proker needs to be special, it only sends the commands coming from the main broker
 The messages received will be like handled like the main broker.
//Save them into a vector
//After that create a behavior of the main broker that sends the required messages to the  other brokers
---------------------------------------------------------------------------------------------------*/

///This file includes the behavior of the master.


caf::behavior TruckServerMaster(caf::io::broker *self, caf::io::connection_handle hdl, const caf::actor& buddy){
    ///monitors the buddy and when it's down, we quit
        self->monitor(buddy);
        self->set_down_handler([=](caf::down_msg& dm) {
            if (dm.source == buddy) {
                std::cout << "[MASTER]:My Mate is down" << std::endl;
                self->quit(dm.reason);
            }
        });
    
//    self->delayed_send(buddy, std::chrono::seconds(1), count_trucks_atom_v);
    ///Debugging reasons
    std::cout << "[MASTER]: I have been spawned.Connections: "<<self->num_connections() << std::endl;
    ///To allow multiple connections, we create a doorman
    self->request(buddy, std::chrono::milliseconds(500), get_port_atom_v).then([=](uint16_t truckPort){auto a = self->add_tcp_doorman(truckPort);});
    ///defines how much we want to read from the buffer
    self->configure_read(hdl,caf::io::receive_policy::at_most(sizeof(uint8_t)+sizeof(uint32_t)+sizeof(char)*21));
    ///Let's start with the inputs
    self->delayed_send(self, std::chrono::seconds(2), ask_for_input_atom_v);
    ///Let's start with the inputs
    self->delayed_send(self, std::chrono::seconds(10), set_speed_atom_v);
    ///My buddy becomes the master
    self->send(buddy, become_master_atom_v);
    self->send(buddy, set_server_atom_v);
    ///Send back new id
    self->request(buddy, std::chrono::seconds(2), assign_id_atom_v).then([=](int32_t newId){
            write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
            write_int(self, hdl, newId);
            self->flush(hdl);
        });
    self->request(buddy, std::chrono::milliseconds(50), get_truck_numbers_atom_v).await(
        [=](truck_quantity a ){
        if (a==3) {
            self->delayed_send(buddy, std::chrono::milliseconds(2000),count_trucks_atom_v);
        }
    });
    ///Initialize the platoon by adding one truck
    self->delayed_send(self, std::chrono::milliseconds(10), initialiaze_truck_platoon_atom_v);
    ///updates port and host of the truck behind
    self->delayed_send(self, std::chrono::milliseconds(20), update_port_host_previous_atom_v);
        return {
            [=](set_speed_atom){
                self->request(buddy, std::chrono::milliseconds(50), get_speed_atom_v).await(
                    [=](float speed) {
                        write_int(self, hdl, static_cast<uint8_t>(operations::set_speed));
                        write_int(self, hdl,static_cast<uint32_t>(speed));
                        self->flush(hdl);
                    },[&](const caf::error& err) {
                        std::cout << "*** cannot compute => "<< std::to_string(err.category()) << std::endl;
                  });
//                self->delayed_send(self, std::chrono::seconds(6), set_speed_atom_v);
            },
        ///polls the input to an actor responsible for it
            [=](ask_for_input_atom){
                auto input = self->home_system().spawn(InputMonitor);
                self->send(input, 1);
                self->delayed_send(self, std::chrono::seconds(10), ask_for_input_atom_v);
            },
        ///initializes the truck platoon and adds one to ir
        ///it also sends to the truck connected the new platoon size
            [=](initialiaze_truck_platoon_atom){
//                self->send(buddy, increment_number_trucks_atom_v, uint32_t(1));
//                self->request(buddy, std::chrono::seconds(1), get_truck_numbers_atom_v).then([=](truck_quantity a ){
//                    write_int(self, hdl, static_cast<uint8_t>(operations::update_number_trucks));
//                    write_int(self, hdl, uint32_t(a));
//                    self->flush(hdl);
//                });
                
            },
        ///requests the port and host from the truck (its own port and host) and
        ///sends them to the truck at the back, so that it can save them
        ///as previous host and previous port
            [=](update_port_host_previous_atom){
                std::string Host = "ciao";
                char temp[20] = {'\0'};
                uint32_t message = 0;
                self->request(buddy, std::chrono::seconds(1), get_host_port_atom_v).then([=](std::pair<int32_t, std::string> pPortHost) mutable {
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
    
        ///Handler for a "connection_closed_msg" (when the connection is closed)
        ///resizies the platoon as well
            [=](const caf::io::connection_closed_msg& msg) {
                std::cout << "[MASTER]: Connection closed" << std::endl;
                self->send(buddy, decrease_number_trucks_atom_v, truck_quantity(1));
                self->request(buddy, std::chrono::milliseconds(500), get_port_atom_v).then(
                    [=](uint16_t truckPort){auto a = self->add_tcp_doorman(truckPort);});
                
            },
        ///Use this part to send commands to the platoon
        ///The fifth command is used for testing
            [=](uint32_t a) {
                
                write_int(self, hdl, static_cast<uint8_t>(operations::command));
                switch (a) {
                    case 1:
                        write_int(self, hdl, static_cast<uint32_t>(commands::stop));
                        self->send(buddy,get_new_command_v,static_cast<uint32_t>(commands::stop));
                        break;
                    case 2:
                        write_int(self, hdl, static_cast<uint32_t>(commands::accellerate));
                        self->send(buddy,get_new_command_v,static_cast<uint32_t>(commands::accellerate));
                        break;
                    case 3:
                        write_int(self, hdl, static_cast<uint32_t>(commands::decellerate));
                        self->send(buddy,get_new_command_v,static_cast<uint32_t>(commands::decellerate));
                        break;
                    case 4:
                        write_int(self, hdl, static_cast<uint32_t>(commands::start));
                        self->send(buddy,get_new_command_v,static_cast<uint32_t>(commands::start));
                        break;
                    default:
                        break;
                }
                self->flush(hdl);
            },
        
            ///Leave it here,master should not use this to update the quantity,
            ///however, needed otherwirse the program crushes
            [=](update_truck_numbers_atom, truck_quantity q) {
        
          },
        ///Handler for a new message.
        ///Switch cases decides what to do
            [=](const caf::io::new_data_msg& msg) mutable {
                auto rd_pos = msg.buf.data(); //keep track of the position in the buffer
                auto op_val = uint8_t{0}; //initialize a variable to check the operation for the switchcase
                read_int(rd_pos, op_val); //read the value
                ++rd_pos; //increment the position of the buffer
                auto val = uint32_t{0}; //used to store the value
                read_int(rd_pos, val); //read the value
//                ++rd_pos; //increment the position of the buffer
                uint32_t val2 = 0; //used to store the value
                read_int(rd_pos, val2);
                
                //            This is insanely ugly. But gets things done.
                uint32_t temp_length = MASK_FIRST_HALF(val);
                uint16_t temp = SHIFT_TO_FIRST_BITS(temp_length);
                uint16_t temp_port = MASK_SECOND_HALF(val);
                
                char cstr[22] = {'\0'}; //initialize translation buffer
                std::string ip; //declare conversion string
                auto tqStopId = uint32_t{0};
                
                switch (static_cast<operations>(op_val)) {
                        ///Assigns the id to the truck by requesting it from the buddy
                    case operations::assign_id:
                        self->request(buddy, std::chrono::seconds(2), assign_id_atom_v).then([=](int32_t newId){
                            write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
                            write_int(self, hdl, newId);
                            self->flush(hdl);
                        });
                        break;
                        ///this is asked from the client of the other truck and tells whether this is a connection directly with the master
                    case operations::master:
                        write_int(self, hdl, static_cast<uint8_t>(operations::master));
                        write_int(self, hdl, int32_t(1));
                        self->flush(hdl);
                        break;
                        ///this updats the platoon size
                    case operations::update_number_trucks:
                        self->send(buddy, update_truck_numbers_atom_v,val);
                        break;
                        ///this updats the platoon size
                    case operations::update_number_trucks_from_client:
                        self->send(buddy, update_truck_numbers_atom_v,val);
                        break;
                        
                        
                        ///Used when we connect to a truck and the platoon size is 2
                        ///The master connects to this one to close the ring
                    case operations::master_connect_to_last_truck:
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
                      self->send(buddy, master_connects_to_truck_behind_v, temp_port, ip );
                        break;
                        
                        
                    case operations::count_trucks:
                        ++rd_pos;
                        ++rd_pos;
                        while (strlen(cstr) <val) {
                            memcpy(&cstr, ++rd_pos, sizeof(char)*(val+5));
                        }
                
                        ip = cstr;
                        memset(cstr, '0', sizeof(cstr));
                        val = ip.back() -'0';
                        ip.pop_back();
                        cstr[21] = ip.back();
                        ip.pop_back();
                        cstr[20] = ip.back();
                        ip.pop_back();
                    
                        ip.back();
                        val2 =atoi(cstr);
                        self->send(buddy, count_trucks_atom_v, std::make_pair(val2,val));
                        break;
                    
                        ///Just in case there is an error
                  default:
                        std::cout << "[MASTER]:invalid value for op_val, stop" << std::endl;
                        break;
                };
            },
            
            
            [=](update_back_up_atom){
                std::string Host = "localhost";
                char temp[20] = {'\0'};
                uint32_t message = 4242;
                self->request(buddy, std::chrono::seconds(4),get_port_host_previous_atom_v ).then([=](std::pair<int32_t, std::string> pPortHost) mutable {
                    Host = pPortHost.second;
                    message = pPortHost.first;
                    uint16_t length = Host.length();
                    while(Host.length()<15) Host.append("-");
                    Host.append("064");
                    length = Host.length();
                    message |= length<<16;
                    std::strcpy(temp, Host.c_str());
                    write_int(self, hdl, static_cast<uint8_t>(operations::update_port_host_back_up));
                    write_int(self, hdl, message);
                    self->write(hdl, sizeof(char)*(length), temp);
                    self->flush(hdl);
                    
                });
            },
            [=](update_port_host_previous_atom){
              std::string Host = "localhost";
              char temp[20] = {'\0'};
              uint32_t message = 0;
              self->request(buddy, std::chrono::seconds(4), get_host_port_atom_v).then([=](std::pair<int32_t, std::string> pPortHost) mutable {
                  
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
    
            [=](update_back_up_atom){
                std::string Host = "localhost";
                char temp[20] = {'\0'};
                uint32_t message = 4242;
                self->request(buddy, std::chrono::seconds(4),get_port_host_previous_atom_v ).then([=](std::pair<int32_t, std::string> pPortHost) mutable {
                    Host = pPortHost.second;
                    message = pPortHost.first;
                    uint16_t length = Host.length();
                    while(Host.length()<15) Host.append("-");
                    Host.append("064");
                    length = Host.length();
                    message |= length<<16;
                    std::strcpy(temp, Host.c_str());
                    write_int(self, hdl, static_cast<uint8_t>(operations::update_port_host_back_up));
                    write_int(self, hdl, message);
                    self->write(hdl, sizeof(char)*(length), temp);
                    self->flush(hdl);
                        
                });
            },
        
                ///This handles new communications
                ///If there are no other connections we fork to the same behavior and we quit this one
                ///(We basically allow reconnections)
                /// If not, we for to an external one and we store the actor
        
    
            [=](const caf::io::new_connection_msg& msg) {
                std::cout << "[MASTER]: New Connection_Accepted" << std::endl;
                if (self->num_connections() <=1 ) {
                    auto impl = self->fork(TruckServerMaster, msg.handle, std::move(buddy));
                    std::cout << "[MASTER]: Im gonna die and fork to a new broker. Connections: "<<self->num_connections() << std::endl;
                    self->quit(caf::sec::feature_disabled);
                    
                        
                    self->send(buddy, increment_number_trucks_atom_v);
//                    self->delayed_send(buddy, std::chrono::seconds(3), count_trucks_atom_v);
//                    self->delayed_send(impl, std::chrono::milliseconds(4000),update_port_host_previous_atom_v);
                    self->delayed_send(impl, std::chrono::milliseconds(50), update_back_up_atom_v);
                    
                    
                }else{
                    
                    ///This is the WIP part, not very useful atm
                    auto a = self->fork(TruckMasterMultiplexer, msg.handle, buddy);
                    self->send(buddy, add_connection_atom_v, a);
    
                }
            },
        };
}
///We create a temp server before the connection to the main one
///We do this because it allows us to save the handler for the connection
caf::behavior temp_master_server(caf::io::broker* self, const caf::actor& buddy) {
  std::cout << "[TEMP_SERVER]: running" << std::endl;
  return {
      ///The temp server forks to the main one and dies afterwards
    [=](const caf::io::new_connection_msg& msg) {
        std::cout << "[TEMP_SERVER]: New Connection_Accepted" << std::endl;
        auto impl = self->fork(TruckServerMaster, msg.handle, std::move(buddy));
        self->quit();
//        self->send(buddy, increment_number_trucks_atom_v);
    
        },
      [=](fork_to_master_atom, caf::io::connection_handle hdl){
      auto impl = self->fork(TruckServerMaster,hdl, std::move(buddy));
      self->quit();
    },
      ///I don't think these are actually being caleld, but they are here so the program doesnt crash
      ///Basically, there is a distinction between server with a client conencted and server with no client connected
      ///We dont do shit if we dont have a client e we leave it that way.
      
      [=](tell_back_im_master_atom){
    },[=](update_truck_behind_port_host_atom, uint16_t p, std::string s){
    },[=](ask_for_input_atom){
    },
    [=](update_truck_numbers_atom, truck_quantity q) {
    },
    [=](somebody_connected_atom){
           
    },
      
  };
}
