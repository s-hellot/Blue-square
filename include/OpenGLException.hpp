#ifndef DEF_OPENGLEXCPETION
#define DEF_OPENGLEXCEPTION

#include <Exception.hpp>
#include <GL/gl.h>
#include <iostream>
#include <sstream>

class OpenGLException: public Exception
{
public :
    OpenGLException (const char* a_file_name = "",
                     const char* a_function_name = "",
                     int an_error_line = 0,
                     int an_error_code = -1) ;
    virtual ~OpenGLException() throw () ;
    virtual const char* what() const throw () ;


private :
    int m_error_name ;


};



#endif
