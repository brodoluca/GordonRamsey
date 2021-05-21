
#ifndef atoms_h
#define atoms_h

///These atoms are used to decide the behaviors of the various entity.
///Keep in mind most of them are USELESS, but I want to keep them here
///THese are empty structs, should not be a big overhead.
///Also, im too lazy to go and check which is usefull and which is not


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
CAF_ADD_ATOM(truck_block, update_id_cascade)
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
CAF_ADD_ATOM(truck_block, update_back_up_atom);
CAF_ADD_ATOM(truck_block, get_port_host_back_up_atom);
CAF_ADD_ATOM(truck_block, truck_left_or_dead_atom);
CAF_ADD_ATOM(truck_block, fork_to_master_atom);
CAF_ADD_ATOM(truck_block, add_connection_atom);
CAF_ADD_ATOM(truck_block, request_for_port_host_atom);
CAF_ADD_ATOM(truck_block, close_connection_atom);
CAF_ADD_ATOM(truck_block, close_client_connection_atom);
CAF_ADD_ATOM(truck_block, allow_switcheroo_atom);
CAF_ADD_ATOM(truck_block, create_new_client_connection_previous_hostport_atom)
CAF_ADD_ATOM(truck_block, somebody_connected_atom);
CAF_ADD_ATOM(truck_block, count_trucks_atom)
CAF_ADD_ATOM(truck_block, master_connect_to_my_server_atom);
CAF_ADD_ATOM(truck_block, master_connects_to_truck_behind);
CAF_ADD_ATOM(truck_block, set_client_atom);
CAF_ADD_ATOM(truck_block, try_luca_atom);
CAF_ADD_ATOM(truck_block, is_switcheroo_atom);
CAF_ADD_ATOM(truck_block, set_switcheroo_atom);
CAF_ADD_ATOM(truck_block, available_to_count);
CAF_ADD_ATOM(truck_block, get_speed_atom);
CAF_ADD_ATOM(truck_block, set_speed_atom);
CAF_ADD_ATOM(truck_block, should_master_count_atom);

CAF_ADD_ATOM(truck_block, start_election_token);
CAF_ADD_ATOM(truck_block, election_in_progress_token)



CAF_ADD_TYPE_ID(truck_block, (std::pair<int32_t, std::string>) )
CAF_ADD_TYPE_ID(truck_block, (std::pair<uint32_t, uint32_t>))

CAF_END_TYPE_ID_BLOCK(truck_block)



///Inspectors required by caf for communicating using user defined data types
template <class Inspector>
bool inspect(Inspector& f, std::pair<int32_t, std::string>& x) {
    return f.object(x).fields(f.field("int32_t", x.first), f.field("string", x.second));
}
template <class Inspector>
bool inspect(Inspector& f, std::pair<uint32_t, uint32_t>& x) {
    return f.object(x).fields(f.field("uint32_t", x.first), f.field("uint32_t", x.second));
}

#endif /* atoms_h */
