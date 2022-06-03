KARAVANGELI EFTYCHIA - 1115201800062

***************************************************USEFUL NOTES*****************************************************

- In this file only genaral information is presented. It concerns compilation and a high level description of what 
each file of the project contains.
- There are highly detailed and descriptive comments in all code files which are very explanatory.
- If -during execution- the program presents any errors/failures it will exit. An error message will be dispalyed.
However, if it presents any errors/failures and does not exit, then its behavior will be undefined.
- remoteClients exit themselves after getting the requested directory from the dataServer
- dataServer terminates after typing ^C (which is being handled), but it can also stop with ^Z, which has the default
behavior.
- multiple_clients.sh (end of this file) is a script I used to test my project. You can see what kinds of experiments
I made.


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

source: https://cgi.di.uoa.gr/~antoulas/k24/ --> sockets-src/src/prs.c and sockets-src/src/prs.c


-------------------------------------------------Queue.c--Queue.h---------------------------------------------------

!These files were used at the previous project as well. A few alterations were made due to the project's purposeses!
They contain a FIFO queue implementation with pointers (like a list). The nodes of the queue are pointers to a struct
called Queue_Node. This struct contains an int and a pointer to union called Data_Node. The union contais a char* and 
a mutex. 
In this project, Queue is used to save information for 2 entities.
1. Files_Queue         --> uses the int and the char*
2. Mutex_Socket_Queue  --> uses the int and the mutex

More information for the purpose of those queues below (see Server.c--Server.h)












**************** will seeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee*********************
1. H oura einai FIFO kai sygkekrimena gia ka8e worker apo8hkeuetai to pid tou kai to onoma tou named_pipe (FIFO),
to opoio xrhsimeuei sthn epikoinvnia tou me ton manager. Dhladh otan o manager dialegei enan worker gia na toy 
ana8esei ena arxeio, aytomata gnvrizei kai to onoma tou FIFO pou syndeei ton manager me ton sygkekrimeno worker.
Ka8e zeugari manager-worker exei monadiko onoma gia to named_pipe tou. Auta fysika einai diaforetika metaxi tous, 
giati yparxei periptwsh thn idia stigmh na einai anoixta parapanw apo ena named_pipes. To onoma tou ka8e named_pipe 
exei to akoloutho pattern "MW_FIFO_[manager_pid]_[worker_pid]", opou manager_pid kai worker_pid einai ta antistoixa 
pids. 
H leitourgia einai h exhs: otan kapoios worker einai busy, den vrisketai sthn oura. Eiserxetai sto telos ths ouras,
otan oloklhrwnei thn ergasia tou, dhladh to process tou arxeiou poy analamvanei. Otan yparxoun diathesimoi workers 
sthn oura, o manager epilegei ton 1o apo autous kai tou ana8etei na epexergastei to arxeio, to opoio o manager 
anixneuse apo ton listener. Ara autos o worker e3erxetai apo thn oura.

2. H oura xrhsimopoieitai kai gia allo skopo; gia na apouhkeuei ta links pou anazhtoun oi workers sta diafora
arxeia. Ousiastika edw paizei to rolo listas kai krataei plhroforia gia to onoma tou ka8e link mazi me poses fores 
emfanizetai se ena arxeio. Ginetai mia aplh anazhthsh sta hdh yparxonta links. An to link yparxei, apla au3anoume 
ta occurences, alliws to pros8etoume sthn oura.


-------------------------------------------------Server.c--Server.h-------------------------------------------------

These files contain the implementation of the dataServer after a client is connected to the server. The functions in 
these files, are basically the different tasks the server has to do from the moment a client is connected to him. The 
Server creates the "ThreadPool" (the Worker_Threads), right before he starts receiving connections from Clients. When 
a new Client is connected to the Server, then a new Communication_Thread is created, which communicates with this
specific Client through a unique socket number, which was acquired by both Server and Client when the connection was
established. When either of the sides closes the socket, the connection terminates.  


--------------------------------------------------Client.c-Client.h-------------------------------------------------


---------------------------------------------------TCP / IP PROTOCOL------------------------------------------------
DATASERVER
1. 
2.
3.
4.
functions are implemented Periexoun tous Signal handlers gia na diaxeirizetai sygkekrimena signals ston manager kai stous workers. 

