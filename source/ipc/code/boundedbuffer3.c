Message *Message::GetFromMemoryMappedFile(int fd) {
  void *map = mmap(
     NULL, sizeof(Message), 
     (PROT_READ|PROT_WRITE), MAP_SHARED, fd, 0);
  if(map == (void*)(-1)) {
     fprintf(stderr, "mmap() returned -1\n");
  }
  Message* msg = (Message*)map;
  return msg;
}

void Message::ReleaseFile(Message *msg, int fd) {
  if(munmap((void*)msg, sizeof(Message)) == (-1)) {
     fprintf(stderr, "munmap() failed\n");
  }
}

