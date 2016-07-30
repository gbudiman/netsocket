EE450 Socket Programming Project Phase 1

Name: Gloria Budiman
ID: 6528-1836-50

The submitted files include those necessary to run both Phase 1 and Phase 2. However, for Phase 1 submission, a flag ENABLE_PHASE_2 (in main.h) has been set to 0 to disable phase 2.

The project files and classes are as follows:

- Shared files:
  + main.h
    Header and global settings shared by all Department, Student, and Admission.
    This file also controls program flow (phase1 or phase1+2), assigned ports, and some programmer-friendly #define
  + Socket.cpp and Socket.hpp
    Class with reusable methods to create TCP socket, identify socket port, and obtain IP address.

- Admission:
  + Admission.cpp and Admission.h
    Main entry point for Admission.
    Upon execution, Admission overwrites database.txt with empty string. Then it opens TCP socket and wait for incomming connections. When incomming connection is detected, Admission will first parse the introduction to identify whether the sender is Department or Student. Then it will process the message and respond accordingly. All parsed messages will be written to database.txt.
    > If Phase 2 is disabled
      Database will count the number of Departments that have sent their data. When this number matches NUM_DEPTS (set to 3 in main.h), Database will notify Admission that all necessary data has been recorded. Admission will that exit with status 0, indicating completion of Phase 1.
    > If Phase 2 is enabled
      Database will wait until NUM_DEPTS and NUM_STUDENTS (both set to 3 and 5 respectively in main.h) have submitted their information. Under the assumption that Departments send their data before the Students do, for each packet sent by Student, Admission will check whether there is matching program offered by Department. This match count is tallied up for each student.
      * If the tally is zero
        The Admission will reply with '0'
      * If the tally is non-zero
        The Admission will reply with 'valid'
      Once done, Admission will close all TCP connections. Then it will instruct Database to make decision. Based on the decision result, Admission will then do the following:
      1. Create UDP sockets and send through packets to each Students which Admission has replied with 'valid' before
      2. Create UDP sockets and send through packets to each Deparments to indicate which Students have been admitted
      3. Close all UDP sockets. 
  + AdmissionMessenger.cpp and AdmissionMessenger.hpp
    Handles on-screen message
  + Database.cpp and Database.hpp
    Serves as a persistence module. Reads and writes plaintext file that contains data from Department, Student, and Admission result.
    Database will make the decision. For each student, it will traverse through a priority-list of program interest and check whether the student's GPA is equal to or greater than the minimum GPA requirement of said program. As soon as the check evaluates to true, the traversal will terminate and stores the admission decision. However, when traversal is complete but none of the checks evaluate to true, a rejection decision will be stored.
    Once all Students have been evaluated, Admission will read the decision and proceed with sending out admissions and/or rejections to both Students and Departments.

- Department:
  + Department.cpp and Department.h
    Main entry point for Department.
    Upon execution, it will fork NUM_DEPTS child process. Each child process will then open TCP connection to Admission server, introduce themselves and send their data. Once completed, TCP connection will be closed.
    > If Phase 2 is enabled
      Each child process will open UDP port for incoming admission result. Once all result has been received (indicated by ADM_END packet), the port will be closed.

    Once all child processes have returned, it will wait for 2 seconds for unflushed buffer then exit.
  + DepartmentMessenger.cpp and DepartmentMessenger.hpp
    Handles on-screen message
  + DepartmentParser.cpp and DepartmentParser.hpp
    Opens department[x].txt and parses them so the main program can process and transmit them accordingly

- Student:
  + Student.cpp and Student.h
    Main entry point for Student.
    Upon execution, it will fork NUM_STUDENTS child process. Each child process will then open TCP connection to Admission server, introduce themselves and send their data. Based on Admission's reply, Student will either terminate or proceed to the next step.

    * If Admission replies with '0' 
      It indicates the application matches no program. Student process will immediately return.
    * If Admission replies with 'valid'
      It indicates there are at least 1 matching program. Student process will then open UDP port to receive admission result. Once all result has been received (indicated by ADM_END packet), the port will be closed. Process will then return.
  + StudentMessenger.cpp and StudentMessenger.hpp
    Handles on-screen message
  + StudentParser.cpp and StudentParser.hpp
    Opens student[y].txt and parses them so the main program can process and transmit them accordingly

- database.txt
  Plaintext file as a scratchpaper for persistence module. Included to avoid writable permission issue. This file will not be deleted. It can only be overwritten or appended to.

- Utility files
  + rsync.sh
    Rsync with nunki
  + Makefile

Compilation:
nunki.usc.edu(11): make
g++ -c StudentParser.cpp 
g++ -c StudentMessenger.cpp
g++ -c Student.cpp
g++ -c AdmissionMessenger.cpp
g++ -c Database.cpp
g++ -c Admission.cpp
g++ -c DepartmentParser.cpp
g++ -c DepartmentMessenger.cpp
g++ -c Department.cpp
g++ -c Socket.cpp
g++ Student.o StudentParser.o StudentMessenger.o Socket.o -o Student -lsocket -lnsl -lresolv
g++ Admission.o AdmissionMessenger.o Database.o Socket.o -o Admission -lsocket -lnsl -lresolv
g++ Department.o DepartmentParser.o DepartmentMessenger.o Socket.o -o Department -lsocket -lnsl -lresolv

Running:
> Admission
> Department
> Student
Note: 
- must be run from 3 different terminals, otherwise the STDOUT from parent process will not display properly
- Program does not handle malfunctions caused by:
  > Unclosed port. Program will simply terminate
  > Incorrect order of execution. Program will stall due to incorrect FSM path and may leave zombie processes
  > Any other system malfunctions that's beyond project's scope such as out-of-memory or broken DNS resolution

Sample on-screen message can be seen in pcap/onscreen.txt.
Sample packets captured using Wireshark can be seen in pcap/sample.txt

Code pertaining to TCP/UDP socket are legally plagiarized with modification from BeeJ's Guide to Network Programing :)