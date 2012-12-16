typdef struct myArg {
    int id
} myArg_t;

door_arg_t d_arg;
int doorfd = open("/tmp/door", O_RDONLY);
myArg_t* arg = (myArg_t*)malloc(sizeof(myArg_t));
arg->id = 12;
d_arg.data_ptr = (char*)arg;
d_arg.data_size = sizeof(*arg);
d_arg.desc_ptr = NULL;
d_arg.desc_num = 0;
door_call(doorfd, &d_arg);
