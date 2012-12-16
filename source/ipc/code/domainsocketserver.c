int server_listen(const char *fileName) {
  unlink(fileName);
  int socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
  struct sockaddr_un address;
  memset(&address, 0, sizeof(struct sockaddr_un));
  address.sun_family = AF_UNIX;
  sprintf(address.sun_path, fileName);

  bind(socket_fd, (struct sockaddr*)&address, sizeof(struct sockaddr_un));
  listen(socket_fd, 5);
  int connection_fd;
  socklen_t address_length;
  while((connection_fd = accept(socket_fd, (struct sockaddr*)&address, &address_length)) > (-1)) {
     int child = fork();
     if(child == 0) {
        return connection_handler(connection_fd);
     } else {
        close(connection_fd);
     } 
  }

  close(socket_fd);
  unlink(fileName);
  return 0;
}
