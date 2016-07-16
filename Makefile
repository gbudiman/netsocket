all: student admission department
	g++ Student.o -o Student
	g++ Admission.o -o Admission
	g++ Department.o -o Department

student: Student.cpp
	g++ -c Student.cpp Student.h main.h

admission: Admission.cpp
	g++ -c Admission.cpp Admission.h main.h

department: Department.cpp
	g++ -c Department.cpp Department.h main.h

clean:
	rm *.o *.gch Student Admission Department *.out