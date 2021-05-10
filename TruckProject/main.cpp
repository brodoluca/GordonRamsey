//
//  main.cpp
//  Testing_brokers
//
//  Created by Luca on 21/04/21.
//
#include <iostream>


#include "Truck.hpp"





void caf_main(caf::actor_system& system, const config& cfg) {

    caf::scoped_actor self{system};
    

    
    
    
    auto t = spawnNewTruck(system,"TRUCK", "localhost",4242);

    
}
CAF_MAIN(caf::io::middleman, caf::id_block::truck_block)



