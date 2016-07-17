#ifndef DepartmentParser_hpp
#define DepartmentParser_hpp

#include "main.h"

#define FILENAME_PREFIX "department"
#define TOKEN_SEPARATOR "#"

class DepartmentParser {
public:
  DepartmentParser(std::string);
  std::map<std::string, float>* requirements;
private:
  void parse();
  
  std::string dept_name;
  char filepath[255];
};

#endif
