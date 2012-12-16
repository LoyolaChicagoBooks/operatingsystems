Storage and Filesystems
=======================


Storage Devices
---------------

-  Types of permanent devices:

   -  Magnetic - hard disk, tape, floppy disk

   -  Optical - CD/DVD/Blu-Ray, Laser Disc, Paper, Punch Cards, Photo
      Film

   -  Solid State - CMOS, NAND based flash, battery backed dynamic
      memory

-  Types of transient devices:

   -  RAM, Processor Caches

IDE/ATA
-------

-  Created by Western Digital Corp in 1986. Reused an existing disk
   interface.

-  Improved greatly over older interfaces due to moving the
   responsibility of managing the disk arm to the hard disk itself. This
   allowed for a greater variation in disk and motor types regardless of
   motherboard support.

-  Connector uses 40 pins.

-  Data bus width is 16 bits.

-  Transfer rates are 16, 33, 66, 100, and 133 MB/s

-  Presents to the computer a set of 512-byte blocks.

ATAPI
-----

-  Improved upon ATA by adding extra commands (such as ’eject’) to
   support other devices.

-  These improvements allowed CD, DVD, and Zip drives to conform to ATA

-  Also helped improve performance by introducing DMA. DMA allowed for
   data transfers to be sent directly to physical memory without
   interrupting the CPU for every bus operation

SATA - Serial ATA
-----------------

-  Helped to reduce cost by reducing the number of pins.

-  Allows hot-swapping or replacing of devices without powering down the
   main board.

-  Supports the full ATAPI command interface

-  Supports transfer rates of 1.5G/s (1.0), 3G/S (2.0), and 6G/s (3.0)

-  The main board hosting SATA typically implement an AHCI controller
   (created by Intel). It supports hotplug capabilities and native
   command queuing. Main boards that do not implement AHCI will
   typically communicate with the SATA device using the ATAPI command
   set.

-  SATA is best supported in Linux 2.6.20 or newer, Windows Vista or 7,
   and more recent versions of OSX. Older operating systems will often
   revert to ATAPI even with an AHCI controller

Hard Disk Geometry - CHS
------------------------

-  Composed of Cylinders, Heads, and Sectors

-  Sector - The unit of storage on a disc. Typically 512, 1024, or 2048
   bytes.

-  Cylinder - A physical, circular disc. Aka "platter". One or more may
   be present in a device.

-  Heads - Refers to either the top or bottom sides of a Cylinder.

-  The CHS address on a disk is a tuple of the {Cylinder, Head, Sector}

Hard Disk Geometry - CHS
------------------------

    .. figure:: storage/diagrams/disk_diagram.*
       :align: center
       :alt: image

       image

Hard Disk Geometry - LBA
------------------------

-  A CHS tuple can be converted to an LBA address with the following
   formula: :math:`$A = (C * N_h + H) * N_s + (S - 1)$`

-  :math:`$N_h$` is the total number of heads

-  :math:`$N_s$` is the total number of sectors

Storage and Failure
-------------------

-  An excellent paper about hard disk failure rates at Google is
   available here:

-  http://labs.google.com/papers/disk_failures.pdf

-  At Google, the failure rates of disks of a given age are:

   -  3-months - 2.5

   -  6-months - 2.0

   -  1-year - 2

   -  2-year - 8

   -  3-year - 8.5

Maximizing Availability - RAID
------------------------------

-  To prevent the loss of availability of data, the use of RAID
   (Redundant Array of Inexpensive Disks) allows for redundant copies of
   data to be stored.

-  Common RAID levels are:

   -  RAID 0 - splits data across disks. Increases disk space and
      provides no redundancy. 2 or more disks are needed.

   -  RAID 1 - creates an exact copy of data on two or more disks.

   -  RAID 5/6 - splits data across disks. Uses one or more disks for
      parity. This allows 1-K out of N disks to fail and allow the data
      of any lost disk to be recovered. 3 or more disks are needed.

RAID
----

-  RAID has three common implementation approaches:

   -  Complete hardware implementation - a disk controller or expansion
      card implements RAID. Several disks are connected to this
      controller and it is presented to the operating system as a single
      storage device. Often have reliability guarantees.

   -  Partial hardware implementation - Same as the complete hardware
      implementation, except parity calculations, and buffering are
      delegated to the host CPU and memory. Don’t often have reliability
      guarantees.

   -  Software implementation - The operating system itself manages
      several disks and presents to the file-system layer a single
      storage device.

RAID - 0
--------

    .. figure:: storage/diagrams/raid_0.*
       :align: center
       :alt: image

       image

RAID - 1
--------

    .. figure:: storage/diagrams/raid_1.*
       :align: center
       :alt: image

       image

RAID - 5
--------

    .. figure:: storage/diagrams/raid_5.*
       :align: center
       :alt: image

       image

RAID - 5 Parity
---------------

{language=C, basicstyle=, indent=xleftmargin}

