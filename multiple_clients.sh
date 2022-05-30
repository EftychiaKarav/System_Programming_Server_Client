#!/bin/bash

# ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/bash-examples > text1 &
# ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/sockets > text2 &
# ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/System_Programming_Exec1_Sniffer/files > text3 &
# ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/make_example > text4 &
# ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/make_example > text5 &
# ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/System_Programming_Exec1_Sniffer/sources > text6 &
# ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/errors-io > text7 &
# ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/ipc-src > text8 &
# ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/threads-src > text9 &
# ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/make_example > text10 &
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/bash-examples > text1 &
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/sockets > text2 &
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/System_Programming_Exec1_Sniffer/files > text3 &
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/make_example > text4 &
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/make_example > text5 &
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/System_Programming_Exec1_Sniffer/sources > text6 &
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/errors-io > text7 &
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/ipc-src > text8 &
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/threads-src > text9 &
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/make_example > text10 &