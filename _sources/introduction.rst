.. _introduction:

Introduction
============

.. _history_operating_systems:

History of Operating Systems
----------------------------

.. index::
   single: operating systems; history
   pair: operating systems; generations

The journey through operating systems starts from the first-generation systems, characterized by vacuum tubes and punch cards, to the present-day advanced systems. This evolution includes several generations, each marked by significant technological advancements and changes in computing paradigms.

Operating systems came relatively late in the history of computing.
For broader background on early electronic computing, see the
`Computing History <https://introcs-python.cs.luc.edu/computing_history/overview.html>`_
chapter in *Introduction to Computer Science in Python*.

.. _first_generation_systems:

First Generation Operating Systems
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: first generation systems
   pair: operating systems; first generation
   single: vacuum tubes
   single: EDVAC
   single: machine language
   single: punch cards

In the early days, computers like EDVAC used vacuum tubes and were programmed with machine language. There were no operating systems, and programming was primarily for numerical calculations.

.. _second_generation_systems:

Second Generation Systems
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: second generation systems
   pair: operating systems; second generation
   single: transistors
   single: mainframe
   single: batch processing
   single: FORTRAN
   single: assembly language

The era of the 1950s and 1960s introduced transistors, leading to the mainframe and batch processing systems. Assembly languages and FORTRAN were prominent, and batch systems consisted of card readers, processors, and printers.

.. _third_generation_systems:

Third Generation Systems
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: third generation systems
   pair: operating systems; third generation
   single: integrated circuits
   single: multi-programming
   pair: IBM; System/360

From the 1960s to the 1980s, integrated circuits came into play. This period marked the beginning of machine images/abstractions and multi-programming, with systems like IBM System/360 playing a significant role.

.. _fourth_generation_systems:

Fourth Generation Systems
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: fourth generation systems
   pair: operating systems; fourth generation
   single: microprocessors
   single: UNIX
   single: MS-DOS

The period from the 1980s to the present saw the rise of micro-processors and the era of mini-computers. Operating systems like UNIX and early versions of MS-DOS emerged during this time.

.. _history_windows_nt_xp_vista_7:

Windows NT/XP/Vista/7
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: Windows NT
   single: Windows 2000
   single: Windows XP
   single: Windows Vista
   single: Windows 7
   pair: Microsoft; Windows
   single: Active Directory

This section details the evolution of Windows operating systems from Windows NT, a competitor to OS/2, through various versions like Windows 2000, XP, Vista, to Windows 7. Each version brought new advancements, from virtual memory support and Active Directory in Windows 2000 to major UI improvements and 64-bit support in Windows 7.

.. _history_linux:

Linux
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: Linux
   pair: Linux; kernel history
   single: Minix

The Linux operating system saw its inception from 0.x versions in the early '90s, inspired by Minix, to the significant improvements in the 2.6 version. The evolution covers improvements in hardware support, performance, and kernel development, with Linux becoming a formidable player in server and desktop operating systems.

.. _types_of_computer_systems:

Types of Computer Systems
-------------------------

.. index::
   single: computer systems; types

This section explores various types of computer systems, including batch processing systems, personal/parallel computers, clusters, grid computers, distributed systems, gaming consoles, handheld/tablet computers, and smartphones, each with its unique characteristics and uses.


.. _personal_computers:

Personal Computers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: personal computers
   pair: computer systems; personal

Personal computers, commonly referred to as PCs, are designed for individual use. These computers are prevalent in both professional and personal settings, offering a range of functionalities from word processing and web browsing to gaming and media consumption. Their versatility, ease of use, and affordability have made them a staple in modern daily life.

.. _parallel_computers:

Parallel Computers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: parallel computers
   pair: computer systems; parallel
   single: parallel processing

Parallel computers are designed to perform complex computations by dividing the tasks across multiple processors. These systems are characterized by their ability to run processes simultaneously, significantly increasing computational speed and efficiency. They are especially useful in scientific research, engineering simulations, and other fields requiring extensive data processing and computation.


.. _distributed_systems:

Distributed Systems
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: distributed systems
   pair: computer systems; distributed

