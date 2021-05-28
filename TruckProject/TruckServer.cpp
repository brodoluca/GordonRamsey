
#include "Truck.hpp"

///Implementation of the server in the truck.
///Allows the trucks to communicate with each other. Every truck has one, regardless if it'S a master or not.
///The truck behind this one will connect to this server and they will communicate with this channel.
///This will also be forked in case the master dies and this truck has a direct connection with it

caf::behavior TruckServer(caf::io::broker *self, caf::io::connection_handle hdl,const caf::actor& buddy){
    ///monitors the buddy and when it's down, we quit
    self->monitor(buddy);
    self->set_down_handler([=](caf::down_msg& dm) {
        if (dm.source == buddy) {
            std::cout << "My Mate is down" << std::endl;
            self->quit(dm.reason);
        }
        
    
    });
    
    
    
    
    self->request(buddy, std::chrono::milliseconds(5000), available_to_count_v).await([=](bool res){
        if (res == true ) {
            self->delayed_send(buddy, std::chrono::seconds(1), count_trucks_atom_v);
            self->send(buddy, set_switcheroo_atom_v, 1);
            self->delayed_send(self, std::chrono::milliseconds(3000),update_port_host_previous_atom_v);
            self->delayed_send(self, std::chrono::milliseconds(4000), update_back_up_atom_v);
        }else{
            self->delayed_send(buddy, std::chrono::seconds(1), count_trucks_atom_v);
        }
    });

    
    ///Sends to the buddy to save the pointer to the server
    self->send(buddy, set_server_atom_v);
    ///Increments the platoon size
//    self->send(buddy, increment_number_trucks_atom_v);
    ///Create doorman to allow reconnection
//    self->add_tcp_doorman(self->remote_port(hdl));
    self->request(buddy, std::chrono::milliseconds(500), get_port_atom_v).then(
        [=](uint16_t port) {auto a =self->add_tcp_doorman(port);});
    ///defines how much we want to read from the buffer
    self->configure_read(hdl, caf::io::receive_policy::at_least(sizeof(uint8_t)+sizeof(uint32_t)));
    return{
        ///If a connection is closed, we open a new doorman (allows to have other connections)
        ///Then we become the temp server again (it means nobody is connected to us)
        /// Then we wait decreases the size by two
        /// And we wait a bit before sending the somebody connected atom. Basically, this is for the switcheroo.
        ///When a truck needs to change position, it will disconnect to this one and then another one will reconnect. If we dont decrease by two, the size will be one more (every time a truck connects we increase by one)
        ///We wait a bit because if there*s no reconnection within 1 second, we just increase the size by one again
        [=](const caf::io::connection_closed_msg& msg) {
          if (msg.handle == hdl) {
            std::cout << "[SERVER]: Connection closed" << std::endl;
          }
            
            self->request(buddy, std::chrono::milliseconds(500), get_truck_numbers_atom_v).await(
                [=](truck_quantity size){
                    if(size != 1){
                        self->send(buddy, decrease_number_trucks_atom_v, truck_quantity(1));
                    }
                
                });
            
            self->request(buddy, std::chrono::milliseconds(500), get_port_atom_v).then(
                [=](uint16_t truckPort){auto a = self->add_tcp_doorman(truckPort);});
            
            self->become(temp_server(self, std::move(buddy)));
//            self->delayed_send(self, std::chrono::seconds(TIME_FOR_RECONNECTION), somebody_connected_atom_v);
        },
        
        ///When an election takes place, this sends back to the client the token
        [=](election_in_progress_token, uint32_t ID) {
            std::cout << "AOH:::"<<ID<<std::endl;
            write_int(self, hdl, static_cast<uint8_t>(operations::election_in_progress));
            write_int(self, hdl, ID);
            self->flush(hdl);
        },
        
        ///When an election takes place, this sends back to the client the token
        [=](reset_back_up) {
            write_int(self, hdl, static_cast<uint8_t>(operations::reset_back_up));
            write_int(self, hdl, 1);
            self->flush(hdl);
        },
        [=](reset_previous) {
            write_int(self, hdl, static_cast<uint8_t>(operations::reset_previous));
            write_int(self, hdl, 1);
            self->flush(hdl);
        },
        
        ///Updates the truck quantity by certain amount
        [=](update_truck_numbers_atom, truck_quantity q) {
          write_int(self, hdl, static_cast<uint8_t>(operations::update_number_trucks_from_client));
          write_int(self, hdl,static_cast<uint32_t>(q));
          self->flush(hdl);
          ///Cascades the command to the truck behind
      },[=](send_new_command_atom, uint32_t command){
          write_int(self, hdl, static_cast<uint8_t>(operations::command));
          write_int(self, hdl, command);
          self->flush(hdl);
          ///Allows this broker to fork into a master situation
      },[=](become_master_atom){
          self->fork(TruckServerMaster,std::move(hdl), std::move(buddy));
      },
        ///Updates the id to the truck in the back
        [=](update_id_cascade, int32_t newID) {
            write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
            write_int(self, hdl, newID);
            self->flush(hdl);
      },
        ///Placeholder
        ///If somebody did reconnect, we dont do anything
        ///When somebody leaves, we reduced the platoon size by two and we give a certain time for reconnecting, if doesnt happen,
        ///we will stay in the other behavior
        ///if it happens, we are here and dont do anything
       [=](somebody_connected_atom){
            
        },
        ///Handler for a new message.
        ///Switch cases decides what to do
          [=](const caf::io::new_data_msg& msg) {
              auto rd_pos = msg.buf.data();
              auto op_val = uint8_t{0};
              read_int(rd_pos, op_val);
              ++rd_pos;
              auto val = uint32_t{0};
              read_int(rd_pos, val);
              
              
              uint32_t val2 = 0; //used to store the value
              read_int(rd_pos, val2);
              char cstr[22] = {'\0'}; //initialize translation buffer
              std::string ip; //declare conversion string
              
              switch (static_cast<operations>(op_val)) {
                  case operations::assign_id:
                      self->request(buddy, std::chrono::seconds(1), which_id_atom_v).await([=](int32_t Id){
                          write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
                          write_int(self, hdl, Id-1);
                          self->flush(hdl);
                      });
                      break;
                    
                  case operations::master:
                        write_int(self, hdl, static_cast<uint8_t>(operations::master));
                        write_int(self, hdl, int32_t(0));
                        self->flush(hdl);
                      break;
                      
                  case operations::initialiaze_truck_platoon:
                        write_int(self, hdl, static_cast<uint8_t>(operations::master));
                        write_int(self, hdl, int32_t(0));
                        self->flush(hdl);
                      break;
                      
                      
                  case operations::update_number_trucks:
                      self->send(buddy, update_truck_numbers_atom_v, val);
                      break;

                  case operations::decrease_number_trucks:
                      self->send(buddy, decrease_number_trucks_atom_v);
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
                      ///Used to try new stuff
//                  case operations::try_luca:
////                      std::cout<< "AAAAA\n";
//                      break;
                  case operations::update_master_previous_host_port:
                      self->request(buddy, std::chrono::seconds(4),get_host_port_atom_v ).then([=](std::pair<int32_t, std::string> pPortHost) mutable {
                          std::string Host = "c";
                          char temp[20] = {'\0'};
                          uint32_t message = 0;
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
                      break;
                default:
                      std::cout << "[SERVER]:invalid No for op_val, stop" << std::endl;
                      break;
          };
        },
        
        [=](send_server_atom){
          self->request(buddy, std::chrono::seconds(1), which_id_atom_v).await([=](int32_t Id){
              write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
              write_int(self, hdl, Id-1);
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
        
    
        [=](const caf::io::new_connection_msg& msg) {
            std::cout << "[SERVER]:new connection accepted" << std::endl;
            self->request(buddy, std::chrono::seconds(2), get_truck_numbers_atom_v).await(
                [=](truck_quantity number_trucks)  {
                    if(number_trucks<=MAX_TRUCKS){
                        std::cout << "[SERVER]: Fork to the routine and send the new truck to update its coordinate" << std::endl;
                        auto switcheroo = self->fork(TruckSwitchRoutine, msg.handle, std::move(buddy));
                        self->send(buddy, set_switcheroo_atom_v, 0);
                        self->send(switcheroo,update_port_host_previous_atom_v);
//                        self->send(buddy, increment_number_trucks_atom_v);
//                        self->delayed_send(buddy, std::chrono::seconds(2), count_trucks_atom_v);
                        ///open new doorman to handle new connections
                        self->request(buddy, std::chrono::milliseconds(500), get_port_atom_v).then(
                            [=](uint16_t port) {
//                            std::cout << port << std::endl;
                            auto a =self->add_tcp_doorman(port);
                        });
                        
                    }else{
                        std::cout << "[SERVER]: Connection_refused : too many trucks" << std::endl;
                        self->close(msg.handle);
                    }
            });
        },
        
        [=](tell_back_im_master_atom){
        },
        [=](update_truck_behind_port_host_atom, uint16_t p, std::string s){

        },
    };
};


///Temporary server to allow the truck behind to connect to this truck
///This actor will die as soon as a connection is created.
caf::behavior temp_server(caf::io::broker *self,const caf::actor& buddy){
//    std::cout << "[SERVER]: SPAWNED" << std::endl;
    
    self->request(buddy, std::chrono::milliseconds(500), get_port_atom_v).then(
        [=](uint16_t port) {
        auto a =self->add_tcp_doorman(port);
    });
  
    return{
        ///Handles a new connection
        ///If the platoon size is less than a certain size, all good
        ///otherwise we close the connection
    
        [=](const caf::io::new_connection_msg& msg) {
            truck_quantity number_trucks=0;
            self->request(buddy, std::chrono::seconds(2), get_truck_numbers_atom_v).await(
                [=](truck_quantity tqNumberTrucks) mutable {
                    number_trucks = tqNumberTrucks;
            });
            if(number_trucks<=MAX_TRUCKS){
                std::cout << "[SERVER]: New Connection_Accepted" << std::endl;
                
                self->delayed_send(buddy, std::chrono::milliseconds(100),increment_number_trucks_atom_v);
                auto impl = self->fork(TruckServer, msg.handle,buddy);
                self->send(impl, send_server_atom_v);
                
//                self->delayed_send(impl, std::chrono::milliseconds(5000),update_port_host_previous_atom_v);
//                self->delayed_send(impl, std::chrono::milliseconds(6000), update_back_up_atom_v);
                self->quit();
            }else{
                std::cout << "[SERVER]: Connection_refused : too many trucks" << std::endl;
                self->close(msg.handle);
            }
        },
        ///If somebody doesnt reconnect, we update the truck size by one
        [=](somebody_connected_atom){
//            self->send(buddy, increment_number_trucks_atom_v);
        },
        ///Those are insanely useful. They save the program from  exploding
        ///These basically handles some messages that happen when this truck becomes the master.
        /// Without those the program will die. Dont erase them
        
        [=](send_new_command_atom, uint32_t command){
        },
        [=](become_master_atom){
            self->become(temp_master_server(self, buddy));
        },
        [=](tell_back_im_master_atom){
        },
        [=](update_truck_behind_port_host_atom, uint16_t p, std::string s){
        },
        [=](update_truck_numbers_atom, truck_quantity q) {

        },
        [=](update_port_host_previous_atom) {

        },
        [=](update_id_cascade, int32_t newID) {

        },
        [=](update_back_up_atom) {

        },
        [=](update_port_host_previous_atom) {

        },
        
    };
}

