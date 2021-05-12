//
//  TruckBehind.cpp
//  Truck2
//
//  Created by Luca on 02/05/21.
//
#include "Truck.hpp"


caf::behavior TruckBehind(caf::io::broker *self, caf::io::connection_handle hdl,const caf::actor& buddy){
    self->monitor(buddy);
    self->set_down_handler([=](caf::down_msg& dm) {
        if (dm.source == buddy) {
            std::cout << "My Mate is down" << std::endl;
//            self->quit(dm.reason);
        }
    });
//    self->request(buddy, std::chrono::seconds(2), get_port_atom_v).then([=](uint16_t new_port){
//        auto a = self->add_tcp_doorman(new_port);
//        });
//    auto a = self->add_tcp_doorman(3232);
    
    self->send(buddy, set_server_atom_v);
    self->send(buddy, increment_number_trucks_atom_v);
    self->configure_read(hdl, caf::io::receive_policy::at_least(sizeof(uint8_t)+sizeof(uint32_t)));
    return{
        [=](const caf::io::connection_closed_msg& msg) {
          if (msg.handle == hdl) {
            std::cout << "[SERVER]: Connection closed" << std::endl;
//              self->send_exit(buddy, caf::exit_reason::remote_link_unreachable);
//              self->quit(caf::exit_reason::remote_link_unreachable);
          }
    
        },[=](const caf::io::new_connection_msg& msg) {
            std::cout << "[SERVER]: New Connection_Accepted" << std::endl;
//            write_int(self, hdl, static_cast<uint8_t>(operations::get_port_host));
//            write_int(self, hdl, uint32_t{1});
      },[=](update_truck_numbers_atom, truck_quantity q) {
          write_int(self, hdl, static_cast<uint8_t>(operations::update_number_trucks_from_client));
          write_int(self, hdl,static_cast<uint32_t>(q));
          self->flush(hdl);
        
      },[=](send_new_command_atom, uint32_t command){
          std::cout<<"Started\n";
          write_int(self, hdl, static_cast<uint8_t>(operations::command));
          write_int(self, hdl, command);
          self->flush(hdl);
      },[=](become_master_atom){
          
          std::cout<<"IM NEW MASTER\n";
        
        
          self->fork(TruckServerMaster,std::move(hdl), std::move(buddy));
//          self->close(hdl);
//          self->quit(caf::exit_reason::remote_link_unreachable);
    
      },[=](update_truck_behind_port_host_atom, uint16_t p, std::string s){
          
      },[=](const caf::io::new_data_msg& msg) {
          auto rd_pos = msg.buf.data();
          auto op_val = uint8_t{0};
          read_int(rd_pos, op_val);
          ++rd_pos;
          auto val = uint32_t{0};
          read_int(rd_pos, val);
          ++rd_pos;
          switch (static_cast<operations>(op_val)) {
              case operations::assign_id:
                  self->request(buddy, std::chrono::seconds(1), which_id_atom_v).await([=](int32_t Id){
                      write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
                      write_int(self, hdl, Id-1);
                      write_int(self, hdl, MAX_TRUCKS - Id-1);
                      self->flush(hdl);
                  });
                  break;
              case operations::update_truck_behind:
//                receive host
                  char cstr[17];
                  while (strlen(cstr) < 15) {
                      memcpy(&cstr, ++rd_pos,sizeof(char)*17);
                  }
                  break;
                  
              case operations::master:
                    write_int(self, hdl, static_cast<uint8_t>(operations::master));
                    write_int(self, hdl, int32_t(0));
                    self->flush(hdl);
                  
              case operations::initialiaze_truck_platoon:
                    write_int(self, hdl, static_cast<uint8_t>(operations::master));
                    write_int(self, hdl, int32_t(0));
                    self->flush(hdl);
                  
                  break;
            default:
                  std::cout << "invalid No for op_val, stop" << std::endl;
                  self->quit(caf::sec::invalid_argument);
          };
      },[=](increment_number_trucks_backwards_atom) {
          
          write_int(self, hdl, static_cast<uint8_t>(operations::update_number_trucks));
          write_int(self, hdl, static_cast<uint32_t>(1));
          self->flush(hdl);
          
      },[=](send_server_atom){
//          std::cout<<"Send Server\n";
          self->request(buddy, std::chrono::seconds(1), which_id_atom_v).await([=](int32_t Id){
              write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
              write_int(self, hdl, Id-1);
              self->flush(hdl);
          });
          
//          self->request(buddy, std::chrono::seconds(1), which_front_id_atom_v).await([=](int32_t Id){
//              write_int(self, hdl, static_cast<uint8_t>(operations::front_id));
//              write_int(self, hdl, Id);
//              self->flush(hdl);
//          });
      },[=](update_id_behind_atom, uint32_t newId){
          std::cout<<"Upsate Id\n";
          write_int(self, hdl, static_cast<uint8_t>(operations::get_id));
          write_int(self, hdl, newId);
          self->flush(hdl);
      },[=](tell_back_im_master_atom){
          std::cout<<"Telle back\n";
          write_int(self, hdl, static_cast<uint8_t>(operations::master));
          write_int(self, hdl, uint32_t(1));
          self->flush(hdl);
      },[=](update_truck_behind_port_host_atom, uint16_t port, std::string Host){
          std::cout<<"Update port host Id\n";
          uint16_t length = Host.length();
          uint32_t message = port;
          message |= length<<16;
          char temp[Host.length()];
          std::strcpy(temp, Host.c_str());
          write_int(self, hdl, static_cast<uint8_t>(operations::update_truck_behind));
          write_int(self, hdl, message);
          self->write(hdl, sizeof(char)*(length), temp);
          self->flush(hdl);
          
      },[=](cascade_port_host_atom, uint16_t newPort, std::string newHost, truck_quantity stopID){
          std::cout<<"HEY, I need to send back\n";
          
      },[=](update_port_host_previous_atom){
          std::string Host = "localhost";
          char temp[20] = {'\0'};
          uint32_t message = 0;
          self->request(buddy, std::chrono::seconds(1), get_host_port_atom_v).then([&](std::pair<int32_t, std::string> pPortHost) mutable {
              Host = pPortHost.second;
              message = pPortHost.first;
//              std::cout << Host << message;
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
            self->request(buddy, std::chrono::seconds(1),get_port_host_previous_atom_v ).then([&](std::pair<int32_t, std::string> pPortHost) mutable {
                Host = pPortHost.second;
                message = pPortHost.first;
//                std::cout << Host << message;
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
    };
};



//Temporary server to allow the truck behind to connect to this truck
//This actor will die as soon as a connection is created.
caf::behavior temp_server(caf::io::broker *self,const caf::actor& buddy){
    std::cout << "[SERVER]: SPAWNED" << std::endl;
  
    return{
        [=](const caf::io::new_connection_msg& msg) {
            truck_quantity number_trucks=0;
            self->request(buddy, std::chrono::seconds(2), get_truck_numbers_atom_v).await(
                [&](truck_quantity tqNumberTrucks) mutable {
                    number_trucks = tqNumberTrucks;
            });
            if(number_trucks<=MAX_TRUCKS){
                std::cout << "[SERVER]: New Connection_Accepted" << std::endl;
                auto impl = self->fork(TruckBehind, msg.handle,buddy);
                self->send(impl, send_server_atom_v);
                self->delayed_send(impl, std::chrono::milliseconds(10),update_port_host_previous_atom_v);
                self->delayed_send(impl, std::chrono::milliseconds(20), update_back_up_atom_v);
                self->quit();
            }else{
                std::cout << "[SERVER]: Connection_refused : too many trucks" << std::endl;
                self->close(msg.handle);
            }
        },[=](send_new_command_atom, uint32_t command){
//          std::cout<<"NO ONE TO SEND COMMANDS TO\n";
        },[=](become_master_atom){
            self->become(temp_master_server(self, buddy));
        },
        
        [=](tell_back_im_master_atom){
//          std::cout<<"NO ONE TO SEND COMMANDS TO\n";
        },
        [=](update_truck_behind_port_host_atom, uint16_t p, std::string s){
//          std::cout<<"NO ONE TO SEND COMMANDS TO\n";
        },
//        caf::others >> [=](const caf::message &m) -> caf::skippable_result {
//            aout(self) << "[SERVER]: Unexpected message "<< std::endl;
//            return ;
//        }
//
    };
}


