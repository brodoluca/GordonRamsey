
#include <iostream>


#include "Truck.hpp"


void caf_main(caf::actor_system& system, const config& cfg) {
    auto t = spawnNewTruck(system,"TRUCK", "localhost",4242,3232);
    int a = 0;
    [&a](const int &b){
        a = b;
    }(4);
    
    std::cout << a << std::endl ;
}


CAF_MAIN(caf::io::middleman, caf::id_block::truck_block)