::

    size_t parityWrite(
            int fd0, int fd1, int fd2, 
            const void *buf0, const void *buf1, 
            size_t count) {
            for(size_t i = 0; i < count; i++) {
                    char byte0 = ((char*)buf0)[i];
                    char byte1 = ((char*)buf1)[i];
                    char parity = byte0 ^ byte1;
                    write(fd0, &byte0, sizeof(char));
                    write(fd1, &byte1, sizeof(char));
                    write(fd2, &parity, sizeof(char));
            }
            return count;
    }

RAID - 5 Parity
---------------

{language=C, basicstyle=, indent=xleftmargin}

::

    size_t parityRead(int fd0, int fd1, void *buf, size_t count) {
            char *buff0 = (char*)malloc(count);
            char *buff1 = (char*)malloc(count);
            char *buff = (char*)buf;
            read(fd0, buff0, count);
            read(fd1, buff1, count);
            for(size_t i = 0; i < count; i++) {
                    buff[i] = buff0[i] ^ buff1[i];
            }
            return count;
    }

RAID - 5 Parity
---------------

{language=C, basicstyle=, indent=xleftmargin}

::

    int main(int argc, char** argv) {
            int fd0 = open("f0", O_CREAT|O_TRUNC|O_RDWR, 0666);
            int fd1 = open("f1", O_CREAT|O_TRUNC|O_RDWR, 0666);
            int fd2 = open("f2", O_CREAT|O_TRUNC|O_RDWR, 0666);

            const char* msg0 = "hello world\n";
            const char* msg1 = "testing 123\n";

            parityWrite(fd0,fd1,fd2,msg0,msg1,strlen(msg0)+1);
           
            close(fd0);
            close(fd1);
            close(fd2);

            unlink("f1");

RAID - 5 Parity
---------------

{language=C, basicstyle=, indent=xleftmargin}

::

            fd0 = open("f0", O_RDWR, 0666);
            fd2 = open("f2", O_RDWR, 0666);

            size_t msgSize = sizeof(char)*strlen(msg0)+1;
            char *buff = (char*)malloc(msgSize);

            parityRead(fd0, fd2, buff, msgSize);

            printf("f1 contents are = %s\n", buff);

            close(fd0);
            close(fd2);

            free(buff);
            unlink("f0");
            unlink("f2");
            return 0;
    }

RAID 5 - Parity and Recovery
----------------------------

-  In the above example, three files are created, f0, f1, and f2

-  Two different messages are written to each of f0 and f1. Parity data
   is written to f2.

-  Failure scenarios:

   -  f0 is deleted - f0 can be recovered from f1 and the parity data of
      f2

   -  f1 is deleted - f1 can be recovered from f0 and the parity data of
      f2

   -  f2 is deleted - f2 can be recovered by recalculating the parity
      between f0 and f1

-  In each case, the loss of one storage medium causes no data loss.

Disk Partitioning
-----------------

-  Operating systems divide disks into partitions (or slices).

-  Partitions are a useful concept in that they allow an operating
   system to divide parts of the disk into different types of disk
   formats. Among these will be a filesystem implementation, in some
   cases a swap partition, and filesystems not managed by the operating
   system (such as in a dual boot configuration).

Disk Partitioning
-----------------

-  On the PC, the most common format for partitions is the MBR (master
   boot record) scheme. The MBR scheme allows a disk to be divided into
   up to four partitions. The offsets and sizes of these partitions are
   located in the MBR record at the beginning of the disk.

-  To raise the limitation on four partitions, the MBR schema allows for
   one partition to be considered an "extended" partition which can be
   further subdivided into multiple "logical" partitions.

Disk Arms / Heads
-----------------

-  For mechanical disks, there are two moving parts:

   -  The head - driven by a stepper motor. Moves to the correct track
      on the platter

   -  The disk motor - spins the platters under the head.

-  For a sector to be read from or written to a hard disk, the platter
   must be rotated to the correct position and the head must be moved to
   the right position to perform the operation.

-  So for a given operation, there is a concept of a physical distance
   the device must ’seek’ to perform the operation.

-  The performance of disk operations are governed by:

   -  The speed of the disk and head motors

   -  The algorithm that orders one or more possible disk operations.

Hard Disk Geometry - CHS
------------------------

    .. figure:: storage/diagrams/disk_diagram.*
       :align: center
       :alt: image

       image

Characteristics of a Good Disk Scheduling Algorithm
---------------------------------------------------

-  Like a process scheduler, decisions involve avoiding starvation and
   addressing both latency and throughput.

-  Goals of a good algorithm:

   -  Should avoid moving the head if another request exists in the same
      track.

   -  Should minimize moving the head overall.

   -  Should minimize the average distance from the current position of
      the head and the next request at any given time.

   -  When ordering requests, an individual request should not be
      delayed for too long.

Disk Scheduling Algorithms
--------------------------

-  FIFO

-  Shortest Seek First

-  Elevator

-  FSCAN

FIFO
----

-  FIFO is the simplest disk scheduling algorithm

-  FIFO simply serves the requests in order as they arrive.

-  FIFO is far from optimal. It does not make any attempt to minimize
   the seeking of the head.

-  FIFO does guarantee fairness. Requests are answered in the order that
   they are received.

