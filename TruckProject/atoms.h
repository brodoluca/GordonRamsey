//
//  atoms.h
//  Truck2
//
//  Created by Luca on 10/05/21.
//

#ifndef atoms_h
#define atoms_h

CAF_BEGIN_TYPE_ID_BLOCK(truck_block, first_custom_type_id)
CAF_ADD_ATOM(truck_block, initialize_atom)
CAF_ADD_ATOM(truck_block, send_server_atom)
CAF_ADD_ATOM(truck_block, get_new_id_atom)
CAF_ADD_ATOM(truck_block, update_port_host_atom)
CAF_ADD_ATOM(truck_block, become_client_atom)
CAF_ADD_ATOM(truck_block, assign_id_atom)
CAF_ADD_ATOM(truck_block, get_new_command)
CAF_ADD_ATOM(truck_block, become_master_atom)
CAF_ADD_ATOM(truck_block, send_new_command_atom)
CAF_ADD_ATOM(truck_block, set_server_atom)
CAF_ADD_ATOM(truck_block, which_id_atom)
CAF_ADD_ATOM(truck_block, which_front_id_atom)
CAF_ADD_ATOM(truck_block, set_front_id)
CAF_ADD_ATOM(truck_block, get_port_atom)
CAF_ADD_ATOM(truck_block, get_host_atom)
CAF_ADD_ATOM(truck_block, get_host_port_atom)
CAF_ADD_ATOM(truck_block, is_master_atom)
CAF_ADD_ATOM(truck_block, set_master_connection_atom)
CAF_ADD_ATOM(truck_block, update_id_behind_atom)
CAF_ADD_ATOM(truck_block, you_are_master_atom)
CAF_ADD_ATOM(truck_block, tell_back_im_master_atom)
CAF_ADD_ATOM(truck_block, update_truck_behind_port_host_atom)
CAF_ADD_ATOM(truck_block, update_master_atom)
CAF_ADD_ATOM(truck_block, cascade_port_host_atom)

CAF_ADD_ATOM(truck_block, ask_for_input_atom)


CAF_ADD_ATOM(truck_block, initialiaze_truck_platoon_atom)
CAF_ADD_ATOM(truck_block, increment_number_trucks_upwards_atom)
CAF_ADD_ATOM(truck_block, increment_number_trucks_backwards_atom)
CAF_ADD_ATOM(truck_block, increment_number_trucks_atom)
CAF_ADD_ATOM(truck_block, decrease_number_trucks_atom)
CAF_ADD_ATOM(truck_block, get_truck_numbers_atom)
CAF_ADD_ATOM(truck_block, update_truck_numbers_atom)
CAF_ADD_ATOM(truck_block, spawn_server_atom);

CAF_ADD_ATOM(truck_block, update_port_host_previous_atom);
CAF_ADD_ATOM(truck_block, get_port_host_previous_atom);
//CAF_ADD_ATOM(truck_block, get_port_atom);
CAF_ADD_ATOM(truck_block, update_back_up_atom);
CAF_ADD_ATOM(truck_block, get_port_host_back_up_atom);

CAF_ADD_ATOM(truck_block, truck_left_or_dead_atom);

CAF_ADD_ATOM(truck_block, fork_to_master_atom);

CAF_ADD_ATOM(truck_block, add_connection_atom);


CAF_ADD_TYPE_ID(truck_block, (std::pair<int32_t, std::string>) )

CAF_END_TYPE_ID_BLOCK(truck_block)

template <class Inspector>
bool inspect(Inspector& f, std::pair<int32_t, std::string>& x) {
    return f.object(x).fields(f.field("int32_t", x.first), f.field("string", x.second));
}


#endif /* atoms_h */
