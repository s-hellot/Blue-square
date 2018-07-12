#ifndef DEF_OPENGLEXCPETION
#define DEF_OPENGLEXCEPTION

#include <exception>
#include <iostream>
#include <sstream>

class OpenGLException: public std::exception
{
public :
    OpenGLException (const std::string a_file_name = "", const std::string a_function_name = "", int an_error_line = 0, int an_error_name = 0, const std::string an_error_message = "") ;
    virtual ~OpenGLException() throw () ;
    virtual const char* what() const throw () ;


private :
    const std::string m_file_name ;
    const std::string m_function_name ;
    int m_error_line ;
    int m_error_name ;
    const std::string m_error_message ;


};



#endif
