KARAVANGELI EFTYCHIA - 1115201800062

***************************************************USEFUL NOTES*****************************************************


- In this file only genaral information is presented. It concerns compilation and a high level description of what 
each file of the project contains.
- There are highly detailed and descriptive comments in all code files which are very explanatory.
- If -during execution- the program presents any errors/failures it will exit. An error message will be dispalyed.
However, if it presents any errors/failures and does not exit, then its behavior will be undefined.
- remoteClients exit themselves after getting the requested directory from the dataServer
- dataServer terminates after typing ^C (which is being handled), but it can also stop with ^Z, which has the default
action.
- multiple_clients.sh (end of this file) is a script I used to test my project. You can see what kinds of experiments
I made (see end of the file).


--------------------------------------STRUCTURE OF THE PROJECT & COMPILATION----------------------------------------


Implementation language: C
The project was implemented in vscode in WSL and was tested at linux04, linux05 and linux06 with two versions:
1. Both dataServer and remoteClient were running in the same host
2. dataServer was running in e.g. linux04 and remoteClient in other hosts e.g. linux05, linux06

There was no problem to be found. 
Valgrind tool identified 0 leaks in all categories and 0 errors for both dataServer and remoteClient.

!!! IMPORTANT !!!
- The DEFAULT DIRECTORY at the dataServer side is "/home/users". All directories given as arguments at the command line
from the remoteClient side should be relative paths to "/home/users" directory.

After unzipping we are in the directory EftychiaKaravangeliProject2/ and there is inside:
- Makefile
- README.txt
and the directories:
- sources/ (contains all .c arxeia --> dataServer.c, Server.c, remoteClient.c, Client.c, CommonFuncs.c, Queue.c)
- headers/ (contains all .h arxeia --> libraries.h, Server.h, Client.h, Queue.h)

-- COMPILATION:
In the "EftychiaKaravangeliProject2/" directory type "make". "make clean" also runs automatically after typing "make".
You can type of course make clean seperately if you wish. All .o files are created in a new directory called objects/
and it is inside EftychiaKaravangeliProject2/ directory. Both executable programms are inside EftychiaKaravangeliProject2/
directory and the programms should be executed from this directory. The sequence of the arguments in the command line for
both programms does not matter.

--COPIED FILES AT THE CLIENTS:
The whole directory every remoteClient requests from the dataServer is being saved in a newly created directory which has
the following name format: SERVER_COPY_[pid_client], where [pid_client] is the process id of the remoteClient when it was
running. Every remoteClient has his own directory of course. These directories are created inside EftychiaKaravangeliProject2
directory and ARE DELETED EVERY TIME "MAKE" is typed (because it also runs "make clean"). They contain the whole filesystem
hierarchy of the Server (/home/users/.../)

!! IMPORTANT !! With every "make" the objects/ directory is being deleted and created again. 


---------------------------------------------------libraries.h------------------------------------------------------


It contains all the libraries needed for the execution of the project as well as some CONSTANTS and the prototypes
of some generic and common functions, which both Client and Server use. For more details check the file.


--------------------------------------------------CommonFuncs.c-----------------------------------------------------


These are helpful functions for both dataServer and remoteClient. They are the same with some of the functions from the
course material or have small changes for the purpose of the project. They concern:
1. error handling
2. clearing buffers
3. sending data
4. receiving data. 

source: https://cgi.di.uoa.gr/~antoulas/k24/ --> sockets-src/src/prs.c, sockets-src/src/prsref.c and
                                                 sockets-src/src/rls.c


-------------------------------------------------Queue.c--Queue.h---------------------------------------------------


!These files were used at the previous project as well. A few alterations were made due to the project's purposeses!
They contain a FIFO queue implementation with pointers (like a list). The nodes of the queue are pointers to a struct
called Queue_Node. This struct contains an int and a pointer to union called Data_Node. The union contais a char* and 
a mutex. 
In this project, Queue is used to save information for 2 entities.
1. Files_Queue         --> uses the int and the char*
2. Mutex_Socket_Queue  --> uses the int and the mutex


1. Files_Queue:
    The elements of this Queue are pointers to structs, which contain an int and a char*. The int is the socket number
    for every Client, who is connected to the Server and assures the communication with him. The char* is the name of 
    the absolute path to a file in the Server's file system, which the Client requested to copy to his own file system.
    The maximum size of this queue is given as argument in the ./dataServer programm from the command line. This queue
    is used by both Communication_Threads and Worker_Threads the following way; communication threads insert a file
    name along with the socket of the client, to whom that file should be sent and the worker threads remove the elements
    from the queue in order to send them to the right client. This queue is protected with a mutex so that only one
    thread has access to the queue each moment. They are also 2 conditional variables to which the threads are blocked 
    if they are no elements in the queue or if the queue is full.

