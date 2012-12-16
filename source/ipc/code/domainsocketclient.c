int client_connect(const char* fileName) {
  int socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);

  struct sockaddr_un address;
  memset(&address, 0, sizeof(struct sockaddr_un));
  address.sun_family = AF_UNIX;
  sprintf(address.sun_path, fileName);

  connect(socket_fd, (struct sockaddr*)&address, sizeof(struct sockaddr_un));
  char buffer[256];
  int nBytes = snprintf(buffer, 256, "hello from a client");
  write(socket_fd, buffer, nBytes);

  nBytes = read(socket_fd, buffer, 256);
  buffer[nBytes] = 0;

  printf("message from server: %s\n", buffer);

  close(socket_fd);

  return 0;
}