Shortest Seek First
-------------------

-  Shortest Seek First scans the request queue for the request that is
   nearest the head and serves that request first.

-  This algorithm minimizes the total seeking that the head must perform

-  This algorithm can allow requests to starve. If new requests keep
   coming in that are near the current position of the head at a
   sufficient rate, the disk head will never move near enough to other
   requests to service them.

Shortest Seek First
-------------------

    .. figure:: storage/diagrams/shortest_seek_first.*
       :align: center
       :alt: image

       image

Elevator Algorithm
------------------

-  A good way to visualize a disk scheduling algorithm is to think of
   how to make the operation of an elevator in a building that has
   several floors optimal.

-  To guarantee that every floor is visited and no one is kept waiting
   forever, the elevator algorithm’s rule is that the elevator should go
   all the way to the top floor before reversing direction to go back to
   the bottom floor.

-  This means that the algorithm has a concept of a direction. Given a
   list of requests, the requests that will be served are those that are
   in the current track or those that are in the direction of the head’s
   movement (in order).

-  Once the head reaches the final track, the direction is reversed and
   the algorithm is repeated.

Elevator Algorithm
------------------

    .. figure:: storage/diagrams/elevator.*
       :align: center
       :alt: image

       image

Evaluation of the Elevator Algorithm
------------------------------------

-  Pros:

   -  Prevents starvation of requests

-  Cons:

   -  Sectors in the middle of the disk are serviced faster on average
      because their average distance from the head is the least.

-  The elevator algorithm can remove the imbalance by starting at the
   innermost track, seeking to the outermost and then returning to the
   innermost. This way the direction never changes. Unfortunately, this
   particular seek takes more time than others due to the greater
   distance.

FSCAN
-----

-  FSCAN works by taking the existing set of requests and putting them
   in one queue

-  All new requests received while completing work in the first queue
   are put into a second queue.

-  FSCAN then services the items in the first queue by serving requests
   nearest the head to the requests furthest from the head in order

-  Once the first queue is empty, items from the second queue are moved
   to the first queue and the algorithm repeats

-  FSCAN guarantees that there will be no starvation because there will
   be at most a fixed set of N items that need to be served before any
   given item is served.

Implementing Files and Folders
------------------------------

Implementing Files and Folders
------------------------------

-  How files and folders are implemented in a storage medium can greatly
   depend upon the physical characteristics and capabilities of that
   medium.

-  For example, on tape-drives, CD/DVD/Blu-Ray, or write-once media,
   files and folders are stored contiguously with no fragmentation. All
   of the information about the filesystem can be held in a TOC (Table
   Of Contents).

-  For filesystems with files that have a finite lifetime, such as on
   flash media, hard disks, SSDs, and others, the layout of files and
   folders must be maintained in a more complex way.

-  Among these more advanced methods are linked lists and i-nodes.

-  To manage free-space, objects like bit-maps and linked lists are
   possibilities.

Linked-Lists
------------

    .. figure:: storage/diagrams/linked_list_files.*
       :align: center
       :alt: image

       image

Linked-Lists
------------

-  Pros:

   -  No external fragmentation of files.

   -  Simple to implement

   -  Sequential access is very simple.

-  Cons:

   -  Has internal fragmentation of the last block unless the last block
      is used completely.

   -  Random access in the file is difficult because for N blocks K-1
      blocks must be read to find block K.

   -  Storage available in a single block is not a power of two. Most
      programs send data to the filesystem in as buffers of sizes that
      are powers of two.

File Allocation Tables (FAT)
----------------------------

    .. figure:: storage/diagrams/linked_list_files_table.*
       :align: center
       :alt: image

       image

File Allocation Tables (FAT)
----------------------------

-  FAT based filesystems improve over linked list filesystems by moving
   the linked list into a centralized table called the FAT.

-  At FS mount time, the FAT is loaded into main memory. Random access
   is fast even though traversal is still needed.

-  Pros:

   -  Implementing a FAT FS is simple. Managing free space and disk
      layout is simple.

   -  FAT can be loaded into memory for fast and simple operations.

   -  Because blocks don’t contain pointers, the entire block can be
      used for data.

-  Cons:

   -  For large filesystems the FAT can become large and consume a lot
      of memory.

inodes
------

-  inodes are the fundamental structures of a UNIX filesystem

-  inodes have the following attributes:

   -  File Ownership - user, group

   -  File Mode - rwx bits for each of user, group, and others

   -  Last access and modified timestamps

   -  File size in bytes

   -  Device id

   -  Pointers to blocks on the storage device for the file or folder’s
      contents

Minix - inode
-------------

    .. figure:: storage/diagrams/minix_inode.*
       :align: center
       :alt: image

       image

Minix - inode
-------------

-  The first 7 "zones" point to the first 7 blocks of the file.

-  The "indirect zone" points to another block that contains a list of
   additional zones.

-  This has the advantage that the file can begin to be read quickly
   with the initial set of blocks available.

-  Also, the indirect zone allows for relatively fast random access by
   traversing the indirect blocks like a tree.