2. Mutex_Socket_Queue:
    The elements of this Queue are pointers to structs, which contain an int and a pthread_mutex_t. The int is the 
    socket number for every Client, who is connected to the Server and assures the communication with him. Every socket
    is associated with a mutex. This queue is used by Worker_Threads the following way; when a Worker_Thread sends a file
    to a client, locks the mutex associated with the client's socket so that no other worker thread can send a file at
    the same time with another.

More information for the purpose of those queues below (see Server.c--Server.h)


-------------------------------------------------Server.c--Server.h-------------------------------------------------


These files contain the implementation of the dataServer after a client is connected to the server. The functions in 
these files, are basically the different tasks the server has to do from the moment a client is connected to him. The 
Server creates the "ThreadPool" (the Worker_Threads), right before he starts receiving connections from Clients. These 
are running in a loop and are waiting to detect new content in the "Files_Queue". When a new Client is connected to
the Server, then a new Communication_Thread is created, which communicates with this specific Client through a unique
socket number, which was acquired by both Server and Client when the connection was established. When either of the
sides closes the socket, the connection terminates.  

For the logic of the Communication_Threads - Worker_Threads synchronization, I followed the logic from the:
-- https://cgi.di.uoa.gr/~antoulas/k24/ --> threads-src/src/p65-prod-cons.c


--------------------------------------------------Client.c-Client.h-------------------------------------------------


These files contain the implementation of the remoteClient after the client is connected to the dataServer. The 
functions in these files, are basically the different tasks the client has to do from the moment he is connected to
the Server. The client creates the right directories after he has received a file from the server and creates a new
file in order to copy the content he receives from Server. After he gets all files, which were in the requested 
directory, he terminates.


---------------------------------------------------TCP / IP PROTOCOL------------------------------------------------


The TCP / IP protocol the server and the client use during their communication is presented briefly here.

1. REMOTECLIENT : sends the length of the directory name and the name of the directory, which he wants to copy
1. DATASERVER   : receives the length of the directory name and the name of the directory the client wants to copy
                  and checks if this is a valid directory in his file system

2. DATASERVER   : sends the length of the confirmation message and the confirmation message itself to the client
2. REMOTECLIENT : receives the length of the confirmation message and the confirmation message itself from the server 

3. REMOTECLIENT : sends confirmation to the server
3. DATASERVER   : receives confirmation from the client

4. DATASERVER   : sends block size to client
4. REMOTECLIENT : receives block size from server

5. REMOTECLIENT : sends confirmation to the server
5. DATASERVER   : receives confirmation from the client

6. while (!all_files_inserted_to_queue || !queue_full)
        7. DATASERVER   : reads directory content 
        8. DATASERVER   : inserts < absolute path of a file_name, socket > to the Files_Queue 
9. DATASERVER   : inserts < termination message, socket > to the Files_Queue 

10.while (!queue_empty)
        11. DATASERVER  : removes < absolute path of a file_name, socket > from the Files_Queue
        12. DATASERVER  : sends length of the file name to client
        13. DATASERVER  : sends file size to client
        14. DATASERVER  : sends file content block size by block size to client
10.while (!all_files_have_been_received)
        12. REMOTECLIENT: receives length of the file name from server
        13. REMOTECLIENT: receives file size  from server
        14. REMOTECLIENT: receives file content block size by block size  from server

15. while(!client_finished)
    REMOTECLIENT: sends confirmation ACK to server
15. while(!client_finished)
    DATASERVER  : receives confirmation ACK from client

16. DATASERVER  : send termination message to client
16. REMOTECLIENT: receives termination message from server

17. REMOTECLIENT: sends END message to server   
17. DATASERVER  : receives confirmation END from client

18. DATASERVER (the communication thread) TERMINATES
18. REMOTECLIENT TERMINATES


----------------------------------------------------dataServer.c----------------------------------------------------


It contains main function for dataServer. It contains argument checking and initialization and the steps until the
establishment of a connection between a client and the server from server's side (socket, bind, listen, connect).

Code skeleton (with small changes):
-- https://cgi.di.uoa.gr/~antoulas/k24/ --> sockets-src/src/prsref.c and sockets-src/src/inet_str_server.c


---------------------------------------------------remoteClient.c----------------------------------------------------


It contains main function for remoteClient. It contains argument checking and initialization and the steps until the
establishment of a connection between a client and the server from client's side (connect).

Code skeleton (with small changes):
-- https://cgi.di.uoa.gr/~antoulas/k24/ --> sockets-src/src/prs.c and sockets-src/src/inet_str_client.c


--------------------------------------------------multiple_clients.sh-----------------------------------------------

!! IMPORTANT !! This bash script will not run because I copied directories which were in my own account. It is just
to show how I ran my project. 

I made lots of experiments with various values combination in the command line arguments.
MAXIMUM VALUES I TESTED:
-- max_block_size 8192
-- max queue_size 20
-- max thread_pool size 20