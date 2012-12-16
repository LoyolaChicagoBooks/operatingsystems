void Message::EnqueueMessage(const char *msg) {
  sem_wait(&_full);
  sem_wait(&_lock);
  _current += 1;
  bzero(&_messages[_current], MaxMessageSize*sizeof(char));
  memcpy(&_messages[_current], msg, strlen(msg)*sizeof(char));
  sem_post(&_lock);
  sem_post(&_empty);
}

char* Message::DequeueMessage() {
  char *msg = new char[MaxMessageSize];
  sem_wait(&_empty);
  sem_wait(&_lock);
  memcpy(msg, &_messages[_current], MaxMessageSize*sizeof(char));
  _current -= 1;
  sem_post(&_lock);
  sem_post(&_full);
  return msg;
}

