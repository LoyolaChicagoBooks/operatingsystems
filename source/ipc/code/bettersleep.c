jmpbuf env_alrm;

void sig_alrm(int signo) {
    longjmp(env_alrm, 1);
}

unsinged int sleepFor(int numSeconds) {
    signal(SIGALRM, sig_alrm);
    if(setjmp(env_alrm) == 0) {
        alarm(nsecs);
        pause();
    }
    return alarm(0);
}
