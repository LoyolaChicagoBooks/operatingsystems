static void handler(int);

int main(int argv, char* argv[]) {
  signal(SIGUSR1, handler);
  signal(SIGINT, handler);
  while(1) { pause(); }
}

static void handler(int signalNum) {
  if(signalNum == SIGUSR1) {
    printf("received SIGUSR1\n");
  } else if(signalNum == SIGINT) {
    printf("received SIGHUP\n");
  }
}
