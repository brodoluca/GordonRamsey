#include <iostream>


#include "Truck.hpp"















void caf_main(caf::actor_system& system, const config& cfg) {

//    auto t = spawnNewTruck(system,"TRUCK", "localhost",4242,3232);
    
    
    
    auto t = spawnNewMaster(system, "TRUCK", "localhost", 3232);
    auto server_actor = system.middleman().spawn_server(server, cfg.port,std::move(t));


    std::cout << "*** listening on port " << cfg.port << std::endl;
    std:: cout << "*** to quit the program, simply press <enter>" << std::endl;
      
   

}
CAF_MAIN(caf::io::middleman, caf::id_block::truck_block)



