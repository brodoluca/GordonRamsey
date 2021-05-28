//
//  WebServer.cpp
//  Truck2
//
//  Created by Luca on 24/05/21.
//

#include "Truck.hpp"



caf::behavior server(caf::io::broker* self,const caf::actor & buddy) {
  auto counter = std::make_shared<int>(0);
  self->set_down_handler([=](caf::down_msg&) {
      aout(self) << "Listening for new connections" << std::endl;
    ++*counter;
  });
    self->link_to(buddy);
  self->delayed_send(self, std::chrono::seconds(1), caf::tick_atom_v);
  return {
    [=](const caf::io::new_connection_msg& ncm) {
      auto worker = self->fork(connection_worker, ncm.handle,std::move(buddy));
      self->monitor(worker);
//      self->link_to(worker);
    },
    [=](caf::tick_atom) {
//      aout(self) << "Finished " << *counter << " requests per second." << std::endl;
      *counter = 0;
      self->delayed_send(self, std::chrono::seconds(1), caf::tick_atom_v);
    }
  };
}


caf::behavior connection_worker(caf::io::broker* self, caf::io::connection_handle hdl,const caf::actor & buddy) {
  self->configure_read(hdl, caf::io::receive_policy::at_most(4000));
    
    self->request(buddy, std::chrono::seconds(2), prepare_json_message_v).await([self,hdl](std::string a){
        self->write(hdl, strlen(a.c_str())-1, a.c_str());
        self->flush(hdl);
//        self->quit();
    });
    
  return {
      [=](const caf::io::new_data_msg& msg) {
          char buf [400] = {'\0'};
          std::cout << "*****"<<msg.buf.size();
          memcpy(&buf, msg.buf.data(), sizeof(char)*msg.buf.size());
          std::string cpp_string(buf);
          std::cout << "**********new:"<<cpp_string;
      self->quit();
          
    },
    [=](const caf::io::connection_closed_msg&) {
        std::cout << "************closed";
      self->quit();
    }
  };
}

