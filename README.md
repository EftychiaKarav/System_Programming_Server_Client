# System Programming - Server Client Architecture
* Server-Client Architecture
* Sockets
* Threads

## Development Environment

The code was tested in the Linux systems of our faculty's lab. More specifically, the software used was:
* Ubuntu 18.04.6 LTS
* gcc 7.5.0
* valgrind 3.13.0
* GNU Make 4.1

## Project Structure
The project contains the following directories:
* **objects**: is created when the user compiles the program, contains all object (.o) files
* **headers**: contains all header files (.h) that the project needs
* **sources**: contains all source code (.c)


## Objective
The purpose of this program is to copy all contents of a directory of a server recursively to the local file system of a client.

The **server** handles connection requests from many different clients and processes the different requests simultaneously. After the server has sent the required contents to the clients, each **client** will have a copy of the directiry it requested. The copied directory has the exact same structure and files as the one the directiry in the server has.

The communication between the server and each client is achieved through sockets using the TCP/IP protocol. The server uses communication threads in order to communicate with more than one client at a time and worker threads, so that the server can send different files to different clients simultaneously.

## Compilation and Running

The project's directory contains a **Makefile** to be used for separate compilation. In order to run the contest's test driver, when you are at the main directory of the project, you need to type to the terminal:

```
make 
```
This Makefile directive will produce all object files (.o) and store them in a directory named "**objects**" as well as **dataServer** and **remoteClient** executables. It also deletes the previous versions of those files.
The sequence of command line arguments is irrelevant.

To **run** the **server** you can type:

```
./dataServer -p <port> -s <thread_pool_size> -q <queue_size> -b <block_size>

port: the port where server listens for connections
thread_pool_size: number of worker threads
queue_size: maximum number of files that can be in the queue waiting to be copied
block_size: number of bytes that the worker threads send
```

To **run** the **client** you can type in another terminal:
(you can run many clients simultaneously) 
```
./remoteClient -i <server_ip> -p <server_port> -d <directoy>

server_ip: ip of the server
server_port: the port where server listens for connections
directory: path to a directory to be copied
```

* If server and clients run in the same host, provide -i 127.0.0.1 as argument for the server's IP.
* If they are in different hosts, then type at server's side "hostname -I" to find the IP of the server and provide it as argument at the clients.

### Useful comments

- If the program presents any errors/failures during execution, it will exit. An error message will be dispalyed. However, if it presents any errors/failures and does not exit, then its behavior will be undefined.

- If a wrong directory is given as an argument and the server cannot resolve the path then the programm should be rerun.
- **remoteClients** exit themselves after getting the requested directory from the dataServer.
- **dataServer** terminates after typing ^C, but it can also stop with ^Z, which has the default
action.
- **BEFORE TERMINATING** the dataServer with ^C, make sure that **ALL** clients have received the directories they requested.
-Each directory that each remoteClient requestsis being saved under the following name format: SERVER_COPY_[pid_client], where [pid_client] is the process id of the remoteClient when it was
running. These directories are created inside the main directory of the project and **ARE DELETED EVERY TIME "MAKE"** is typed (because slso "make clean" is being run).
- With every "make" the **objects** directory and the copied directories to the clients (**SERVER_COPY_[pid_client]**) are being deleted and created again. 

  
## Bash Scripting

**!! IMPORTANT !!** 
multiple_clients_test.sh is a script I used to test my project. You can see what kinds of experiments
I conducted. *DO NOT RUN* this bash script as it includes paths from my own PC. It is just a sample to show how I ran and tested my project. You can use it changing the paths. 

I conducted lots of experiments with various combinations for the command line arguments.

```
MAXIMUM VALUES TESTED:

- max_block_size 8192
- max queue_size 16
- max thread_pool size 16
```