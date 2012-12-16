void server(void* cookie, char* argp, size_t arg_size, door_desc_t* dp, uint_t n_desc) {

    /*server code goes here*/

}

int doorfd = door_create(server, 0, 0);
int fd = creat("/tmp/door", 0666);
fdetach("/tmp/door");
fattach(doorfd,"/tmp/door");
pause();

