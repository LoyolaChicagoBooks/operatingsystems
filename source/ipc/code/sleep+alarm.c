    void sig_alrm(int signo) 
    {
       /* ... */
    }

    unsigned int sleepFor(int numSeconds) {
       signal(SIGALRM, sig_alrm);    //set signal handler
       alarm(numSeconds);            //set alarm for n-seconds
       pause();                      //wait for signal
       return alarm(0);              //turn off alarm
       handler;
    }
