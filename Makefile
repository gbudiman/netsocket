all: student admission department
	g++ Student.o StudentParser.o -o Student
	g++ Admission.o -o Admission
	g++ Department.o DepartmentParser.o -o Department

student: student_parser
	g++ -c StudentParser.o Student.cpp Student.h main.h

student_parser:
	g++ -c StudentParser.cpp StudentParser.hpp main.h
	
admission: Admission.cpp
	g++ -c Admission.cpp Admission.h main.h

department: department_parser
	g++ -c Department.cpp Department.h main.h

department_parser:
	g++ -c DepartmentParser.cpp DepartmentParser.hpp main.h

clean:
	rm *.o *.gch Student Admission Department *.out
