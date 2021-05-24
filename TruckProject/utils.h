

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

///factors the host and the port to be sent to the buddy
///
///@param
/// ip ->string to be sent to buddy
/// cstr - buffer containing the string received from the broker
/// buddy - truck  which we are associated to

inline void factorHostPort(std::string &ip, char* cstr){
//                    convert it to a c++ string
    ip = cstr;
//                    set the memory of the copy buffer to 0
    memset(cstr, '0', sizeof(cstr));
//                    remove the first 3 chars of the string (There is always some garbage in the buffer)
    ip.erase(ip.begin(), ip.begin()+3);
//                    save the 3 char at the end of the string (that's where we save the stop ID)
    cstr[21] = ip.back();
    ip.pop_back();
    cstr[20] = ip.back();
    ip.pop_back();
    cstr[19] = ip.back();
    ip.pop_back();
//                    convert it to an in
//    *tqStopId =atoi(cstr);
//                    remove the '-' char from the string (place holders)
    while(ip.back() == '-') ip.pop_back();
}



template <size_t Size>
constexpr size_t cstr_size(const char (&)[Size]) {
  return Size;
}

#endif /* utils_h */
