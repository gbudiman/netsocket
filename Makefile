all: student admission department
	g++ Student.o StudentParser.o -o Student -lsocket -lnsl -lresolv
	g++ Admission.o -o Admission -lsocket -lnsl -lresolv
	g++ Department.o DepartmentParser.o -o Department -lsocket -lnsl -lresolv

student: student_parser
	g++ -c StudentParser.o Student.cpp Student.h main.h -lsocket -lnsl -lresolv

student_parser:
	g++ -c StudentParser.cpp StudentParser.hpp main.h
	
admission: Admission.cpp
	g++ -c Admission.cpp Admission.h main.h -lsocket -lnsl -lresolv

department: department_parser
	g++ -c Department.cpp Department.h main.h -lsocket -lnsl -lresolv

department_parser:
	g++ -c DepartmentParser.cpp DepartmentParser.hpp main.h

clean:
	rm *.o *.gch Student Admission Department *.out
