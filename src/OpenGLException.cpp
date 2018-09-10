#include "OpenGLException.hpp"

OpenGLException::OpenGLException(const char* a_file_name,
                                 const char* a_function_name,
                                 int an_error_line,
                                 int an_error_name)
{
    m_error_name = an_error_name ;
    std::string an_error_message ;
    switch (m_error_name) {
    case GL_INVALID_ENUM :
            an_error_message = "An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag." ;
            break ;
    case GL_INVALID_VALUE :
            an_error_message = "A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag. " ;
            break ;
    case GL_INVALID_OPERATION :
            an_error_message = "The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag. " ;
            break ;

    case GL_OUT_OF_MEMORY :
            an_error_message = "There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded. " ;
            break ;
    default :
            an_error_message = "Erreur OpenGL inconnue" ;
            break ;
    }
    m_file_name = a_file_name ;
    m_function_name = a_function_name ;
    m_error_line = an_error_line ;
    m_error_message = an_error_message ;
    //Exception (a_file_name, a_function_name, an_error_line, an_error_message) ;
}


const char* OpenGLException::what() const throw() {
    std::ostringstream temp_error_line ;
    temp_error_line << m_error_name << std::endl << Exception::what() << std::endl ;
    std::string strg (temp_error_line.str()) ;
    return strg.c_str() ;
}

OpenGLException::~OpenGLException() throw ()
{

}



