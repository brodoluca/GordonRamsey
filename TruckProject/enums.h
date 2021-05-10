//
//  enums.h
//  Truck2
//
//  Created by Luca on 10/05/21.
//

#ifndef enums_h
#define enums_h

enum class operations : uint8_t {
    master = 5,
    assign_id,
    get_id,
    command,
    front_id,
    update_truck_behind,
    get_port_host,
    update_id_behind,
    ready,
    cascade_port_host,
    update_number_trucks,
    initialiaze_truck_platoon,
    try_luca,
    update_number_trucks_from_client
};

enum class commands : uint32_t{
    stop,
    accellerate,
    decellerate,
    start
};

#endif /* enums_h */
