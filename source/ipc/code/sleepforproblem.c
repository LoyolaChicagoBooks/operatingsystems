void sig_int(int signo) {
    volatile int j = 0;
    printf("sig_int enter\n");
    for(int i = 0; i < 10000000; i++) {
        j += i * i;
    }
    printf("sig_int done\n");
}

int main(int argc, char* argv[]) {
    signal(SIGINT, sig_int);
    unsigned int unslept = sleepFor(1);
    printf("sleepFor returned: %u\n", unslept);
    return 0;
}