-  The pointer to a "double indirect" zone is a list of a list of zones.

-  The first few zones can address 7KB. The indirect zones can address
   up to 64MB. The double indirect zones can address more than 4GB.

inodes
------

-  The strategy of using indirect, double indirect, and even triple
   indirect blocks is a very successful implementation strategy

-  This approach is also used by ext2 / ext3 / ext4 in Linux.

Block Caches
------------

-  To improve the performance of a filesystem, and to make disk
   scheduling algorithms more realizable, most operating systems
   implement some kind of block cache.

-  The block cache allows for read-ahead and write-behind. It also
   allows for lower latency I/O operations.

-  With a block cache, the write() system call for instance only needs
   to complete modifications to the cache before returning. The
   operating system can complete the operation on disk in a background
   thread.

-  Without this cache, the system call would not be able to return until
   the write had been committed to disk.

Block Caches
------------

-  In Minix, the block cache is implemented with an LRU policy. The
   cache maintains a linked list of buffers from most recently to least
   recently used

    .. figure:: storage/diagrams/minix_block_cache.*
       :align: center
       :alt: image

       image

Block Caches
------------

-  Important parameters of any block cache are:

   -  The size of the cache in physical memory

   -  The delay before committing ’dirty’ items in the cache to disk

-  The larger the cache, the better the filesystem will likely perform,
   but this can come at the cost of available memory for programs.

-  The larger the delay before writing items to the disk, the better the
   disk allocation and scheduling decisions the operating system can
   make.

-  The shorter the delay before writing to disk, the greater the
   guarantee in the presence of failure that modifications will be
   persisted to disk.

Folders and Path Traversal
--------------------------

-  In all but the most simple filesystems, there is a concept of a
   folder and a path.

-  In UNIX operating systems, folder entries are held within inodes that
   have the filetype in the mode set to type directory.

-  The contents of the inode then are a list of filenames and pointers
   to the inodes of those files and/or folders.

-  Resolving paths involve accessing a root folder, and accessing each
   folder recursively until reaching a file or finding the folder to be
   invalid.

Path Traversal
--------------

-  An example of path traversal. When traversing paths, the path may
   cross into different filesystems.

    .. figure:: storage/diagrams/path_traversal.*
       :align: center
       :alt: image

       image

Virtual Filesystems / VFS
-------------------------

-  Aside from files and folders there are other things like named pipes,
   domain sockets, symbolic and hard links that need to be handled by
   the filesystem.

-  Rather than have the semantics of these implemented in each
   filesystem implementation, many OS architectures include a virtual
   filesystem or VFS.

-  The VFS stands between the OS kernel and the filesystem
   implementation.

Virtual Filesystems / VFS
-------------------------

-  The VFS can help adapt both foreign filesystems (such as VFAT) by
   producing a contract that these implementations can adapt to.

-  The VFS can also help reduce code duplication between FS
   implementations by providing common structures and handling shared
   behavior:

   -  Path traversal

   -  Handling named pipes, domain sockets, etc...

   -  Managing file handles and file locking

   -  Structures and functions for the block cache.

   -  Structures and functions for accessing storage devices

Virtual Filesystems and Stacking
--------------------------------

-  In some VFS implementations it is possible to stack filesystems on
   top of each other.

-  A great example of this in Linux is UMSDOS: the base VFAT filesystem
   does not have support for users, groups, security or extended
   attributes. By creating special files on VFAT and then hiding them,
   UMSDOS can adapt VFAT to be a UNIX-like filesystem

-  Another great example of this is UnionFS. It allows two filesystems
   to be transparently overlaid.

Virtual Filesystems and User-Mode
---------------------------------

-  Because VFS provides a contract for a filesystem to implement, it is
   simpler for unique filesystems to be implemented. Good examples
   include:

-  Proc - process and kernel metadata, typically mounted under ’/proc’

-  SysFs - exposes block and character device files to user mode,
   typically mounted under ’/dev’

-  FUSE - provides infrastructure to redirect calls to and from the VFS
   to and from user mode programs.

User-Mode Filesystems
---------------------

-  The advent of user-mode filesystems in popular operating systems
   (they existed in less popular operating systems for a while) has led
   to a great deal of new filesystem development.

-  The two most popular systems are FUSE for Linux / MacOSX, and other
   UNIX-like systems, and Dokan for Windows systems.

-  These frameworks have been greatly successful in large part because
   they help make the task of systems development much easier.

User-Mode Filesystems
---------------------

-  Development in a monolithic kernel can be very challenging. Crashes
   can bring the entire system down, stopping and restarting components
   may not be possible, and often debugging is limited to logs.

-  With user-mode development, it is possible to make use of a debugger
   in most cases.

-  Because of these advantages systems like FUSE and Dokan have become
   very popular.

-  Other areas of systems that were traditionally kernel mode only have
   moved to user-mode systems to ease development and improve
   architecture. In Windows, the display manager, and much of the driver
   framework has moved to user-mode.

Example pass-through FUSE Filesystem
------------------------------------

{language=C, basicstyle=, indent=xleftmargin}

