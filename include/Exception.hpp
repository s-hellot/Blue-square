#ifndef __Exception_h
#define __Exception_h

#include <exception>
#include <iostream>
#include <sstream>

class Exception: public std::exception
{
public :
    Exception (const char* a_file_name = "",
               const char* a_function_name = "",
               int an_error_line = 0,
               std::string an_error_message= "") ;
    Exception (const char* a_file_name,
               const char* a_function_name,
               int an_error_line,
               const char* an_error_message) ;
    virtual ~Exception() throw () ;
    virtual const char* what() const throw () ;


protected :
    const char* m_file_name ;
    const char* m_function_name ;
    int m_error_line ;
    std::string m_error_message ;


};



#endif
