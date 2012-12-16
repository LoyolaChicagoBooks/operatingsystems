int connection_handler(int socket_fd) {
  char buff[256];
  int nBytes = read(socket_fd, buff, 256);
  buff[nBytes] = 0;
  printf("message from client: %s\n", buff);
  nBytes = snprintf(buff, 256, "hello from server");
  write(socket_fd, buff, nBytes);

  close(socket_fd);
  return 0;
}