::

    void ExampleFS::AbsPath(
        char dest[PATH_MAX], const char *path) {
      strcpy(dest, _root);
      strncat(dest, path, PATH_MAX);
    }
    void ExampleFS::setRootDir(const char *path) {
      printf("setting FS root to: %s\n", path);
      _root = path;
    }
    int ExampleFS::Getattr(
        const char *path, struct stat *statbuf) {
      char fullPath[PATH_MAX];
      AbsPath(fullPath, path);
      printf("getattr(%s)\n", fullPath);
      return RETURN_ERRNO(lstat(fullPath, statbuf));
    }

Example pass-through FUSE Filesystem
------------------------------------

{language=C, basicstyle=, indent=xleftmargin}

::

    int ExampleFS::Readlink(
    const char* path, char* link, size_t size){
      printf("readlink(path=%s, link=%s, size=%d)\n", 
                              path, link, (int)size);
      char fullPath[PATH_MAX];
      AbsPath(fullPath, path);
      return RETURN_ERRNO(readlink(fullPath, link, size));
    }
    int ExampleFS::Mknod(
    const char *path, mode_t mode, dev_t dev) {
      printf("mknod(path=%s, mode=%d)\n", path, mode);
      char fullPath[PATH_MAX];
      AbsPath(fullPath, path);
      //handles creating FIFOs, regular files, etc...
      return RETURN_ERRNO(mknod(fullPath, mode, dev));
    }

Example pass-through FUSE Filesystem
------------------------------------

{language=C, basicstyle=, indent=xleftmargin}

::

    int ExampleFS::Mkdir(const char *path, mode_t mode) {
      printf("**mkdir(path=%s, mode=%d)\n", path, (int)mode);
      char fullPath[PATH_MAX];
      AbsPath(fullPath, path);
      return RETURN_ERRNO(mkdir(fullPath, mode));
    }
    int ExampleFS::Unlink(const char *path) {
      printf("unlink(path=%s\n)", path);
      char fullPath[PATH_MAX];
      AbsPath(fullPath, path);
      return RETURN_ERRNO(unlink(fullPath));
    }
    int ExampleFS::Rmdir(const char *path) {
      printf("rmkdir(path=%s\n)", path);
      char fullPath[PATH_MAX];
      AbsPath(fullPath, path);
      return RETURN_ERRNO(rmdir(fullPath));
    }

Example pass-through FUSE Filesystem
------------------------------------

{language=C, basicstyle=, indent=xleftmargin}

::

    int ExampleFS::Symlink(const char *path, const char *link) {
      printf("symlink(path=%s, link=%s)\n", path, link);
      char fullPath[PATH_MAX];
      AbsPath(fullPath, path);
      return RETURN_ERRNO(symlink(fullPath, link));
    }
    int ExampleFS::Rename(const char *path, const char *newpath) {
      printf("rename(path=%s, newPath=%s)\n", path, newpath);
      char fullPath[PATH_MAX];
      AbsPath(fullPath, path);
      return RETURN_ERRNO(rename(fullPath, newpath));
    }

Example pass-through FUSE Filesystem
------------------------------------

{language=C, basicstyle=, indent=xleftmargin}

::

    int ExampleFS::Link(const char *path, const char *newpath) {
      printf("link(path=%s, newPath=%s)\n", path, newpath);
      char fullPath[PATH_MAX];
      char fullNewPath[PATH_MAX];
      AbsPath(fullPath, path);
      AbsPath(fullNewPath, newpath);
      return RETURN_ERRNO(link(fullPath, fullNewPath));
    }
    int ExampleFS::Chmod(const char *path, mode_t mode) {
      printf("chmod(path=%s, mode=%d)\n", path, mode);
      char fullPath[PATH_MAX];
      AbsPath(fullPath, path);
      return RETURN_ERRNO(chmod(fullPath, mode));
    }

Example pass-through FUSE Filesystem
------------------------------------

{language=C, basicstyle=, indent=xleftmargin}

::

    int ExampleFS::Chown(const char *path, uid_t uid, gid_t gid) {
      printf("chown(path=%s, uid=%d, gid=%d)\n", 
                path, (int)uid, (int)gid);
      char fullPath[PATH_MAX];
      AbsPath(fullPath, path);
      return RETURN_ERRNO(chown(fullPath, uid, gid));
    }
    int ExampleFS::Truncate(const char *path, off_t newSize) {
      printf("truncate(path=%s, newSize=%d\n", path, (int)newSize);
      char fullPath[PATH_MAX];
      AbsPath(fullPath, path);
      return RETURN_ERRNO(truncate(fullPath, newSize));
    }
    int ExampleFS::Utime(const char *path, struct utimbuf *ubuf) {
      printf("utime(path=%s)\n", path);
      char fullPath[PATH_MAX];
      AbsPath(fullPath, path);
      return RETURN_ERRNO(utime(fullPath, ubuf));
    }

Example pass-through FUSE Filesystem
------------------------------------

{language=C, basicstyle=, indent=xleftmargin}

