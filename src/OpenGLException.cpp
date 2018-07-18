#include "OpenGLException.hpp"

OpenGLException::OpenGLException(const std::string a_file_name, const std::string a_function_name, int an_error_line, int an_error_name, const std::string an_error_message)
    : m_file_name (a_file_name), m_function_name (a_function_name), m_error_line (an_error_line), m_error_name (an_error_name), m_error_message (an_error_message)
    {}


const char* OpenGLException::what() const throw() {
    std::stringstream temp_error_line ;
    temp_error_line << m_error_name << std::endl << m_error_message << std::endl << "\t- in File: " << m_file_name << std::endl << "\t- in Function : " << m_function_name << std::endl << "\t- at Line : " << m_error_line << std::endl ;
    std::string strg (temp_error_line.str()) ;
    return strg.c_str() ;
}

OpenGLException::~OpenGLException() throw ()
{

}