1. MANAGER  --> exei signal handlers gia na diaxeirizetai:
    a. to SIGINT, otan o xrhsths plhktrologhsei ^C gia na termatisei to programma. Allazei to mode tou fd[READ] tou 
    pipe se O_NONBLOCK opote o manager 3eblokarei apo thn read me ton listener kai sth synexeia allazei to RUNNING 
    tou manager se false opote stamataei.
    b. to SIGCHLD me dyo diaforetikous tropous (enan otan oi workers stamatoun h synexizoun th douleia tous kai 
    enan gia otan termatizoun)

2. WORKERS  --> exoun signal handler gia na diaxeirontai:
    a. to SIGTSTP pou tous steleni o manager otan einai wra na termatisei to programma, kata to opoio allazei o 
    RUNNING tous se false.


--------------------------------------------------Client.c-Client.h-------------------------------------------------

Periexoun synarthseis pou syn8etoun th douleia pou ektelei o manager kath8s epishs kai th douleia tou listener.
1. O listener pou dhmioutgeitai me fork() apo ton manager ektelei me execlp thn inotifywait me orismata -m (gia na 
kanei monitor synexeia) kai me events "create" kai "moved_to". To apotelesma exei to parakatw format:
    directory/filename

Epeidh by default to apotelesma apo thn inotifywait emfnizetai sto stdout, kanoume redirection tou stdout sto akro 
pou pipe pou einai gia write.
2. O manager perimenei stamathmenos sth read na diabasei kapoio arxeio apo to allo akro tou pipe, auto tou read, me 
to opoio epikoinwnei me ton listener. An h read diakopei apo syscall, kanoume catch to errno == EINTR kai 
3anadiavazoume.
3. Ginetai elegxos gia to an h oura me tous workers einai adeia h oxi. An yparxei diathesimos worker sthn oura 
ginetai pop (pairnoume to pid kai FIFO name pou antistoixei sto sygkekrimeno zeygari manager-worker) kai o manager 
tou stelnei SIGCONT na synexisei. An oxi, o manager me fork() dhmiourgei kainourio worker kai to onoma tou 
antistoixou FIFO gia na epe3ergastei to neo arxeio pou elave o manager apo ton listener.
4. O manager anoigei to FIFO sto opoio thelei na grapsei to onoma tou arxeiou poy elave apo ton listener, to 
grafei, kleinei to file descriptor tou FIFO pou anoi3e kai 3anagyrnaei pali sthn arxh perimenontas na diavasei neo 
arxeio pou tou esteile o listener.
5. Otan path8ei to ^C gia na termatisei to programma, to parapanw loop (2-4) stamataei na ekteleitai kai o manager 
termatizei tous workers me ton e3hs tropo. Tous stelnei prwta SIGCONT gia na fygoun apo thn stopped katastash pou 
vriskontai kai sth syneceia SIGTSTP gia na allaxei to RUNNING se false kai na diakopei to loop tous.
6. O manager stelnei SIGKILL ston listener gia na ton termatisei.
7. Yparxei kai h synarthsh gia thn apodesmeush ths mnhmhs gia oles tis diergasies.
 

----------------------------------------------------dataServer.c----------------------------------------------------

Periexoun synarthseis pou syn8etoun th douleia pou ekteloun oi workers. 

1. O ka83 worker molis dhmiourgeitai dhmiourgei to onoma tou FIFO mesw tou opoiou epikoinwnei me ton manager, opws 
e3hgh8hke sto "Queue.c - Queue.h".
2. Ginetai to establishment mesw ths sigaction tou signal handler "Finish_Worker" gia to signal SIGTSTP.
3. Anoigei to file descriptor tou FIFO gia na mporei na diavazei oti grafei o manager sto allo akro tou FIFO.
4. Perimenei na diavasei (opws kai o manager parapanw) to onoma tou arxeiou pou egrapse o manager sto FIFO.
5. Diavazei to onoma tpu arxeiou.
6. Anoigei to arxeio kai 3ekinaei na to diavazei wste na vrei ta links.
7. Ta links apo8hkeuontai opws eipame parapanw se mia oura mazi me ton ariumo tvn emfanisewn tous mesa se ena 
sygkekrimeno arxeio.
8. Dhmiourgei kai anoigei to .out arxeio, wste na grapsei ta apotelesmata pou diavase. (!! an to arxeio yparxei hdh 
ginetai overwrite !!)
9. Grafei ta links mazi me ton arithmo twn emfanisewn touw. 
10. Kleinei ola ta arxeia.
11. Epanalmvanei ta vhmata (4-10) mexri na lavei mhnyma apo ton manager me SIGTSTP na stamthsei.
12. Otan stamathsei, kleinei to fd tou FIFO tou, apodesmeuei mnhmh kai termatizei.


