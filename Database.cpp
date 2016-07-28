#include "Database.hpp"

Database::Database() {
  
}

bool Database::has_program(char *x) {
  std::ifstream infile(DATABASE_FILE);
  std::string line;
  bool is_valid_program = false;
  
  while(std::getline(infile, line)) {
    char name[3] = "";
    if (line[0] == 'S') {
      continue;
    } else {
      name[0] = line[0];
      name[1] = line[1];
      name[2] = '\0';
      
      if (strcmp((const char*) x, (const char*) name) == 0) {
        is_valid_program = true;
        break;
      }
    }
  }
  
  infile.close();
  return is_valid_program;
}

bool Database::check_is_complete(bool execute_build) {
  //sleep(2);
  std::set<std::string> *members = new std::set<std::string>();
  std::ifstream infile(DATABASE_FILE);
  std::string line;
  int limit = ENABLE_PHASE_2 ? NUM_DEPTS + NUM_STUDENTS : NUM_DEPTS;
  
  while(std::getline(infile, line)) {
    std::string name = "";
    
    if (line[0] == 'S') {
      name = line[0] + line[1];
      if (execute_build) {
        process_student_data(line);
      }
    } else {
      name = line[0];
      if (execute_build) {
        process_department_data(line);
      }
    }
    
    members->insert(name);
  }
  
  if (members->size() == limit) {
    return true;
  }
  
  infile.close();
  return false;
}

bool Database::check_is_complete() {
  return Database::check_is_complete(false);
}

void Database::build() {
  student_grades = new std::map<int, float>();
  student_interests = new std::map<int, std::vector<std::string>*>();
  department_programs = new std::map<std::string, float>();
  decision = new std::vector<std::string>();
  
  check_is_complete(true);
  if (PROJ_DEBUG) {
    debug_database();
  }
}

void Database::process_student_data(std::string x) {
  char *element;
  uint32_t e_pos = 0;
  char *buffer = new char[x.length() + 1];
  strcpy(buffer, x.c_str());
  element = strtok((char*) buffer, "#");
  int student_id = -1;
  bool parse_gpa = false;
  std::string interest = "";
  std::ofstream f;
  float student_gpa = 0.0;
  
  while (element != NULL) {
    switch(e_pos) {
      case 0:
        student_id = element[1] - 0x30;
        if (PROJ_DEBUG) {
          std::cout << "Captured student ID: " << student_id << "\n";
        }
        break;
      case 1:
        if (strcmp(element, "GPA") == 0) {
          parse_gpa = true;
        } else {
          interest = element;
          if (PROJ_DEBUG) {
            std::cout << "Capture interest of Student " << student_id << ": " << interest << "\n";
          }
        }
        break;
      case 2:
        if (parse_gpa) {
          student_gpa = std::atof(element);
          if (PROJ_DEBUG) {
            std::cout << "Captured GPA of Student " << student_id << ": " << student_gpa << "\n";
          }
        }
        break;
      default:
        std::cerr << "Shouldn't reach here\n";
    }
    
    element = strtok(NULL, "#");
    e_pos++;
  }
  
  if (parse_gpa) {
    if (student_grades->find(student_id) == student_grades->end()) {
      // student not found
      student_grades->insert(std::make_pair(student_id, student_gpa));
    } else {
      student_grades->at(student_id) = student_gpa;
    }
  } else {
    if (student_interests->find(student_id) == student_interests->end()) {
      // no interest found
      std::vector<std::string> *k = new std::vector<std::string>();
      student_interests->insert(std::make_pair(student_id, k));
    }
    
    student_interests->at(student_id)->push_back(interest);
  }
}

void Database::process_department_data(std::string x) {
  char *element;
  uint32_t e_pos = 0;
  char *buffer = new char[x.length() + 1];
  strcpy(buffer, x.c_str());
  element = strtok((char*) buffer, "#");
  std::string dept_program = "";
  float dept_program_min_gpa = 0;
  
  while (element != NULL) {
    switch(e_pos) {
      case 0:
        dept_program = element;
        break;
      case 1:
        dept_program_min_gpa = std::atof(element);
        break;
    }
    element = strtok(NULL, "#");
    e_pos++;
  }
  
  if (department_programs->find(dept_program) == department_programs->end()) {
    department_programs->insert(std::make_pair(dept_program, dept_program_min_gpa));
  } else {
    department_programs->at(dept_program) = dept_program_min_gpa;
  }
}

void Database::debug_database() {
  for (std::map<int, float>::iterator g = student_grades->begin(); g != student_grades->end(); ++g) {
    std::cout << "Student" << g->first << " GPA: " << g->second << "\n";
  }
  
  for (std::map<int, std::vector<std::string>*>::iterator s = student_interests->begin(); s != student_interests->end(); ++s) {
    std::cout << "Student" << s->first << " Interests: ";
    for (std::vector<std::string>::iterator ss = s->second->begin(); ss != s->second->end(); ++ss) {
      std::cout << ss->c_str() << " ";
    }
    std::cout << "\n";
  }
  
  for (std::map<std::string, float>::iterator r = department_programs->begin(); r!= department_programs->end(); ++r) {
    std::cout << "Department Program " << r->first << " min GPA: " << r->second << "\n";
  }
}

void Database::make_decision() {
  for (std::map<int, float>::iterator g = student_grades->begin(); g != student_grades->end(); ++g) {
    int student_id = g->first;
    int matching_interest = 0;
    char student_id_s[MAXDATASIZE] = "";
    float student_gpa = g->second;
    bool admitted = false;
    
    sprintf(student_id_s, "%d", student_id);
    
    for (std::vector<std::string>::iterator p = student_interests->at(student_id)->begin(); p != student_interests->at(student_id)->end() && !admitted; ++p) {
      
      if (department_programs->find(*p) != department_programs->end()) {
        matching_interest++;
        std::string program_application = *p;
        float min_gpa = department_programs->at(*p);
        
        int min_gpa_rd = round(min_gpa * 10);
        int student_gpa_rd = round(student_gpa * 10);
        
        if (PROJ_DEBUG) {
          std::cout << student_id << ": " << student_gpa_rd << " >=? " << min_gpa_rd << "\n";
        }
        
        if (student_gpa_rd >= min_gpa_rd) {
          std::string dec_s = "";
          dec_s += (std::string) student_id_s + "#Accept#" + *p + "#department" + p->at(0);
          decision->push_back(dec_s);
          
          if (PROJ_DEBUG) {
            std::cout << "   Accept: " << p[0] << "\n";
          }
          admitted = true;
        }
      }
    }
    
    if (!admitted && matching_interest > 0) {
      std::string dec_s = "";
      dec_s += (std::string) student_id_s + "#Reject";
      decision->push_back(dec_s);
    }
  }
}

void Database::debug_decision() {
  for (std::vector<std::string>::iterator d = decision->begin(); d != decision->end(); ++d) {
    std::cout << *d << "\n";
  }
}
