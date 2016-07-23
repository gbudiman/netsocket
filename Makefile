all: student admission department socket
	g++ Student.o StudentParser.o StudentMessenger.o Socket.o -o Student -lsocket -lnsl -lresolv
	g++ Admission.o AdmissionMessenger.o Socket.o -o Admission -lsocket -lnsl -lresolv
	g++ Department.o DepartmentParser.o DepartmentMessenger.o Socket.o -o Department -lsocket -lnsl -lresolv

mac: student admission department socket
	g++ Student.o StudentParser.o StudentMessenger.o Socket.o -o Student
	g++ Admission.o AdmissionMessenger.o Socket.o -o Admission
	g++ Department.o DepartmentParser.o DepartmentMessenger.o Socket.o -o Department

socket: 
	g++ -c Socket.cpp

student: student_parser student_messenger
	g++ -c Student.cpp

student_parser:
	g++ -c StudentParser.cpp 

student_messenger:
	g++ -c StudentMessenger.cpp
	
admission: admission_messenger
	g++ -c Admission.cpp

admission_messenger: 
	g++ -c AdmissionMessenger.cpp

department: department_parser department_messenger
	g++ -c Department.cpp

department_parser:
	g++ -c DepartmentParser.cpp

department_messenger:
	g++ -c DepartmentMessenger.cpp

clean:
	rm *.o *.gch Student Admission Department *.out
