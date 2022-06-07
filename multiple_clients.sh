#!/bin/bash

#for my pc
port=9000
#IP=127.0.0.1
IP=195.134.65.75
VALGRIND="valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes "

#for my laptop with same directories  -- very small files
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/bla1 > text1.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/bla1 > text2.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/bla2 > text3.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/bla3 > text4.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/bla3 > text5.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/bla2 > text6.txt &

#for my laptop with same directories
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/sockets > text1.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/sockets > text2.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/sockets > text3.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/make_example > text4.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/make_example > text5.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/sockets > text6.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/sockets > text7.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/make_example > text8.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/threads-src > text9.txt &
# ./remoteClient -i $IP -p $port -d ../SEMESTER_6/SYSPRO/make_example > text10.txt &
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

#for my laptop with different directories
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

#for university linux
./remoteClient -i $IP -p $port -d ./sdi1800062/Documents/Syspro > text1 &
./remoteClient -i $IP -p $port -d ./sdi1800062/Documents/B_SEMESTER > text2 &
./remoteClient -i $IP -p $port -d ./sdi1800062/Documents/E_SEMESTER > text3 &
./remoteClient -i $IP -p $port -d ./sdi1800062/Documents/C_SEMESTER > text4 &
./remoteClient -i $IP -p $port -d ./sdi1800062/Documents/Project > text5 &
./remoteClient -i $IP -p $port -d ./sdi1800062/Downloads > text6 &
./remoteClient -i $IP -p $port -d ./sdi1800062/Documents/B_SEMESTER > text7 &
./remoteClient -i $IP -p $port -d ./sdi1800062/Documents/B_SEMESTER/ERGASIA1 > text8 &
./remoteClient -i $IP -p $port -d ./sdi1800062/Downloads/bla1 > text9 &
./remoteClient -i $IP -p $port -d ./sdi1800062/Downloads/bla2 > text10 &
./remoteClient -i $IP -p $port -d ./sdi1800062/Downloads/bla3 > text11 &

# $VALGRIND ./remoteClient -i $IP -p $port -d ./sdi1800062/Documents/Syspro > text1 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ./sdi1800062/Documents/ > text2 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ./sdi1800062/Documents/E_SEMESTER > text3 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ./sdi1800062/Documents/C_SEMESTER > text4 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ./sdi1800062/Documents/Project > text5 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ./sdi1800062/Downloads > text6 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ./sdi1800062/Documents/B_SEMESTER > text7 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ./sdi1800062/Documents/B_SEMESTER/ERGASIA1 > text8 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ./sdi1800062/Downloads/bla1 > text9 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ./sdi1800062/Downloads/bla2 > text10 &
# $VALGRIND ./remoteClient -i $IP -p $port -d ./sdi1800062/Downloads/bla3 > text11 &