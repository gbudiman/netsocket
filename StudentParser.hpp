#ifndef StudentParser_hpp
#define StudentParser_hpp

#include "main.h"

#define FILENAME_PREFIX "student"
#define TOKEN_GPA "GPA"
#define TOKEN_INTEREST "Interest"
#define TOKEN_SEPARATOR ":"

class StudentParser {
public:
  StudentParser(uint32_t);
  
  float gpa;
  std::vector<std::string>* interests;
private:
  void parse();
  
  uint32_t id;
  char filepath[255];
};

#endif