::

    int ExampleFS::Open(const char *path, 
            struct fuse_file_info *fileInfo) {
      char fullPath[PATH_MAX];
      AbsPath(fullPath, path);
      fileInfo->fh = open(fullPath, fileInfo->flags);
      return 0;
    }
    int ExampleFS::Read(const char *path, char *buf, 
            size_t size, off_t offset, struct fuse_file_info *fileInfo) {
      return RETURN_ERRNO(pread(fileInfo->fh, buf, size, offset));
    }
    int ExampleFS::Write(const char *path, const char *buf, 
            size_t size, off_t offset, struct fuse_file_info *fileInfo) {
      return RETURN_ERRNO(pwrite(fileInfo->fh, buf, size, offset));
    }

Example pass-through FUSE Filesystem
------------------------------------

{language=C, basicstyle=, indent=xleftmargin}

::

    int ExampleFS::Statfs(const char *path, struct statvfs *statInfo) {
      printf("statfs(path=%s)\n", path);
      char fullPath[PATH_MAX];
      AbsPath(fullPath, path);
      return RETURN_ERRNO(statvfs(fullPath, statInfo));
    }
    int ExampleFS::Flush(const char *path, struct fuse_file_info *fileInfo) {
      printf("flush(path=%s)\n", path);
      //noop because we don't maintain our own buffers
      return 0;
    }
    int ExampleFS::Release(const char *path, struct fuse_file_info *fileInfo) {
      printf("release(path=%s)\n", path);
      return 0;
    }

Example pass-through FUSE Filesystem
------------------------------------

{language=C, basicstyle=, indent=xleftmargin}

::

    int ExampleFS::Fsync(const char *path, int datasync, struct fuse_file_info *fi) {
      printf("fsync(path=%s, datasync=%d\n", path, datasync);
      if(datasync) {
        //sync data only
        return RETURN_ERRNO(fdatasync(fi->fh));
      } else {
        //sync data + file metadata
        return RETURN_ERRNO(fsync(fi->fh));
      }
    }
    int ExampleFS::Setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
      printf("setxattr(path=%s, name=%s, value=%s, size=%d, flags=%d\n",
        path, name, value, (int)size, flags);
      char fullPath[PATH_MAX];
      AbsPath(fullPath, path);
      return RETURN_ERRNO(lsetxattr(fullPath, name, value, size, flags));
    }

Example pass-through FUSE Filesystem
------------------------------------

{language=C, basicstyle=, indent=xleftmargin}

::

    int ExampleFS::Getxattr(const char *path, 
        const char *name, char *value, size_t size) {
      char fullPath[PATH_MAX];
      AbsPath(fullPath, path);
      return RETURN_ERRNO(getxattr(fullPath, name, value, size));
    }
    int ExampleFS::Listxattr(const char *path, 
        char *list, size_t size) {
      char fullPath[PATH_MAX];
      AbsPath(fullPath, path);
      return RETURN_ERRNO(llistxattr(fullPath, list, size));
    }
    int ExampleFS::Removexattr(const char *path, const char *name) {
      char fullPath[PATH_MAX];
      AbsPath(fullPath, path);
      return RETURN_ERRNO(lremovexattr(fullPath, name));
    }

Example pass-through FUSE Filesystem
------------------------------------

{language=C, basicstyle=, indent=xleftmargin}

::

    int ExampleFS::Opendir(const char *path, 
        struct fuse_file_info *fileInfo) {
      printf("opendir(path=%s)\n", path);
      char fullPath[PATH_MAX];
      AbsPath(fullPath, path);
      DIR *dir = opendir(fullPath);
      fileInfo->fh = (uint64_t)dir;
      return NULL -- dir ? -errno : 0;
    }

Example pass-through FUSE Filesystem
------------------------------------

{language=C, basicstyle=, indent=xleftmargin}

::

    int ExampleFS::Readdir(const char *path, void *buf, 
        fuse_fill_dir_t filler, off_t offset, 
        struct fuse_file_info *fileInfo) {
      printf("readdir(path=%s, offset=%d)\n", path, (int)offset);
      DIR *dir = (DIR*)fileInfo->fh;
      struct dirent *de = readdir(dir);
      if(NULL -- de) {
        return -errno;
      } else {
        do {
          if(filler(buf, de->d_name, NULL, 0) != 0) {
            return -ENOMEM;
          }
        } while(NULL != (de = readdir(dir)));
      }
      return 0;
    }

Example pass-through FUSE Filesystem
------------------------------------

{language=C, basicstyle=, indent=xleftmargin}

::

    int ExampleFS::Releasedir(const char *path, 
        struct fuse_file_info *fileInfo) {
      closedir((DIR*)fileInfo->fh);
      return 0;
    }
    int ExampleFS::Fsyncdir(const char *path, int datasync, 
        struct fuse_file_info *fileInfo) {
      return 0;
    }
    int ExampleFS::Init(struct fuse_conn_info *conn) {
      return 0;
    }
    int ExampleFS::Truncate(const char *path, off_t offset, 
        struct fuse_file_info *fileInfo) {
      char fullPath[PATH_MAX];
      AbsPath(fullPath, path);
      return RETURN_ERRNO(ftruncate(fileInfo->fh, offset));
    }

