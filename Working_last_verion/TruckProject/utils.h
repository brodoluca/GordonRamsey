

#ifndef utils_h
#define utils_h
//
#include "atoms.h"
#include "enums.h"
#include "defines.h"

///This file includes all the atoms, enums and definitions
///also some commodity functions


///this template function is used to write int safely into the netoek
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


/// Utility function for reading an integer from incoming data.
template <class T>
void read_int(const void* data, T& storage) {
  using unsigned_type = typename std::make_unsigned<T>::type;
  memcpy(&storage, data, sizeof(T));
  if constexpr (sizeof(T) > 1)
    storage = static_cast<T>(ntohl(static_cast<unsigned_type>(storage)));
}


/// Utility function to print an exit message with custom name.
inline void print_on_exit(const caf::actor& hdl, const std::string& name) {
    hdl->attach_functor([=](const caf::error& reason) {
        std::cout << "["<< name << "]" <<" exited: " << to_string(reason) << std::endl;
  });
}

///config used at the beginning, but we dont use this anymore
class config : public caf::actor_system_config {
public:
  uint16_t port = 4242;
  std::string host = "localhost";
  bool server_mode = false;

  config() {
    opt_group{custom_options_, "global"}
      .add(port, "port,p", "set port")
      .add(host, "host,H", "set host (ignored in server mode)");
    
  }
};




#endif /* utils_h */
