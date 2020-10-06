Storage and Devices
===================


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
   formula: :math:`$A = (C x Nh + H) * Ns + (S - 1)$`

-  :math:`$Nh$` is the total number of heads

-  :math:`$Ns$` is the total number of sectors

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



