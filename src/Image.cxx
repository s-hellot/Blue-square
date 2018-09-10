#include <sstream>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <limits>
#include <typeinfo>

/*#include <jerror.h>
#include <jpeglib.h>*/

#ifdef USE_GUI
#include <GLFW/glfw3.h>
#endif

#if defined(USE_OPENMP) && defined(NDEBUG)
#include <omp.h>
#endif


#ifndef __Image_h
#include "Image.h"
#endif

/*
template<> void Image<unsigned char, 1>::loadFromJPEGFile(const char* aFileName)
{
    destroy();

    // Allocate and initialize a JPEG decompression object
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    // Specify the source of the compressed data (eg, a file)
    FILE* p_input_file(fopen(aFileName, "rb"));
    if (!p_input_file)
    {
        std::string error("Can't open \"");
        error += aFileName;
        error += "\"";

        throw  error;
    }
    jpeg_stdio_src(&cinfo, p_input_file);

    // Call jpeg_read_header() to obtain image info
    jpeg_read_header(&cinfo, TRUE);

    // Set parameters for decompression
    // Nothing to be done if the defaults are appropriate

    // Start decompression
    jpeg_start_decompress(&cinfo);

    // Save the size of the image
    unsigned int width(cinfo.output_width);
    unsigned int height(cinfo.output_height);

    // Decompress data
    JSAMPROW row_pointer[1];        // pointer to a single row
    int row_stride;                 // physical row width in buffer

    if (cinfo.out_color_space == JCS_RGB)
    {
        row_stride = width * 3;   // JSAMPLEs per row in image_buffer
    }
    else if (cinfo.out_color_space == JCS_GRAYSCALE)
    {
        row_stride = width;   // JSAMPLEs per row in image_buffer
    }
    // Unknown colour space
    else
    {
        throw "Unknown colour space";
    }


    if (cinfo.out_color_space == JCS_RGB)
    {
        // Finish decompression
        //jpeg_finish_decompress(&cinfo);

        // Release the JPEG decompression object
        jpeg_destroy_decompress(&cinfo);

        *this = Image<unsigned char, 3>(aFileName).RGB2luminance();
    }
    else
    {
        *this = Image<unsigned char, 1>(width, height);

        while (cinfo.output_scanline < cinfo.output_height)
        {
            row_pointer[0] = & m_p_image[cinfo.output_scanline * row_stride];
            jpeg_read_scanlines(&cinfo, row_pointer, 1);
        }

        // Finish decompression
        jpeg_finish_decompress(&cinfo);

        // Release the JPEG decompression object
        jpeg_destroy_decompress(&cinfo);

        // The statistics is not up-to-date
        m_is_stats_up_to_date = false;
    }
}


template<> void Image<unsigned char, 3>::loadFromJPEGFile(const char* aFileName)
{
    destroy();

    // Allocate and initialize a JPEG decompression object
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    // Specify the source of the compressed data (eg, a file)
    FILE* p_input_file(fopen(aFileName, "rb"));
    if (!p_input_file)
    {
        std::string error("Can't open \"");
        error += aFileName;
        error += "\"";

        throw  error;
    }
    jpeg_stdio_src(&cinfo, p_input_file);

    // Call jpeg_read_header() to obtain image info
    jpeg_read_header(&cinfo, TRUE);

    // Set parameters for decompression
    // Nothing to be done if the defaults are appropriate

    // Start decompression
    jpeg_start_decompress(&cinfo);

    // Save the size of the image
    unsigned int width(cinfo.output_width);
    unsigned int height(cinfo.output_height);

    // Decompress data
    JSAMPROW row_pointer[1];        // pointer to a single row
    int row_stride;                 // physical row width in buffer

    if (cinfo.out_color_space == JCS_RGB)
    {
        row_stride = width * 3;   // JSAMPLEs per row in image_buffer
    }
    else if (cinfo.out_color_space == JCS_GRAYSCALE)
    {
        row_stride = width;   // JSAMPLEs per row in image_buffer
    }
    // Unknown colour space
    else
    {
        throw "Unknown colour space";
    }

    if (cinfo.out_color_space == JCS_GRAYSCALE)
    {
        // Finish decompression
        //jpeg_finish_decompress(&cinfo);

        // Release the JPEG decompression object
        jpeg_destroy_decompress(&cinfo);

        *this = Image<unsigned char, 1>(aFileName).luminance2RGB();
    }
    else
    {
        *this = Image<unsigned char, 3>(width, height);

        while (cinfo.output_scanline < cinfo.output_height)
        {
            row_pointer[0] = & m_p_image[cinfo.output_scanline * row_stride];
            jpeg_read_scanlines(&cinfo, row_pointer, 1);
        }

        // Finish decompression
        jpeg_finish_decompress(&cinfo);

        // Release the JPEG decompression object
        jpeg_destroy_decompress(&cinfo);

        // The statistics is not up-to-date
        m_is_stats_up_to_date = false;
    }
}*/


template<> void Image<unsigned char, 1>::updateOpenGLTexture(unsigned int& aTextureID) const
{
#ifdef USE_GUI
    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_LUMINANCE, m_width, m_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, &m_p_image[0]);
#endif
}


template<> void Image<unsigned char, 3>::updateOpenGLTexture(unsigned int& aTextureID) const
{
#ifdef USE_GUI
    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, &m_p_image[0]);
#endif
}


template<> void Image<unsigned char, 4>::updateOpenGLTexture(unsigned int& aTextureID) const
{

#ifdef USE_GUI
    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &m_p_image[0]);
#endif
}


template<> void Image<float, 1>::updateOpenGLTexture(unsigned int& aTextureID) const
{
#ifdef USE_GUI
    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_LUMINANCE, m_width, m_height, 0, GL_LUMINANCE, GL_FLOAT, &m_p_image[0]);
#endif
}


template<> void Image<float, 3>::updateOpenGLTexture(unsigned int& aTextureID) const
{
#ifdef USE_GUI
    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, m_width, m_height, 0, GL_RGB, GL_FLOAT, &m_p_image[0]);
#endif
}


template<> void Image<float, 4>::updateOpenGLTexture(unsigned int& aTextureID) const
{
#ifdef USE_GUI
    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_FLOAT, &m_p_image[0]);
#endif
}