FUSE
----

-  As you can see in the above example, the FUSE filesystem matches
   closely the contract of the UNIX system calls for files and folders.

-  Each of these functions has very well explained behavior that can be
   found in the manual pages for each of them.

-  Typically, a FUSE filesystem can be implemented with between 500 -
   4000 lines of code. This is fairly comparable to kernel mode
   filesystems.

-  A very advanced filesystem, NTFS, has been implemented with FUSE in
   about 17,500 lines of code.

-  A very popular FUSE filesystem, SSHFS, has been implemented in about
   4,500 lines of code.

-  In the Linux kernel, Ext4 is approximately 35,500 lines of code and
   Ext2 is approximately 9000 lines of code.

Filesystems Research at Loyola
------------------------------

-  Profs. Thiruvathukal, Laufer, and myself have been working on
   filesystems and filesystems frameworks for several years.

-  Our first project was OLFS. OLFS was a FUSE filesystem similar to
   UnionFS, but written in Java. This was an excellent learning
   experience for us.

-  Building on our experience with OLFS, we developed two frameworks:
   NOFS and RestFS. We believe that both of these frameworks help to
   both bring filesystems development to high level languages and
   greatly simplify the problem space.

NOFS
----

-  In most OOP graphical applications, there are a few common
   components:

   -  A domain model

   -  A view/presentation layer

   -  A controller layer that translates between the domain model and
      the presentation layer

   -  A persistence layer. This can be the filesystem or a database.

-  The principle behind the NO part of NOFS (Naked Objects) is that a
   behaviorally complete domain model can be all that an application
   needs if the other layers can be generated in an automated fashion.

-  For many applications the Naked Objects architecture reduces the
   amount of development effort and complexity while producing a decent
   application

NOFS
----

-  The principle behind NOFS is that the fundamental structures or
   domain objects of a filesystem should be all that need to be
   implemented and the interface to any VFS layer or any persistence
   mechanism (hard disk, database, filesystem) should be provided in an
   automatic fashion.

-  With NOFS, it is possible to build a complete filesystem with a
   single Java class containing no more than 50-60 lines of code and
   little or no knowledge about the filesystem contract (operations like
   truncate, read, write, seek, etc...) or the underlying storage
   architecture (hard disk, files, etc...)

An Example NOFS Filesystem in 3 Slides
--------------------------------------

{language=Java, basicstyle=, indent=xleftmargin}

::

    @RootFolderObject
    @DomainObject
    @FolderObject(CanAdd=false, CanRemove=false)
    public class Book {
      private IDomainObjectContainer<Book> _bookContainer;
      private IDomainObjectContainerManager _containerManager;
      @NeedsContainer
      public void setContainer(IDomainObjectContainer<Book> container) {
        _bookContainer = container;
      }
      @NeedsContainerManager
      public void setContainerManager(IDomainObjectContainerManager manager) {
        _containerManager = manager;
      }
      private IDomainObjectContainerManager GetContainerManager() throws Exception {
        if(_containerManager -- null) {
          throw new Exception("container manager is null");
        }
        return _containerManager;
      }

      public List<Contact> getContacts() throws Exception {
        IDomainObjectContainer<Contact> contactContainer =
          GetContainerManager().GetContainer(Contact.class);
        if(contactContainer -- null) {
          throw new Exception("Container for Contact is null");
        }
        return new LinkedList<Contact>(contactContainer.GetAllInstances());
      }

An Example NOFS Filesystem in 3 Slides
--------------------------------------

{language=Java, basicstyle=, indent=xleftmargin}

::

      @Executable
      public void AddAContact(String name, String phone) throws Exception {
        IDomainObjectContainer<Contact> contactContainer = 
                        GetContainerManager().GetContainer(Contact.class);
        Contact contact = contactContainer.NewPersistentInstance();
        contact.setName(name);
        contact.setPhoneNumber(phone);
        _bookContainer.ObjectChanged(this);
      }

      @Executable
      public void RemoveAContact(Contact contact) throws Exception {
        IDomainObjectContainer<Contact> contactContainer = 
                     GetContainerManager().GetContainer(Contact.class);
        contactContainer.Remove(contact);
        _bookContainer.ObjectChanged(this);
      }

      @Executable
      public void RenameAContact(Contact contact, String newName) throws Exception {
        IDomainObjectContainer<Contact> contactContainer = 
                     GetContainerManager().GetContainer(Contact.class);
        String oldName = contact.getName();
        contact.setName(newName);
        contactContainer.ObjectRenamed(contact, oldName, newName);
        contactContainer.ObjectChanged(contact);
      }
    }

An Example NOFS Filesystem in 3 Slides
--------------------------------------

{language=Java, basicstyle=, indent=xleftmargin}

