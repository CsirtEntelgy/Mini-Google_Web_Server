/*
 * Copyright ©2023 Justin Hsia.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Winter Quarter 2023 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include <stdio.h>       // for snprintf()
#include <unistd.h>      // for close(), fcntl()
#include <sys/types.h>   // for socket(), getaddrinfo(), etc.
#include <sys/socket.h>  // for socket(), getaddrinfo(), etc.
#include <arpa/inet.h>   // for inet_ntop()
#include <netdb.h>       // for getaddrinfo()
#include <errno.h>       // for errno, used by strerror()
#include <string.h>      // for memset, strerror()
#include <iostream>      // for std::cerr, etc.

#include "./ServerSocket.h"

static const int kHostName = 1024;

extern "C" {
  #include "libhw1/CSE333.h"
}

namespace hw4 {

ServerSocket::ServerSocket(uint16_t port) {
  port_ = port;
  listen_sock_fd_ = -1;
}

ServerSocket::~ServerSocket() {
  // Close the listening socket if it's not zero.  The rest of this
  // class will make sure to zero out the socket if it is closed
  // elsewhere.
  if (listen_sock_fd_ != -1)
    close(listen_sock_fd_);
  listen_sock_fd_ = -1;
}

bool ServerSocket::BindAndListen(int ai_family, int* const listen_fd) {
  // Use "getaddrinfo," "socket," "bind," and "listen" to
  // create a listening socket on port port_.  Return the
  // listening socket through the output parameter "listen_fd"
  // and set the ServerSocket data member "listen_sock_fd_"

  // STEP 1:
  sock_family_ = ai_family;

  // below codes borrow heavily from lecture code examples

  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET6;       // IPv6 (also handles IPv4 clients)
  hints.ai_socktype = SOCK_STREAM;  // stream
  hints.ai_flags = AI_PASSIVE;      // use wildcard "in6addr_any" address
  hints.ai_flags |= AI_V4MAPPED;    // use v4-mapped v6 if no v6 found
  hints.ai_protocol = IPPROTO_TCP;  // tcp protocol
  hints.ai_canonname = nullptr;
  hints.ai_addr = nullptr;
  hints.ai_next = nullptr;

  // find out the IP addr and port
  struct addrinfo *result;
  std::string port = std::to_string(port_);
  int res = getaddrinfo(nullptr, port.c_str(), &hints, &result);
  // don't use cerr. just use assertion according to specs.
  Verify333(res == 0);

  // Loop through the returned address structures until we are able
  // to create a socket and bind to one.  The address structures are
  // linked in a list through the "ai_next" field of result.
  int listen_fd_val = -1;
  for (struct addrinfo *rp = result; rp != nullptr; rp = rp->ai_next) {
    listen_fd_val = socket(rp->ai_family,
                           rp->ai_socktype,
                           rp->ai_protocol);
    if (listen_fd_val == -1) {
      // Creating this socket failed.  So, loop to the next returned
      // result and try again.
      std::cerr << "socket() failed: " << strerror(errno) << std::endl;
      listen_fd_val = -1;
      continue;
    }

    // Configure the socket; we're setting a socket "option."  In
    // particular, we set "SO_REUSEADDR", which tells the TCP stack
    // so make the port we bind to available again as soon as we
    // exit, rather than waiting for a few tens of seconds to recycle it.
    int optval = 1;
    setsockopt(listen_fd_val, SOL_SOCKET, SO_REUSEADDR,
               &optval, sizeof(optval));

    // Try binding the socket to the address and port number returned
    // by getaddrinfo().
    if (bind(listen_fd_val, rp->ai_addr, rp->ai_addrlen) == 0) {
      // Bind worked!
      break;
    }

    // The bind failed.  Close the socket, then loop back around and
    // try the next address/port returned by getaddrinfo().
    close(listen_fd_val);
    listen_fd_val = -1;
  }

  // Free the structure returned by getaddrinfo().
  freeaddrinfo(result);

  // If we failed to bind, return false.
  if (listen_fd_val <= 0)
    return false;

  // Success. Tell the OS that we want this to be a listening socket.
  if (listen(listen_fd_val, SOMAXCONN) != 0) {
    std::cerr << "Failed to mark socket as listening: ";
    std::cerr << strerror(errno) << std::endl;
    close(listen_fd_val);
    return false;
  }

  *listen_fd = listen_fd_val;  // update output parameter
  listen_sock_fd_ = listen_fd_val;

  return true;
}

bool ServerSocket::Accept(int* const accepted_fd,
                          std::string* const client_addr,
                          uint16_t* const client_port,
                          std::string* const client_dns_name,
                          std::string* const server_addr,
                          std::string* const server_dns_name) const {
  // Accept a new connection on the listening socket listen_sock_fd_.
  // (Block until a new connection arrives.)  Return the newly accepted
  // socket, as well as information about both ends of the new connection,
  // through the various output parameters.

  // STEP 2:
  // Wait for a client to arrive.
  while (true) {
    struct sockaddr_storage caddr;
    socklen_t caddr_len = sizeof(caddr);
    struct sockaddr *addr = reinterpret_cast<struct sockaddr *>(&caddr);
    int client_fd = accept(listen_sock_fd_, addr, &caddr_len);
    if (client_fd < 0) {
      if ((errno == EAGAIN) || (errno == EINTR))
        continue;
      std::cerr << "Failure on accept: " << strerror(errno) << std::endl;
    }

    // update out param for client fd
    *accepted_fd = client_fd;

    if (addr->sa_family == AF_INET) {
      // IPV4 address and port
      char addr_string[INET_ADDRSTRLEN];
      struct sockaddr_in *in4 = reinterpret_cast<struct sockaddr_in *>(&addr);
      inet_ntop(AF_INET, &(in4->sin_addr), addr_string, INET_ADDRSTRLEN);
      *client_port = ntohs(in4->sin_port);
      *client_addr = std::string(addr_string);
    } else {
      // IPV6 address and port
      char addr_string[INET6_ADDRSTRLEN];
      struct sockaddr_in6 *in6 = reinterpret_cast<struct sockaddr_in6 *>(addr);
      inet_ntop(AF_INET6, &(in6->sin6_addr), addr_string, INET6_ADDRSTRLEN);
      *client_port = ntohs(in6->sin6_port);
      *client_addr = std::string(addr_string);
    }

    char host_name[kHostName];  // big enough.
    if (getnameinfo(addr, sizeof(caddr), host_name,
                    kHostName, nullptr, 0, 0) != 0) {
      std::cerr << host_name << " reverse DNS failed" << std::endl;
    }
    *client_dns_name = std::string(host_name);

    char h_name[kHostName];
    h_name[0] = '\0';

    if (sock_family_ == AF_INET) {
      // The server is using IPv4 address.
      struct sockaddr_in srvr;
      socklen_t srvrlen = sizeof(srvr);
      char addr_buf[INET_ADDRSTRLEN];

      getsockname(client_fd, (struct sockaddr *) &srvr, &srvrlen);
      inet_ntop(AF_INET, &srvr.sin_addr, addr_buf, INET_ADDRSTRLEN);
      *server_addr = std::string(addr_buf);

      // Get the server's dns name, or return it's IP address as
      // an alternative/substitute if the dns lookup fails.
      getnameinfo((const struct sockaddr *) &srvr, srvrlen,
                  h_name, 1024, nullptr, 0, 0);
      *server_dns_name = std::string(h_name);
    } else {
      // The server is using an IPv6 address.
      struct sockaddr_in6 srvr;
      socklen_t srvrlen = sizeof(srvr);
      char addr_buf[INET6_ADDRSTRLEN];

      getsockname(client_fd, (struct sockaddr *) &srvr, &srvrlen);
      inet_ntop(AF_INET6, &srvr.sin6_addr, addr_buf, INET6_ADDRSTRLEN);
      *server_addr = std::string(addr_buf);

      // Get the server's dns name, or return it's IP address as
      // a alternative/substitute if the dns lookup fails.
      getnameinfo((const struct sockaddr *) &srvr, srvrlen,
                  h_name, 1024, nullptr, 0, 0);
      *server_dns_name = std::string(h_name);
    }
    break;
  }

  return true;
}

}  // namespace hw4