---------------------------------------------------remoteClient.c----------------------------------------------------

Periexei th main function tou programmatos. 
1. Arxika ginetai kapoios elegxos sxetika me ta orismata sth grammh entolwn.
2. Meta dhmiourgountai oi directories gia thn apo8hkeush tvn FIFOs kai tvn .out arxeiwn.
3. Sthn arxh prin thn dhmiourgia tou listener, o manager agnoei ola ta signals, wste o listenr na ta klhronomhsei 
kai na agnohsei to SIGINT otan 8a to pathsei o xrhsths.
4. Dhmiourgeitai to pipe gia thn epikoinwnia manager - listener.
5. Dhmiourgeitai o listener me fork() apo thn main (manager).
6. Ginetai to establishment mesw ths sigaction tou signal handler "Inform_Manager" gia to signal SIGINT kai tou 
Signal_from_Child gia to SIGCHLD.
7. Kaleitai h synarthsh tou manager (vlepe parapanw) mesa sthn opoia yparxei olh h ylopoihsh twn zhtoumenwn ths 
ergasias.
8. Otan o xrhsths plhktrologhsei ^C gia na termatisei to programma, tote o signal handler gia to SIGCHLD ginetai o 
Wait_Child kai o manager sth main eidopoiei ton listener (me SIGKILL) kai tous workers (me SIGTSTP) na termatisoun.
9. Perimenei na termatisoun oles oi diergasies.
10. Apodesmeuetai h mnhmh poy egine allocate (gia oles tis diergasies, fysika otan termatizei h ka8emia oxi 
tautoxrona gia oles mazi).
11. Kleinoun oi file descriptors gia stdin, stdout, stderr.
12. Termatizei to programma.


--------------------------------------------------multiple_clients.sh-----------------------------------------------

!! IMPORTANT !! Yparxei sto makefile entolh gia na dinontai execute rights gia to bash script. An gia kapoio logo, 
den doulepsei parakalw na tre3ete thn entolh:
chmod +x finder.sh

To Bash Script psaxnei kai metraei ta TLDs opws ayta prosdiorizontai sto cammand line sto synolo olwn twn .out 
arxeiwn pou proekypsan apo thn ektelesh tou "./sniffer" anexarthta apo to poio path do8hke ston sniffer san orisma. 
Ola ta .out arxeia vriskontai sto OUT_FILES/ directory. Na mhn allax8ei to path pou tha trexei to script. Na trexei 
apo ekei pou hdh vrisketai. Vevaiw8eite oti ola ta onomata tvn arxeiwn apotelountai apo mia symboloseira kai oxi 
apo parapanw (e.g. file_1 not file 1), alliws ta arxeia den tha anoigoun kai to finder.sh tha emfanisei error. 

!! IMPORTANT !! Ta TLDs na perastoun san orismata xwris to '.' (e.g. com gr org NOT .com .gr .org)
Ta apotelesmata tha emfanizontai sto stdout mazi me plhroforia sxetika me to se poia arxeia egine h anazhthsh (ola 
osa vriskontai mesa ston OUT_FILES directory otan trexei to script).
Yparxei enas sxetikos elegxos lathwn sto script an den yparxei o katalogos h an ta arxeia den exoun periexomeno. 
Gia thn prvth anazhthsh xrhsimopoieitai h grep kai kanei match ta strings pou 3ekinoun me '.' kai teleiwnoun me 
whitespace gia ka8e TLD. Dhladh an anazhtoume to ".com" sthn periptwsh tou ".com.br", to ".com" den 8a prosmetrh8ei 
sto apotelesma, giati to TLD einai to "br". Perissoteres leptomereies yparxoun sta sxolia sto finder.sh. 