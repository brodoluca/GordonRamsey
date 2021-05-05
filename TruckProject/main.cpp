//
//  main.cpp
//  Testing_brokers
//
//  Created by Luca on 21/04/21.
//
#include <iostream>
#include "caf/all.hpp"
#include "caf/io/all.hpp"
#include <string.h>

#include "Truck.hpp"

class config : public caf::actor_system_config {
public:
  uint16_t port = 4242;
  std::string host = "localhost";
  bool server_mode = false;

  config() {
    opt_group{custom_options_, "global"}
      .add(port, "port,p", "set port")
      .add(host, "host,H", "set host (ignored in server mode)");
    
  }
};

caf::behavior server(caf::io::broker* self, const caf::actor& buddy);
void run_server(caf::actor_system& system, const config& cfg);

void caf_main(caf::actor_system& system, const config& cfg) {
//    std::cout << cfg.host +" - "+ std::to_string(cfg.host.size())+"\n";

    caf::scoped_actor self{system};
    auto truck_actor = system.spawn(truck);
    auto server_actor = system.middleman().spawn_client(TruckClient, cfg.host, cfg.port,truck_actor);
    if (!server_actor) {
        std::cerr << "failed to spawn client: " << to_string(server_actor.error())<< std::endl;
        return;
    }
    
    send_as(*server_actor,truck_actor, initialize_atom_v, "TRUCK");
    send_as(*server_actor,truck_actor, update_port_host_atom_v, cfg.port, cfg.host);
    print_on_exit(*server_actor, "[CLIENT]");
//    print_on_exit(truck_actor, "[TRUCK]");
    
    
}
CAF_MAIN(caf::io::middleman, caf::id_block::truck_block)






caf::behavior server(caf::io::broker* self, const caf::actor& buddy) {
  std::cout << "[SERVER]: running" << std::endl;
  return {
    [=](const caf::io::new_connection_msg& msg) {
      std::cout << "[SERVER]: New Connection_Accepted" << std::endl;
      auto impl = self->fork(TruckServerMaster, msg.handle, std::move(buddy));
      print_on_exit(impl, "[SERVER]");
      self->quit();
    },
  };
}


void run_server(caf::actor_system& system, const config& cfg) {
    
}