Distributed systems involve a network of interconnected computers that interact with each other to achieve a common goal. These systems distribute processing and data storage across multiple nodes, offering resilience, scalability, and resource sharing, making them ideal for applications that require reliability across multiple networked environments.

.. _batch_processing_systems:

Batch Processing Systems
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: batch processing systems
   pair: computer systems; batch processing

Batch processing systems process large volumes of data in batches without continuous user interaction. These systems are integral in environments where vast datasets are processed, like financial transaction processing or large-scale data analytics.

.. _clusters:

Clusters
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: clusters
   pair: computer systems; clusters
   single: high availability

Clusters consist of multiple computers connected to work together as a single system, enhancing performance, and providing high availability. They are often used in environments that require a high degree of computational power or high availability of resources, like scientific computing or web services.

.. _cloud_computing:

Cloud Computing
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: cloud computing
   pair: computer systems; cloud

Cloud computing delivers computing services—including servers, storage, databases, networking, software, analytics, and intelligence—over the internet ("the cloud"). It offers faster innovation, flexible resources, and economies of scale, typically paying only for cloud services used.

.. _grid_computing:

Grid Computing
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: grid computing
   pair: computer systems; grid

Grid computing connects disparate computers, often geographically dispersed, to create a distributed computing system capable of handling large-scale processing tasks. This system is often utilized in scientific research, where massive computational tasks are distributed across various nodes in the grid.

Quantum Computing
^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: quantum computing
   single: qubits
   pair: computer systems; quantum
   single: quantum mechanics

Quantum computing represents a significant leap forward in computational capabilities, utilizing the principles of quantum mechanics. Unlike classical computers which use bits as the smallest unit of data, quantum computers use quantum bits or qubits, which can exist in multiple states simultaneously. This allows quantum computers to perform complex calculations at speeds unattainable by traditional computing systems. Quantum computing holds immense potential for solving intricate problems in fields like cryptography, materials science, and complex system modeling, which are beyond the reach of classical computers.

Summary
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Each type of computing system has its unique characteristics and applications, contributing to the diverse landscape of modern computing. If further details or adjustments are needed, please let me know.

Types of Operating Systems
--------------------------

.. index::
   single: operating systems; types
   single: real-time operating system
   single: microkernel
   single: monolithic kernel
   single: distributed operating system

Operating systems are categorized into several types, such as single program/simple OS, real-time OS, microkernel, monolithic kernel, and distributed OS. Each type serves different purposes and is suited for specific applications and hardware environments.



.. index::
   single: UNIX shell
   single: bash
   pair: shell; bash
   single: /bin/bash

Welcome to this Crash Course on the UNIX Shell, where we delve into the basics of using `/bin/bash`, one of the most commonly used shells. The shell is a powerful interface that lets users interact efficiently with the system through various commands.

.. _basic_operations:

Basic Shell Operations
----------------------

The UNIX shell is versatile, allowing you to perform a wide range of operations with simple commands. Here are some fundamental operations:

.. _displaying_file_contents:

Displaying File Contents
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: cat command
   pair: shell command; cat

You can display the contents of a file or multiple files using the `cat` command. For a single file:

.. code-block:: bash

   $ cat file

For multiple files, either list them:

.. code-block:: bash

   $ cat file1 file2 file3

or use bracket expansion:

.. code-block:: bash

   $ cat {file1,file2,file3}

.. _creating_directories_files:

Creating Directories and Files
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: mkdir command
   pair: shell command; mkdir
   single: touch command
   pair: shell command; touch

Creating directories and files is a common task. To create a directory:

.. code-block:: bash

   $ mkdir foobar

For nested directories:

.. code-block:: bash

   $ mkdir -p foo/bar/baz

Creating files can be as simple as:

.. code-block:: bash

   $ touch file1

or creating multiple files at once:

.. code-block:: bash

   $ touch file1 file2 file3
   $ touch {file1,file2,file3}

Creating an empty file:

.. code-block:: bash

   > filename.txt

Be cautious with these commands as some can be destructive if not used properly.

.. _more_useful_commands:

Print Text to Console
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. _print_text_console:

.. index::
   single: echo command
   pair: shell command; echo

