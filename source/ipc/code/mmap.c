int main(int argc, char* argv[]) {
    const char *data = "Hello World";
    const int dummyValue = 0;
    int dataSize = sizeof(char) * strlen(data) + sizeof(char);

    int fd = open("shared.dat", O_CREAT|O_TRUNC|O_RDWR, 0666);
    lseek(fd, dataSize, SEEK_SET);
    write(fd, (char*)&dummyValue, sizeof(char));
    
    void* map = mmap(NULL,dataSize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    memcpy(map, data, dataSize);

    getchar();

    munmap(map, dataSize);
    close(fd);

    return 0;
}

