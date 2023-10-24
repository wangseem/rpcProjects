/****************************************************
 *
 * ****     ***     ****    *   *    *****    *****
 * *  *    *   *   *        ****     ***        *
 * *   *    ***     ****    *   *    *****      *
 *
 * business_exception.cc
 * 2023-10-16 15:23:06
 * Generated by rocket framework rocket_generator.py
 * File will not generate while exist
 * Allow editing
****************************************************/


#ifndef ./TEST_COMM_BUSINESSEXCEPTION_H
#define ./TEST_COMM_BUSINESSEXCEPTION_H 




#include <string>
#include <memory>
#include <sstream>
#include <rocket/common/exception.h>
#include <rocket/common/log.h>


namespace ./test {

class BusinessException : public rocket::RocketException {
 public:

  BusinessException(int code, const std::string& err_info, const std::string& file_name, int line):
      rocket::RocketException(code, err_info), m_file_name(file_name), m_line(line) {

    APPINFOLOG("[%s:%lld] throw BusinessException[code: %d, error info: %s]", file_name.c_str(), line, code, err_info.c_str());
  }

  ~BusinessException() {
  
  }

  void handle() override {
    
  }

  const char* what() {
    return m_error_info.c_str();
  }

  std::string file_name() {
    return m_file_name;
  }

  int line() {
    return m_line;
  }

 private:

  std::string m_file_name;

  int m_line {0};

};

}

#endif