::

    @DomainObject
    public class Contact {
      private String _name;
      private String _phoneNumber;
      private IDomainObjectContainer<Contact> _container;

      @ProvidesName
      public String getName() {
        return _name;
      }

      @ProvidesName
      public void setName(String name) throws Exception {
        _name = name;
        _container.ObjectChanged(this);
      }

      public String getPhoneNumber() { return _phoneNumber; }
      public void setPhoneNumber(String value) throws Exception {
        _phoneNumber = value;
        _container.ObjectChanged(this);
      }

      @NeedsContainer
      public void setContainer(IDomainObjectContainer<Contact> container) {
        _container = container;
      }
    }

NOFS - Running the Sample
-------------------------

    .. figure:: storage/diagrams/nofs_addressbook.*
       :align: center
       :alt: image

       image

NOFS
----

-  With NOFS, the developer can fill in as many details as they are
   concerned about.

-  If the developer wants to implement details about permissions, random
   file access, persistence, caches, etc.. it is optional. Any details
   left out are filled in by the NOFS framework with reasonable
   defaults.

-  For application oriented filesystems - filesystems that are oriented
   more towards behavior than storage - the developer does not need to
   be concerned with any detail of the filesystem contract.

-  For storage oriented filesystems, the developer needs to manage more
   details. Things like permissions, access / modify times, user, and
   group ownership become more relevant.

NOFS - Architecture - Relation to FUSE and the OS
-------------------------------------------------

    .. figure:: storage/diagrams/nofs_kernel_usermode.*
       :align: center
       :alt: image

       image

NOFS - Architecture - Translation of Domain Model to Files
----------------------------------------------------------

    .. figure:: storage/diagrams/nofs_cache_serialization.*
       :align: center
       :alt: image

       image

NOFS - Architecture - Method Execution
--------------------------------------

    .. figure:: storage/diagrams/nofs_execution.*
       :align: center
       :alt: image

       image

RestFS
------

-  While developing NOFS, we realized that there were two common types
   of application oriented filesystems

-  The first type is a local or mixed local / remote behavioral
   filesystem. Something like the addressbook example that we
   demonstrated earlier.

-  The second type that we discovered where filesystems that connected
   to web services. One example we built was a NOFS filesystem in about
   400-500 lines of code to mount a Flickr photo album as a folder of
   pictures.

-  With the knowledge that RESTful services are very similar to
   filesystems architecturally and how well NOFS was doing with
   application oriented filesystems. We asked ourselves if we could make
   accessing web services even easier.

RestFS - Service Composition
----------------------------

    .. figure:: storage/diagrams/composition.*
       :align: center
       :alt: image

       image

RestFS
------

-  While developing application filesystems for web services with NOFS,
   we found that domain model modifications were roughly mapping one or
   two FUSE operations to a HTTP verb.

-  So, our first attempt with RestFS has been to provide file and
   configuration file pairs to map particular FS system calls onto
   particular web methods.

-  This has allowed us to do things such as configure a file so that
   when it is ’touched’ that a Google search can be performed with the
   results stored to the file.

RestFS - Communications
-----------------------

    .. figure:: storage/diagrams/communication_timeline.*
       :align: center
       :alt: image

       image

RestFS - Google Search
----------------------

{language=bash, basicstyle=, indent=xleftmargin}

::

    #!/bin/bash
    URL_ARGS=`echo $@ | sed 's/ /%20/g'`
    FILE_NAME=`echo $@ | sed 's/ /_/g'`
    RESOURCE="ajax/services/search/web?v=1.0&q=$URL_ARGS"
    RESOURCE=`echo $RESOURCE | \
             sed -e 's~&~\&amp;~g' \
                 -e 's~<~\&lt;~g' \
                 -e 's~>~\&gt;~g'`
    touch $FILE_NAME
    ./configureResource .$FILE_NAME \
             fs:utime web:get \
             host:ajax.googleapis.com \
             resource:$RESOURCE
    touch $FILE_NAME
    cat $FILE_NAME

RestFS - Authentication
-----------------------

-  A common concern for many rest-ful services is authentication.

-  A popular authentication method for many rest-ful services is OAuth.

-  Since RestFS is built on top of NOFS, we were able to map an existing
   OAuth library into the filesystem

-  In RestFS, there is a special OAuth folder called ’/auth’

RestFS - Authentication
-----------------------

    .. figure:: storage/diagrams/restfs_auth.*
       :align: center
       :alt: image

       image

RestFS - Authentication
-----------------------

    .. figure:: storage/diagrams/restfs_authentication_process.*
       :align: center
       :alt: image

       image

RestFS
------

-  With RestFS we’ve been able to show how local software that works
   principally with files (cat, grep), can be used with web services

-  We’ve also been able to demonstrate how remote web services can be
   enhanced locally with RestFS and then re-exposed as new rest-ful web
   services.

RestFS - Future Directions
--------------------------

-  We’re currently re-architecting RestFS to be more rules based so that
   more than one FS system call can be mapped per file.

-  We’re also investigating how folders and symbolic links can be used
   to further enhance RestFS.

-  During the fall of 2011, with the help of Shaohui Chen, we’ve been
   able to move NOFS from Java to the .NET framework

-  This porting effort will allow us to support both Windows and Linux,
   and give us a much better language framework to work from.


