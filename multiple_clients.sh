#!/bin/bash

#for my pc
port=9000
IP=127.0.0.1
VALGRIND="valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes "
./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/sockets > text1.txt &
./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/sockets > text2.txt &
./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/sockets > text3.txt &
./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/make_example > text4.txt &
./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/make_example > text5.txt &
./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/sockets > text6.txt &
./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/sockets > text7.txt &
./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/make_example > text8.txt &
./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/threads-src > text9.txt &
./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/make_example > text10.txt &

# $VALGRIND ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/sockets > text1.txt &
# $VALGRIND ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/sockets > text2.txt &
# $VALGRIND ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/sockets > text3.txt &
# $VALGRIND ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/make_example > text4.txt &
# $VALGRIND ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/make_example > text5.txt &
# $VALGRIND ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/sockets > text6.txt &
# $VALGRIND ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/sockets > text7.txt &
# $VALGRIND ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/make_example > text8.txt &
# $VALGRIND ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/threads-src > text9.txt &
# $VALGRIND ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/make_example > text10.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/bash-examples > text1.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/sockets > text2.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/System_Programming_Exec1_Sniffer/files > text3.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/make_example > text4.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/make_example > text5.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/System_Programming_Exec1_Sniffer/sources > text6.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/errors-io > text7.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/threads-src > text8.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/threads-src > text9.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/make_example > text10.txt &
# $VALGRIND ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/bash-examples > text1 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/sockets > text2 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/System_Programming_Exec1_Sniffer/files > text3 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/make_example > text4 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/make_example > text5 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/System_Programming_Exec1_Sniffer/sources > text6 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/errors-io > text7 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/threads-src > text8 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/threads-src > text9 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/make_example > text10 &
#for limux
# ./remoteClient -i $IP -p $port -d ./Syspro > text1 &
# ./remoteClient -i $IP -p $port -d ./B_SEMESTER > text2 &
# ./remoteClient -i $IP -p $port -d ./E_SEMESTER > text3 &
# ./remoteClient -i $IP -p $port -d ./C_SEMESTER > text4 &
# ./remoteClient -i $IP -p $port -d ./Project > text5 &
# ./remoteClient -i $IP -p $port -d ../BF_lib_22_12 > text6 &
# ./remoteClient -i $IP -p $port -d ../Downloads > text7 &
# ./remoteClient -i $IP -p $port -d ../Desktop > text8 &
# ./remoteClient -i $IP -p $port -d ./B_SEMESTER > text9 &
# ./remoteClient -i $IP -p $port -d ../BF_lib_22_12 > text10 &
# ./remoteClient -i 0.0.0.0 -p $port -d ./Syspro > text1 &
# ./remoteClient -i 0.0.0.0 -p $port -d ./B_SEMESTER > text2 &
# ./remoteClient -i 0.0.0.0 -p $port -d ./E_SEMESTER > text3 &
# ./remoteClient -i 0.0.0.0 -p $port -d ./C_SEMESTER > text4 &
# ./remoteClient -i 0.0.0.0 -p $port -d ./Project > text5 &
# ./remoteClient -i 0.0.0.0 -p $port -d ../BF_lib_22_12 > text6 &
# ./remoteClient -i 0.0.0.0 -p $port -d ../Downloads > text7 &
# ./remoteClient -i 0.0.0.0 -p $port -d ../Desktop > text8 &
# ./remoteClient -i 0.0.0.0 -p $port -d ./B_SEMESTER > text9 &
# ./remoteClient -i 0.0.0.0 -p $port -d ../BF_lib_22_12 > text10 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ./Syspro > text1 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ./B_SEMESTER > text2 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ./E_SEMESTER > text3 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ./C_SEMESTER > text4 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ./Project > text5 &