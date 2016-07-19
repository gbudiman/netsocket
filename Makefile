all: student admission department
	g++ Student.o StudentParser.o -o Student -lsocket -lnsl -lresolv
	g++ Admission.o AdmissionMessenger.o -o Admission -lsocket -lnsl -lresolv
	g++ Department.o DepartmentParser.o DepartmentMessenger.o -o Department -lsocket -lnsl -lresolv

student: student_parser
	g++ -c StudentParser.o Student.cpp Student.h

student_parser:
	g++ -c StudentParser.cpp StudentParser.hpp
	
admission: admission_messenger
	g++ -c Admission.cpp Admission.h

admission_messenger: 
	g++ -c AdmissionMessenger.cpp AdmissionMessenger.hpp

department: department_parser department_messenger
	g++ -c Department.cpp Department.h main.h

department_parser:
	g++ -c DepartmentParser.cpp DepartmentParser.hpp

department_messenger:
	g++ -c DepartmentMessenger.cpp DepartmentMessenger.hpp

clean:
	rm *.o *.gch Student Admission Department *.out
