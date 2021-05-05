//
//  utils.h
//  TruckProject
//
//  Created by Luca on 22/04/21.
//

#ifndef utils_h
#define utils_h
//

template <class T>
void write_int(caf::io::broker* self, caf::io::connection_handle hdl, T value) {
  using unsigned_type = typename std::make_unsigned<T>::type;
  if constexpr (sizeof(T) > 1) {
    auto cpy = static_cast<T>(htonl(static_cast<unsigned_type>(value)));
    self->write(hdl, sizeof(T), &cpy);
  } else {
    self->write(hdl, sizeof(T), &value);
  }
}


// Utility function for reading an integer from incoming data.
template <class T>
void read_int(const void* data, T& storage) {
  using unsigned_type = typename std::make_unsigned<T>::type;
  memcpy(&storage, data, sizeof(T));
  if constexpr (sizeof(T) > 1)
    storage = static_cast<T>(ntohl(static_cast<unsigned_type>(storage)));
}


// Utility function to print an exit message with custom name.
inline void print_on_exit(const caf::actor& hdl, const std::string& name) {
    hdl->attach_functor([=](const caf::error& reason) {
        std::cout << "["<< name << "]" <<" exited: " << to_string(reason) << std::endl;
  });
}

constexpr char http_valid_get[] = "GET / HTTP/1.1";

constexpr char http_get[] = "GET / HTTP/1.1\r\n"
                            "Host: localhost\r\n"
                            "Connection: close\r\n"
                            "Accept: text/plain\r\n"
                            "User-Agent: CAF/0.14\r\n"
                            "Accept-Language: en-US\r\n"
                            "\r\n";

constexpr char http_ok[] = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/plain\r\n"
                           "Connection: close\r\n"
                           "Transfer-Encoding: chunked\r\n"
                           "\r\n"
                           "d\r\n"
                           "Hi there! :)\r\n"
                           "\r\n"
                           "0\r\n"
                           "\r\n"
                           "\r\n";

constexpr char http_error[] = "HTTP/1.1 404 Not Found\r\n"
                              "Connection: close\r\n"
                              "\r\n";

constexpr char newline[2] = {'\r', '\n'};


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

CAF_ADD_ATOM(truck_block, ask_for_input_atom)
CAF_ADD_ATOM(truck_block, update_number_trucks_atom)
CAF_ADD_TYPE_ID(truck_block, (std::pair<int32_t, std::string>) )

CAF_END_TYPE_ID_BLOCK(truck_block)
template <class Inspector>
bool inspect(Inspector& f, std::pair<int32_t, std::string>& x) {
    return f.object(x).fields(f.field("int32_t", x.first), f.field("string", x.second));
}

enum class operations : uint8_t {
    master = 5,
    assign_id,
    get_id,
    command,
    front_id,
    update_truck_behind,
    get_port_host,
    update_id_behind,
    ready
};


enum class commands : uint32_t{
    stop,
    accellerate,
    decellerate,
    start
};

#endif /* utils_h */
