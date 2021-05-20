//
//  enums.h
//  Truck2
//
//  Created by Luca on 10/05/21.
//

#ifndef enums_h
#define enums_h


///This enums class is used to differentiato operations when two brokers are connected to each other
///basically, you can think at them like you would with atoms
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
    update_number_trucks_from_client,
    update_port_host_previous,
    update_port_host_back_up,
    request_for_host_port,
    connect_new_server,
    master_connect_to_last_truck,
    decrease_number_trucks,
    count_trucks,
    update_master_previous_host_port,
    set_speed
};
///These are the commands a truck can interprates. 
enum class commands : uint32_t{
    stop,
    accellerate,
    decellerate,
    start
};


#endif /* enums_h */
