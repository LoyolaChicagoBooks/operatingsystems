int main(argc, char* argv[]) {
    struct stat fileStat;
    stat("shared.dat", &fileStat);
    int fileSize = fileStat.st_size;

    int fd = open("shared.dat", O_RDWR);

    void* map = mmap(NULL, fileSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    char* data = (char*)calloc(1, fileSize);

    memcpy(data, map, fileSize);

    printf("%s\n", data);

    free(data);
    munmap(map);
    close(fd);

    return 0;
}