Print text to the console:

.. code-block:: bash

   $ echo "hello world"
   $ echo -e "hello\t\t world\n"

.. _read_file_content:

.. index::
   single: less command
   pair: shell command; less

Read the contents of a file, one screen at a time:

.. code-block:: bash

   $ less file1.txt

.. _search_file_text:

.. index::
   single: grep command
   pair: shell command; grep

Search a file for specific text:

.. code-block:: bash

   $ grep "hello world" file1.txt
   $ grep -e "hello.* d" file1.txt

The sections above provide a foundation in UNIX shell commands. The following sections will continue to explore more advanced operations and concepts in UNIX, such as file permissions, combining commands, background processes, and an introduction to C programming.
i



.. index::
   single: pwd command
   pair: shell command; pwd

Print Working Directory (pwd)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The `pwd` command displays the current working directory's absolute path. It's an essential command for orienting yourself in the filesystem and ensuring you're in the correct directory before performing operations.

.. code-block:: bash

   $ pwd

.. _list_files_directories:

List Files and Directories (ls)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: ls command
   pair: shell command; ls

`ls` is one of the most frequently used commands. It lists files and directories in the current directory. With options like `-l` for a detailed list and `-a` to include hidden files, it provides a versatile way to view directory contents.

.. code-block:: bash

   $ ls
   $ ls -l
   $ ls -a

.. _change_directory:

Change Directory (cd)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: cd command
   pair: shell command; cd

Navigating the filesystem is a fundamental task in the shell, and `cd` is the command that makes it possible. It changes the current directory to another one specified in the command.

.. code-block:: bash

   $ cd /path/to/directory
   $ cd ..
   $ cd

.. _copy_files_directories:

Copy Files and Directories (cp)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: cp command
   pair: shell command; cp

The `cp` command is used for copying files and directories. It's crucial for duplicating data, creating backups, or moving data around. The `-r` option is used to copy directories recursively.

.. code-block:: bash

   $ cp source_file destination_file
   $ cp -r source_directory destination_directory

.. _move_rename_files:

Move/Rename Files (mv)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: mv command
   pair: shell command; mv

`mv` serves a dual purpose: it moves files and directories and renames them. This command is especially useful for organizing files and directories or changing file names.

.. code-block:: bash

   $ mv old_filename new_filename
   $ mv file /path/to/new/location

.. _remove_files_directories:

Remove Files and Directories (rm)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: rm command
   pair: shell command; rm

`rm` is used to delete files and directories. Caution is advised with this command, especially with the `-r` option for recursive deletion, as it permanently removes data.

.. code-block:: bash

   $ rm file
   $ rm -r directory

.. _view_file_contents:

View File Contents (less, more, cat)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: more command
   pair: shell command; more

Commands like `less`, `more`, and `cat` are used for viewing the contents of files. `less` and `more` are particularly useful for navigating through larger files.

.. code-block:: bash

   $ less file
   $ more file
   $ cat file

.. _find_files_directories:

Find Files and Directories (find, locate)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: find command
   pair: shell command; find
   single: locate command
   pair: shell command; locate

The `find` and `locate` commands are powerful tools for searching files and directories. They can search based on different criteria like name, size, or modification date.

.. code-block:: bash

   $ find /path -name "filename"
   $ locate filename

.. _file_permissions_ownership:

File Permissions and Ownership (chmod, chown)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: chmod command
   pair: shell command; chmod
   single: chown command
   pair: shell command; chown
   single: file permissions
   pair: security; file permissions

`chmod` and `chown` are vital for managing file permissions and ownership. They ensure security and proper access control of files and directories.

.. code-block:: bash

   $ chmod 755 file
   $ chown user:group file

.. _process_management:

Process Management (ps, top, kill)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: ps command
   pair: shell command; ps
   single: top command
   pair: shell command; top
   single: kill command
   pair: shell command; kill
   pair: process; management

Managing processes is a critical aspect of system administration. Commands like `ps`, `top`, and `kill` allow users to view active processes, monitor system resources, and terminate processes when necessary.

.. code-block:: bash

   $ ps
   $ top
   $ kill PID
