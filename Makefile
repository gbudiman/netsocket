all: student admission department
	g++ Student.o StudentParser.o -o Student -lsocket -lnsl -lresolv
	g++ Admission.o AdmissionMessenger.o -o Admission -lsocket -lnsl -lresolv
	g++ Department.o DepartmentParser.o DepartmentMessenger.o -o Department -lsocket -lnsl -lresolv

student: student_parser
	g++ -c Student.cpp

student_parser:
	g++ -c StudentParser.cpp 
	
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
