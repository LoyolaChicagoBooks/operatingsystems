Message::Message() {
  sem_init(&_lock,  1, 1);                                //passing 1 as the 2nd parameter allows the
  sem_init(&_empty, 1, 0);                              //semaphore work in a memory mapped region
  sem_init(&_full,  1, MessageQueueSize);
  _current = 0;
}
Message::~Message() { }
Message *Message::CopyToMemoryMappedFile(int fd) {
  int datasize = sizeof(Message);
  printf("message size = %d\n", datasize);
  if(lseek(fd, sizeof(Message), SEEK_SET) == (-1)) {
     fprintf(stderr, "error in lseek\n");
  }
  int dummyVal = 0;
  if(write(fd, (char*)&dummyVal, sizeof(char)) == (-1)) {
     fprintf(stderr, "error in write\n");
  }
  void *map = mmap(NULL, sizeof(Message), (PROT_READ|PROT_WRITE), MAP_SHARED, fd, 0);
  if(map == (void*)(-1)) {
     fprintf(stderr, "mmap() returned -1\n");
  }
  Message *msg = new Message();
  memcpy(map, (void*)msg, sizeof(Message));
  delete msg;
  return (Message*)map;
}

