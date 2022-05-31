#!/bin/bash

#for my pc
port=9000
IP=127.0.0.1
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
# valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/bash-examples > text1 &
# valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/sockets > text2 &
# valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/System_Programming_Exec1_Sniffer/files > text3 &
# valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/make_example > text4 &
# valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/make_example > text5 &
# valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/System_Programming_Exec1_Sniffer/sources > text6 &
# valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/errors-io > text7 &
# valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/ipc-src > text8 &
# valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/threads-src > text9 &
# valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./remoteClient -i 127.0.0.1 -p 9000 -d ../SEMESTER_6/SYSPRO/make_example > text10 &
#for limux
#./remoteClient -i 127.0.0.1 -p $port -d ./Syspro &
./remoteClient -i 127.0.0.1 -p $port -d ./B_SEMESTER &
# ./remoteClient -i 127.0.0.1 -p 9000 -d ./E_SEMESTER > text3 &
# ./remoteClient -i 127.0.0.1 -p 9000 -d ./C_SEMESTER > text4 &
# ./remoteClient -i 127.0.0.1 -p 9000 -d ./Project > text5 &
# ./remoteClient -i 127.0.0.1 -p 9000 -d ../BF_lib_22_12 > text6 &
# ./remoteClient -i 127.0.0.1 -p 9000 -d ../Downloads > text7 &
# ./remoteClient -i 127.0.0.1 -p 9000 -d ../Desktop > text8 &
# ./remoteClient -i 127.0.0.1 -p 9000 -d ./B_SEMESTER > text9 &
# ./remoteClient -i 127.0.0.1 -p 9000 -d ../BF_lib_22_12 > text10 &
# ./remoteClient -i 0.0.0.0 -p 9000 -d ./Syspro > text1 &
# ./remoteClient -i 0.0.0.0 -p 9000 -d ./B_SEMESTER > text2 &
# ./remoteClient -i 0.0.0.0 -p 9000 -d ./E_SEMESTER > text3 &
# ./remoteClient -i 0.0.0.0 -p 9000 -d ./C_SEMESTER > text4 &
# ./remoteClient -i 0.0.0.0 -p 9000 -d ./Project > text5 &
# ./remoteClient -i 0.0.0.0 -p 9000 -d ../BF_lib_22_12 > text6 &
# ./remoteClient -i 0.0.0.0 -p 9000 -d ../Downloads > text7 &
# ./remoteClient -i 0.0.0.0 -p 9000 -d ../Desktop > text8 &
# ./remoteClient -i 0.0.0.0 -p 9000 -d ./B_SEMESTER > text9 &
# ./remoteClient -i 0.0.0.0 -p 9000 -d ../BF_lib_22_12 > text10 &