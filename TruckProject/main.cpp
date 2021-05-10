
#include <iostream>


#include "Truck.hpp"


void caf_main(caf::actor_system& system, const config& cfg) {
    auto t = spawnNewTruck(system,"TRUCK", "localhost",4242);
}

CAF_MAIN(caf::io::middleman, caf::id_block::truck_block)



