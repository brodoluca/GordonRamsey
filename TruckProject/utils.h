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
#endif /* utils_h */
