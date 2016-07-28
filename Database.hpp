#ifndef Database_hpp
#define Database_hpp

#include "main.h"

#define DB_FILE "database.txt"

class Database {
public:
  Database();
  bool check_is_complete();
  bool check_is_complete(bool);
  bool has_program(char *);
  void build();
  void make_decision();
  void debug_decision();
  
  std::map<int, float> *student_grades;
  std::vector<std::string> *decision;
private:
  void process_student_data(std::string);
  void process_department_data(std::string);
  void debug_database();
  
  std::map<int, std::vector<std::string>*> *student_interests;
  std::map<std::string, float> *department_programs;
};

#endif
