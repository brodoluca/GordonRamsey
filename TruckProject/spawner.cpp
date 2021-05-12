//
//  spawner.cpp
//  Truck2
//
//  Created by Luca on 10/05/21.
//

#include "Truck.hpp"
caf::actor spawnNewTruck(caf::actor_system& system,std::string name, std::string host , uint16_t port, uint16_t own_port){
    auto truck_actor = system.spawn(truck);
    auto server_actor = system.middleman().spawn_client(TruckClient, host, port,truck_actor);
    caf::scoped_actor self{system};
    try {
        if (!server_actor) {
            throw "Are you sure the Ip and the port are correct? ";
        }
        print_on_exit(*server_actor, "CLIENT");
        send_as(*server_actor,truck_actor, initialize_atom_v, name, own_port);
        send_as(*server_actor,truck_actor, update_port_host_atom_v, own_port,host);
    } catch (const char* msg) {
        self->send(truck_actor, initialize_atom_v, name, port);
        std::cerr << "failed to spawn "<< name << "'s client: " << to_string(server_actor.error()) <<"\n"<< msg << "\n\n"<< std::endl;
    }
    
    print_on_exit(truck_actor, name);
    
    return truck_actor;
}


caf::actor spawnNewMaster(caf::actor_system& system,std::string name, std::string host , uint16_t port){
    auto truck_actor = system.spawn(truck);
    caf::scoped_actor self{system};
    auto server_actor = system.middleman().spawn_server(temp_master_server,port, truck_actor);
    try {
        if (!server_actor) {
            throw "I can not spawn a new server.";
        }
        print_on_exit(*server_actor, "TEMP_SERVER");
        send_as(*server_actor,truck_actor, update_port_host_atom_v, port,host);
    } catch (const char* msg) {
        self->send_exit(truck_actor, caf::exit_reason::user_shutdown);
        std::cerr << msg << ": "<< to_string(server_actor.error()) <<"\n" << std::endl;
        
    }
    print_on_exit(truck_actor, name);
    
    
    return truck_actor;
    
}
