/**
********************************************************************************
*
*   @file       Image.cpp
*
*   @brief      Class to handle a greyscale image.
*
*   @version    1.0
*
*   @todo       Write here anything you did not implement.
*
*   @date       11/11/2016
*
*   @author     Franck Vidal
*
*
********************************************************************************
*/


//******************************************************************************
//  Define
//******************************************************************************
//#define LINE_SIZE 4096


//******************************************************************************
//  Include
//******************************************************************************
#include <cstring>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <limits>
#include <typeinfo>
#include <iterator>
#include <cmath>
#include <numeric>
/*#include <jerror.h>
#include <jpeglib.h>*/

#ifdef USE_GUI
#include <GLFW/glfw3.h>
#endif

#if defined(USE_OPENMP) && defined(NDEBUG)
#include <omp.h>
#endif

/*#include <sstream> // Header file for stringstream
#include <fstream> // Header file for filestream
#include <iomanip>
#include <algorithm> // Header file for min/max/fill
#include <cmath> // Header file for abs
#include <numeric> // Header file for accumulate
#include <limits> // Numeric limits per types
#include <vector>
#include <iostream>
#include <typeinfo>

// C++ 11
#if __cplusplus > 199711L
#include <utility>
#endif

#ifdef HAS_ITK
#include <itkImage.h>
#include <itkImageSeriesReader.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkNumericSeriesFileNames.h>
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImportImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkAntiAliasBinaryImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include <itkBinaryMorphologicalOpeningImageFilter.h>
#include <itkBinaryMorphologicalClosingImageFilter.h>
#include <itkRecursiveGaussianImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkIdentityTransform.h>
#include <itkBSplineInterpolateImageFunction.h>
#include "itkMesh.h"
//#include "itkBinaryThresholdImageFilter.h"
#include "itkBinaryMask3DMeshSource.h"
//#include "itkMeshFileWriter.h"
#endif

#ifdef HAS_GDCM
#include <gdcmGlobal.h>

#include "gdcmImageReader.h"
#include "gdcmSequenceOfFragments.h"
#include "gdcmSystem.h"
#include "gdcmImageWriter.h"
#include "gdcmPixmapWriter.h"
#include <gdcmRescaler.h>

#include <gdcmImage.h>
#include <gdcmBitmap.h>
#include <gdcmImageWriter.h>
#include <gdcmPixelFormat.h>
#include <gdcmPhotometricInterpretation.h>
#include <gdcmImageChangeTransferSyntax.h>
#endif

#ifdef HAS_OPENMP
#include <omp.h>
#endif

#ifdef HAS_TIFF
#include <tiff.h>
#include <tiffio.h>
#endif

#ifndef __ConstantValues_h
#include "gVirtualXRay/ConstantValues.h"
#endif

#ifndef __Utilities_h
#include "gVirtualXRay/Utilities.h"
#endif


#ifndef __OutOfBoundsException_h
#include "gVirtualXRay/OutOfBoundsException.h"
#endif

#ifndef __OutOfMemoryException_h
#include "gVirtualXRay/OutOfMemoryException.h"
#endif

#ifndef __FileDoesNotExistException_h
#include "FileDoesNotExistException.h"
#endif

#ifndef __PolygonMesh_h
#include "gVirtualXRay/PolygonMesh.h"
#endif
*/

#ifndef __Exception_h
#include "Exception.hpp"
#endif


//******************************************************************************
//  namespace
//******************************************************************************


//-------------------------------------
template<typename T, unsigned int L> Image<T, L>::Image():
//-------------------------------------
        m_width(0),
        m_height(0),
        m_number_of_slices(0),
	    m_voxel_width(1.0),
	    m_voxel_height(1.0),
	    m_voxel_depth(1.0),
        m_is_stats_up_to_date(true)
//-------------------------------------
{
    for (int i(0); i < L; ++i)
    {
        m_min_value[i] = 0;
        m_max_value[i] = 0;
        m_average[i] = 0;
        m_variance[i] = 0;
    }
}


//----------------------------------------------
template<typename T, unsigned int L> Image<T, L>::Image(const Image<T, L>& anImage):
//----------------------------------------------
        m_width(anImage.m_width),
        m_height(anImage.m_height),
        m_number_of_slices(anImage.m_number_of_slices),
	    m_voxel_width(anImage.m_voxel_width),
	    m_voxel_height(anImage.m_voxel_height),
	    m_voxel_depth(anImage.m_voxel_depth),
        m_p_image(anImage.m_p_image),
        m_is_stats_up_to_date(anImage.m_is_stats_up_to_date)
//----------------------------------------------
{
    // Out of memeory
    if (m_width && m_height && m_number_of_slices && L && m_p_image.size() != getNumberOfVoxels() * L)
    {
        throw "OutOfMemoryException(__FILE__, __FUNCTION__, __LINE__)";
    }

    for (int i(0); i < L; ++i)
    {
        m_min_value[i] = anImage.m_min_value[i];
        m_max_value[i] = anImage.m_max_value[i];
        m_average[i]   = anImage.m_average[i];
        m_variance[i]  = anImage.m_variance[i];
    }
}


//----------------------------------------------------------
template<typename T, unsigned int L> Image<T, L>::Image(const char* aFileName):
//----------------------------------------------------------
        m_width(0),
        m_height(0),
        m_number_of_slices(0),
	    m_voxel_width(1.0),
	    m_voxel_height(1.0),
	    m_voxel_depth(1.0),
        m_is_stats_up_to_date(false)
//----------------------------------------------------------
{
    loadFromFile(aFileName);
}


//-----------------------------------------------------------------
template<typename T, unsigned int L> Image<T, L>::Image(const std::string& aFileName):
//-----------------------------------------------------------------
        m_width(0),
        m_height(0),
        m_number_of_slices(0),
	    m_voxel_width(1.0),
	    m_voxel_height(1.0),
	    m_voxel_depth(1.0),
        m_is_stats_up_to_date(false)
//-----------------------------------------------------------------
{
    loadFromFile(aFileName);
}


//----------------------------------------------
template<typename T,  unsigned int L> Image<T,L>::Image(unsigned int aWidth,
                                     unsigned int aHeight,
                                     unsigned int aNumberOfSlices,
                                     T aDefaultValue):
//----------------------------------------------
        m_width(aWidth),
        m_height(aHeight),
        m_number_of_slices(aNumberOfSlices),
	    m_voxel_width(1.0),
	    m_voxel_height(1.0),
	    m_voxel_depth(1.0),
        m_p_image(std::vector<T>(aWidth * aHeight * aNumberOfSlices * L, aDefaultValue))
//----------------------------------------------
{
    // Out of memeory
    if (m_width && m_height && m_number_of_slices && L && m_p_image.size() != getNumberOfVoxels() * L)
    {
        throw "OutOfMemoryException(__FILE__, __FUNCTION__, __LINE__)";
    }
}


template<typename T, unsigned int L> Image<T, L>::~Image()
{
    destroy();
}


template<typename T, unsigned int L> Image<T, L>& Image<T, L>::operator=(const Image<T, L>& anImage)
{
    destroy();

    m_width = anImage.m_width;
    m_height = anImage.m_height;
    m_number_of_slices = anImage.m_number_of_slices;
    m_voxel_width = anImage.m_voxel_width;
    m_voxel_height = anImage.m_voxel_height;
    m_voxel_depth = anImage.m_voxel_depth;

    m_p_image = anImage.m_p_image;
    m_is_stats_up_to_date = anImage.m_is_stats_up_to_date;

    // Out of memory
    if (m_width && m_height && m_number_of_slices && !m_p_image.size())
    {
        throw "OutOfMemoryException(__FILE__, __FUNCTION__, __LINE__";
    }

    for (int i(0); i < L; ++i)
    {
        m_min_value[i] = anImage.m_min_value[i];
        m_max_value[i] = anImage.m_max_value[i];
        m_average[i]   = anImage.m_average[i];
        m_variance[i]  = anImage.m_variance[i];
    }

    return (*this);
}


template<typename T, unsigned int L> void Image<T, L>::clear()
{
    destroy();
}


template<typename T, unsigned int L> void Image<T, L>::loadFromFile(const char* aFileName)
{
    // Empty the image
    clear();

    // The file name correspons to a JPEG file
    if (getFileType(aFileName) == std::string("JPEG"))
        {
        // Load the JPEG file
        loadFromJPEGFile(aFileName);
        }
    // The file name correspons to a RAW file
    else if (getFileType(aFileName) == std::string("RAW"))
        {
        // Cannot read binary raw file without knowing the image size
        // in number of pixels
        // Throw an error
        std::string error_message =
                "Cannot load raw file without knowing its size (";
        error_message += aFileName;
        error_message += ")";
        throw Exception(__FILE__, __FUNCTION__, __LINE__, error_message);
        }
    // The file name correspons to an ASCII/TEXT file
    else if (getFileType(aFileName) == std::string("ASCII"))
        {
        // Load the text file
        loadFromASCIIFile(aFileName);
        }
    // The file name correspons to a MHD file
    else if (getFileType(aFileName) == std::string("MHD"))
        {
        // Load the text file
        loadFromMHDFile(aFileName);
        }
    // The file name correspons to a MHA file
    else if (getFileType(aFileName) == std::string("MHA"))
        {
        // Load the text file
        loadFromMHAFile(aFileName);
        }
    // Unsupportd file type
    else
        {
        // Throw an error
        std::string error_message = "Cannot load file of unknown type (";
        error_message += aFileName;
        error_message += ")";
        throw Exception(__FILE__, __FUNCTION__, __LINE__, error_message);
        }
}


template<typename T, unsigned int L> void Image<T, L>::loadFromFile(const std::string& aFileName)
{
    loadFromFile(aFileName.data());
}




template<typename T, unsigned int L> T& Image<T, L>::getPixel(unsigned int i, unsigned int j, unsigned int k, unsigned int l)
{
    return (this->operator()(i, j, k, l));
}

template<typename T, unsigned int L> const T& Image<T, L>::getPixel(unsigned int i, unsigned int j, unsigned int k, unsigned int l) const
{
    return (this->operator()(i, j, k, l));
}


//------------------------------------------------------
template<typename T, unsigned int L> T& Image<T, L>::operator()(unsigned int i, unsigned int j, unsigned int k, unsigned int l)
//------------------------------------------------------
{
    // The indices are invalid, throw an error
    if (i >= m_width ||
            j >= m_height ||
            k >= m_number_of_slices ||
            l >= L ||
            k * m_height * m_width * L + j * m_width * L + i * L  >= m_p_image.size())
        {
        throw Exception(__FILE__, __FUNCTION__, __LINE__, "Out of bound access");
        }

    // Cannot guaranty that the stats are still up-to-date
    m_is_stats_up_to_date = false;

    // Return the element
    return (m_p_image[k * m_height * m_width * L + j * m_width * L + i * L + l]);
}


//------------------------------------------------------------------
template<typename T, unsigned int L> const T& Image<T, L>::operator()(unsigned int i, unsigned int j, unsigned int k, unsigned int l) const
//------------------------------------------------------------------
{
    // The indices are invalid, throw an error
    if (i >= m_width ||
            j >= m_height ||
            k >= m_number_of_slices ||
            l >= L ||
            k * m_height * m_width * L + j * m_width * L + i * L  >= m_p_image.size())
        {
        throw Exception(__FILE__, __FUNCTION__, __LINE__, "Out of bound access");
        }

    // Return the element
    return (m_p_image[k * m_height * m_width * L + j * m_width * L + i * L + l]);
}

//------------------------------------------------------
template<typename T, unsigned int L> T& Image<T, L>::operator[](unsigned int anElementIndex)
//------------------------------------------------------
{
    // The index is invalid, throw an error
    if (anElementIndex >= getNumberOfVoxels() * L ||
    anElementIndex  >= m_p_image.size())
    {
        throw Exception(__FILE__, __FUNCTION__, __LINE__, "Out of bound access");
    }

    // Cannot guaranty that the stats are still up-to-date
    m_is_stats_up_to_date = false;

    // Return the element
    return (m_p_image[anElementIndex]);
}


//------------------------------------------------------------------
template<typename T, unsigned int L> const T& Image<T, L>::operator[](unsigned int anElementIndex) const
//------------------------------------------------------------------
{
    // The index is invalid, throw an error
    if (anElementIndex >= getNumberOfVoxels() * L ||
             anElementIndex  >= m_p_image.size())
        {
        throw Exception(__FILE__, __FUNCTION__, __LINE__, "Out of bound access");
        }

    // Return the element
    return (m_p_image[anElementIndex]);
}


template<typename T, unsigned int L> void Image<T, L>::saveFile(const char* aFileName) const
{
    if (getFileType(aFileName) == std::string("PGM"))
        {
        savePGMFile(aFileName);
        }
    else if (getFileType(aFileName) == std::string("JPEG"))
        {
        saveJPEGFile(aFileName);
        }
    else if (getFileType(aFileName) == std::string("RAW"))
        {
        saveRawFile(aFileName);
        }
    else if (getFileType(aFileName) == std::string("ASCII"))
        {
        saveASCIIFile(aFileName);
        }
    else if (getFileType(aFileName) == std::string("MHD"))
        {
        saveMHDFile(aFileName);
        }
    else
        {
        std::string error_message = "Cannot save file of unknown type (";
        error_message += aFileName;
        error_message += ")";
        throw Exception(__FILE__, __FUNCTION__, __LINE__, error_message);
        }
}


template<typename T, unsigned int L> void Image<T, L>::saveFile(const std::string& aFileName) const
{
    saveFile(aFileName.data());
}

template<typename T, unsigned int L> void Image<T, L>::saveASCIIFile(const char* aFileName) const
{
    // Open the file
    std::ofstream output_file(aFileName);

    // The file cannot be created
    // Throw an error
    if (!output_file.is_open())
        {
        std::string error_message = "Cannot save ASCII file (";
        error_message += aFileName;
        error_message += ")";
        throw Exception(__FILE__, __FUNCTION__, __LINE__, error_message);
        }

    // Print the image into the stream
    output_file << *this;
}


template<typename T, unsigned int L> void Image<T, L>::saveASCIIFile(const std::string& aFileName) const
{
    saveASCIIFile(aFileName.data());
}


template<typename T, unsigned int L> void Image<T, L>::saveMHDFile(const std::string& aFileName) const
{
    saveMHDFile(aFileName.data());
}


template<typename T, unsigned int L> void Image<T, L>::saveMHDFile(const char* aFileName) const
{
    if (L != 1)
    {
        throw "Invalid pixel type";
    }

	// Open header file
    std::ofstream header_file(aFileName);

    if (!header_file.is_open())
        {
        std::string error_message = "Cannot save MHD file (";
        error_message += aFileName;
        error_message += ")";
        throw Exception(__FILE__, __FUNCTION__, __LINE__, error_message);
        }
    // The file is open
    else
        {
        header_file << "ObjectType = Image" << std::endl;
		header_file << "NDims = " << (m_number_of_slices <= 1 ? 2 : 3) << std::endl;
		header_file << "BinaryData = True" << std::endl;
		header_file << "BinaryDataByteOrderMSB = False" << std::endl;
		header_file << "CompressedData = False" << std::endl;

		// 2D
		if (m_number_of_slices <= 1)
			{
			header_file << "ElementSpacing = " << m_voxel_width << " " << m_voxel_height << std::endl;
			header_file << "DimSize = " << m_width << " " << m_height << std::endl;
			}
		// 3D
		else
			{
			header_file << "TransformMatrix = 1 0 0 0 1 0 0 0 1" << std::endl;
			header_file << "Offset = 0 0 0" << std::endl;
			header_file << "CenterOfRotation = 0 0 0" << std::endl;
			header_file << "ElementSpacing = " << m_voxel_width << " " << m_voxel_height << " " << m_voxel_depth << std::endl;
			header_file << "DimSize = " << m_width << " " << m_height << " " << m_number_of_slices << std::endl;
			header_file << "AnatomicalOrientation = ???" << std::endl;
			}

		if (typeid(T) == typeid(unsigned char))
			{
			header_file << "ElementType = MET_UCHAR" << std::endl;
			}
		else if (typeid(T) == typeid(char))
			{
				header_file << "ElementType = MET_CHAR" << std::endl;
			}
		else if (typeid(T) == typeid(unsigned short))
			{
				header_file << "ElementType = MET_USHORT" << std::endl;
			}
		else if (typeid(T) == typeid(short))
			{
				header_file << "ElementType = MET_SHORT" << std::endl;
			}
		else if (typeid(T) == typeid(unsigned int))
			{
				header_file << "ElementType = MET_UINT" << std::endl;
			}
		else if (typeid(T) == typeid(int))
			{
				header_file << "ElementType = MET_INT" << std::endl;
			}
		else if (typeid(T) == typeid(float))
			{
				header_file << "ElementType = MET_FLOAT" << std::endl;
			}
		else if (typeid(T) == typeid(double))
			{
				header_file << "ElementType = MET_DOUBLE" << std::endl;
			}
		else
			{
	        throw "Invalid pixel type";
			}

		header_file << "ElementDataFile = " << getFileNameWithoutPath(getFileNameWithoutExtension(aFileName)) << ".raw" << std::endl;
        }

	// Save binary file
	saveRawFile(getFileNameWithoutExtension(aFileName) + ".raw");
}


template<typename T, unsigned int L> unsigned int Image<T, L>::getWidth() const
{
    return (m_width);
}


template<typename T, unsigned int L> unsigned int Image<T, L>::getHeight() const
{
    return (m_height);
}


template<typename T, unsigned int L> unsigned int Image<T, L>::getNumberOfSlices() const
{
    return (m_number_of_slices);
}


template<typename T, unsigned int L> unsigned int Image<T, L>::getNumberOfVoxels() const
{
    return (m_width * m_height * m_number_of_slices);
}


template<typename T, unsigned int L> unsigned int Image<T, L>::getNumberOfColourChannels() const
{
    return (L);
}


template<typename T, unsigned int L> unsigned int Image<T, L>::getNumberOfColorChannels() const
{
    return (L);
}


template<typename T, unsigned int L> void Image<T, L>::setVoxelWidth(const double& aSize)
{
	m_voxel_width = aSize;
}


template<typename T, unsigned int L> void Image<T, L>::setVoxelHeight(const double& aSize)
{
	m_voxel_height = aSize;
}


template<typename T, unsigned int L> void Image<T, L>::setVoxelDepth(const double& aSize)
{
	m_voxel_depth = aSize;
}


template<typename T, unsigned int L> const double& Image<T, L>::getVoxelWidth() const
{
	return (m_voxel_width);
}


template<typename T, unsigned int L> const double& Image<T, L>::getVoxelHeight() const
{
	return (m_voxel_height);
}


template<typename T, unsigned int L> const double& Image<T, L>::getVoxelDepth() const
{
	return (m_voxel_depth);
}


template<typename T, unsigned int L> T* Image<T, L>::getRawDataPointer()
{
    return (&m_p_image[0]);
}


template<typename T, unsigned int L> const T* Image<T, L>::getRawDataPointer() const
{
    return (&m_p_image[0]);
}


template<typename T, unsigned int L> Image<T, L> Image<T, L>::flipVertically() const
{
    Image<T, L> temp(*this);

#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel for schedule(dynamic,1) collapse(3)
#endif
    for (int k = 0; k < m_number_of_slices; ++k)
    {
        for (int j = 0; j < m_height; ++j)
        {
            for (int i = 0; i < m_width; ++i)
            {
                for (int l = 0; l < L; ++l)
                {
                    temp[k * m_height * m_width * L +
                                   (m_height - j - 1) * m_width * L +
                                   i * L +
                                   l] = m_p_image[k * m_height * m_width * L +
                                                  j * m_width * L +
                                                  i * L +
                                                  l];
                }
            }
        }
    }

    return (temp);
}


template<typename T, unsigned int L> Image<T, L> Image<T, L>::flipHorizontally() const
{
    Image<T, L> temp(*this);

#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel for schedule(dynamic,1) collapse(3)
#endif
    for (int k = 0; k < m_number_of_slices; ++k)
    {
        for (int j = 0; j < m_height; ++j)
        {
            for (int i = 0; i < m_width; ++i)
            {
                for (int l = 0; l < L; ++l)
                {
                    temp[k * m_height * m_width * L +
                                   j * m_width * L +
                                   (m_width - i - 1) * L +
                                   l] = m_p_image[k * m_height * m_width * L +
                                                  j * m_width * L +
                                                  i * L +
                                                  l];
                }
            }
        }
    }

    return (temp);
}


template<typename T, unsigned int L> Image<T, L> Image<T, L>::operator!() const
{
    Image<T, L> temp = *this;

    T min_value = getMin();
    T max_value =  getMax();

    return (min_value + (max_value - min_value) - (*this - min_value));
}


template<typename T, unsigned int L> Image<T, L> Image<T, L>::normaliseMeanStdDev() const
{
    return (((*this) - getMean()) / getStandardDeviation());
}


template<typename T, unsigned int L> Image<T, L> Image<T, L>::normaliseZeroToOne() const
{
    return (contrastStretching(0.0, 1.0));
}


template<typename T, unsigned int L> Image<T, L> Image<T, L>::log() const
{
	Image<T, L> image(*this);
	image.m_voxel_width  = m_voxel_width;
	image.m_voxel_height = m_voxel_height;
	image.m_voxel_depth  = m_voxel_depth;

#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel for schedule(dynamic,1) collapse(3)
#endif
    for (int k = 0; k < m_number_of_slices; ++k)

    {
        for (int j = 0; j < m_height; ++j)
        {
            for (int i = 0; i < m_width; ++i)
            {
                for (int l = 0; l < L; ++l)
			    {
				    image(i, j, k, l) = std::log(image(i, j, k, l));
			    }
		    }
		}
	}


	return (image);
}


template<typename T, unsigned int L> Image<T, L> Image<T, L>::abs() const
{
	Image<T, L> image(*this);

#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel for schedule(dynamic,1) collapse(3)
#endif
    for (int k = 0; k < m_number_of_slices; ++k)

    {
        for (int j = 0; j < m_height; ++j)
        {
            for (int i = 0; i < m_width; ++i)
            {
                for (int l = 0; l < L; ++l)
			    {
				    image(i, j, k, l) = std::abs(image(i, j, k, l));
			    }
		    }
		}
	}


	return (image);
}



template<typename T, unsigned int L> double Image<T, L>::getSAE(const Image<T, L>& anImage) const
{
    if (m_width != anImage.m_width || m_height != anImage.m_height || m_number_of_slices != anImage.m_number_of_slices)
    {
        throw Exception(__FILE__, __FUNCTION__, __LINE__, "Images of different sizes");
    }

    double sum(0.0);

#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel reduction(+: sum)
#endif
    for (int i = 0; i < getNumberOfVoxels() * L; ++i)
    {
        sum += std::abs(m_p_image[i] - anImage[i]);
    }
    return (sum);
}


template<typename T, unsigned int L> double Image<T, L>::getEuclideanDistance(const Image<T, L>& anImage) const
{
    return (std::sqrt(getSSE(anImage)));
}


template<typename T, unsigned int L> double Image<T, L>::getMAE(const Image<T, L>& anImage) const
{
    return (getSAE(anImage) / double(getNumberOfVoxels() * L));
}


template<typename T, unsigned int L> double Image<T, L>::getSSE(const Image<T, L>& anImage) const
{
    if (m_width != anImage.m_width || m_height != anImage.m_height || m_number_of_slices != anImage.m_number_of_slices)
    {
        throw Exception(__FILE__, __FUNCTION__, __LINE__, "Images of different sizes");
    }

    double sum(0.0);
    double a;
    double b;

#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel reduction(+: sum) private(a, b)
#endif
    for (int i = 0; i < getNumberOfVoxels() * L; ++i)
    {
        a = m_p_image[i];
        b = anImage[i];
        sum += (a - b) * (a - b);
    }
    return (sum);
}


template<typename T, unsigned int L> double Image<T, L>::getMSE(const Image<T, L>& anImage) const
{
    return (getSSE(anImage) / double(getNumberOfVoxels() * L));
}


template<typename T, unsigned int L> double Image<T, L>::getRMSE(const Image<T, L>& anImage) const
{
    return (std::sqrt(getMSE(anImage)));
}


template<typename T, unsigned int L> double Image<T, L>::getSAD(const Image<T, L>& anImage) const
{
    return (getSAE(anImage));
}


template<typename T, unsigned int L> double Image<T, L>::getMAD(const Image<T, L>& anImage) const
{
    return (getMAE(anImage));
}


template<typename T, unsigned int L> double Image<T, L>::getSSD(const Image<T, L>& anImage) const
{
    return (getSSE(anImage));
}


template<typename T, unsigned int L> double Image<T, L>::getMSD(const Image<T, L>& anImage) const
{
    return (getMSE(anImage));
}


template<typename T, unsigned int L> double Image<T, L>::getRMSD(const Image<T, L>& anImage) const
{
    return (getRMSE(anImage));
}


template<typename T, unsigned int L> double Image<T, L>::getL1Norm(const Image<T, L>& anImage) const
{
    return (getSAE(anImage));
}

template<typename T, unsigned int L> double Image<T, L>::getL2Norm(const Image<T, L>& anImage) const
{
    return (getSSE(anImage));
}


template<typename T, unsigned int L> double Image<T, L>::getSNR(const Image<T, L>& aReferenceImage) const
{
    if (m_width != aReferenceImage.m_width || m_height != aReferenceImage.m_height || m_number_of_slices != aReferenceImage.m_number_of_slices)
    {
        throw Exception(__FILE__, __FUNCTION__, __LINE__, "Images of different sizes");
    }

    // Signal-to-noise ratio expressed in dB

    // See http://bigwww.epfl.ch/sage/soft/snr/
    return (10.0 * std::log10((aReferenceImage * aReferenceImage).getSum() / getSSE(aReferenceImage)));
}


template<typename T, unsigned int L> double Image<T, L>::getPSNR(const Image<T, L>& aReferenceImage) const
{
    if (m_width != aReferenceImage.m_width || m_height != aReferenceImage.m_height || m_number_of_slices != aReferenceImage.m_number_of_slices)
    {
        throw Exception(__FILE__, __FUNCTION__, __LINE__, "Images of different sizes");
    }

    // Peak signal-to-noise ratio expressed in dB

    // See http://bigwww.epfl.ch/sage/soft/snr/
    // https://en.wikipedia.org/wiki/Peak_signal-to-noise_ratio

    return (20.0 * std::log10(aReferenceImage.getMax()) - 10.0 * std::log10(getMSE(aReferenceImage)));
}


template<typename T, unsigned int L> double Image<T, L>::getSSIM(const Image<T, L>& anImage) const
{
    if (m_width != anImage.m_width || m_height != anImage.m_height || m_number_of_slices != anImage.m_number_of_slices)
    {
        throw Exception(__FILE__, __FUNCTION__, __LINE__, "Images of different sizes");
    }

    // structural similarity (SSIM) index

    // https://en.wikipedia.org/wiki/Structural_similarity

    double mean_x = getMean();
    double mean_y = anImage.getMean();

    double variance_x = getVariance();
    double variance_y = anImage.getVariance();

    double covariance = getCovariance(anImage);

    double k1 = 0.01;
    double k2 = 0.03;

    double l = std::max(getMax() - getMin(), anImage.getMax() - anImage.getMin());

    double c1 = k1 * l;
    double c2 = k2 * l;

    c1 *= c1;
    c2 *= c2;

    return (((2.0 * mean_x * mean_y + c1) * (2.0 * covariance + c2)) / ((mean_x * mean_x + mean_y * mean_y + c1) * (variance_x + variance_y + c2)));
}


template<typename T, unsigned int L> double Image<T, L>::getDSSIM(const Image<T, L>& anImage) const
{
    return ((1.0 - getSSIM(anImage)) / 2.0);
}


template<typename T, unsigned int L> double Image<T, L>::getIsotropicTotalVariation(unsigned int aFlag, const double& aSigmaValue) const
{
    double tv = 0.0;

    if (m_number_of_slices == 1)
    {
#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel for collapse(3) reduction(+: tv)
#endif
        for (int k = 0; k < m_number_of_slices; ++k)
        {
            for (int j = 0; j < m_height - 1; ++j)
            {
                for (int i = 0; i < m_width - 1; ++i)
                {
                    for (int l = 0; l < L; ++l)
                    {
                        double tv_ijk = std::sqrt(
                                std::pow((*this)(i + 1, j, k, l) - (*this)(i, j, k, l), 2) +
                                std::pow((*this)(i, j + 1, k, l) - (*this)(i, j, k, l), 2)
                        );

                        double weight = 1.0;

                        if (aFlag == 1)
                        {
                            weight = weightTV1(tv_ijk, aSigmaValue);
                        }
                        else if (aFlag == 2)
                        {
                            weight = weightTV2(tv_ijk, aSigmaValue);
                        }

                        tv += weight * tv_ijk;
                    }
                }
            }
        }
    }
    else
    {
#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel for schedule(dynamic,1) collapse(3) reduction(+: tv)
#endif
        for (int k = 0; k < m_number_of_slices - 1; ++k)
        {
            for (int j = 0; j < m_height - 1; ++j)
            {
                for (int i = 0; i < m_width - 1; ++i)
                {
                    for (int l = 0; l < L; ++l)
                    {
                        double tv_ijk = std::sqrt(
                                std::pow((*this)(i + 1, j, k, l) - (*this)(i, j, k, l), 2) +
                                std::pow((*this)(i, j + 1, k, l) - (*this)(i, j, k, l), 2) +
                                std::pow((*this)(i, j, k + 1, l) - (*this)(i, j, k, l), 2)
                        );

                        double weight = 1.0;

                        if (aFlag == 1)
                        {
                            weight = weightTV1(tv_ijk, aSigmaValue);
                        }
                        else if (aFlag == 2)
                        {
                            weight = weightTV2(tv_ijk, aSigmaValue);
                        }

                        tv += weight * tv_ijk;
                    }
                }
            }
        }
    }

    return (tv);
}


template<typename T, unsigned int L> double Image<T, L>::getAnisotropicTotalVariation(unsigned int aFlag, const double& aSigmaValue) const
{
    double tv = 0.0;

    if (m_number_of_slices == 1)
    {
#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel for schedule(dynamic,1) collapse(3) reduction(+: tv)
#endif
        for (int k = 0; k < m_number_of_slices; ++k)
        {
            for (int j = 0; j < m_height - 1; ++j)
            {
                for (int i = 0; i < m_width - 1; ++i)
                {
                    for (int l = 0; l < L; ++l)
                    {
                        double tv_ijk =
                                std::abs((*this)(i + 1, j, k, l) - (*this)(i, j, k, l)) +
                                std::abs((*this)(i, j + 1, k, l) - (*this)(i, j, k, l));

                        double weight = 1.0;

                        if (aFlag == 1)
                        {
                            weight = weightTV1(tv_ijk, aSigmaValue);
                        }
                        else if (aFlag == 2)
                        {
                            weight = weightTV2(tv_ijk, aSigmaValue);
                        }

                        tv += weight * tv_ijk;
                    }
                }
            }
        }
    }
    else
    {
#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel for schedule(dynamic,1) collapse(3) reduction(+: tv)
#endif
        for (int k = 0; k < m_number_of_slices; ++k)
        {
            for (int j = 0; j < m_height - 1; ++j)
            {
                for (int i = 0; i < m_width - 1; ++i)
                {
                    for (int l = 0; l < L; ++l)
                    {
                        double tv_ijk =
                                std::abs((*this)(i + 1, j, k, l) - (*this)(i, j, k, l)) +
                                std::abs((*this)(i, j + 1, k, l) - (*this)(i, j, k, l)) +
                                std::abs((*this)(i, j, k + 1, l) - (*this)(i, j, k, l));

                        double weight = 1.0;

                        if (aFlag == 1)
                        {
                            weight = weightTV1(tv_ijk, aSigmaValue);
                        }
                        else if (aFlag == 2)
                        {
                            weight = weightTV2(tv_ijk, aSigmaValue);
                        }

                        tv += weight * tv_ijk;
                    }
                }
            }
        }
    }

    return (tv);
}


template<typename T, unsigned int L> double Image<T, L>::getDotProduct(const Image<T, L>& anImage) const
{
    if (m_width != anImage.m_width || m_height != anImage.m_height || m_number_of_slices != anImage.m_number_of_slices)
    {
        throw Exception(__FILE__, __FUNCTION__, __LINE__, "Images of different sizes");
    }

    double sum = 0.0;
#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel reduction(+: sum)
#endif
    for (int i = 0; i < getNumberOfVoxels() * L; ++i)
    {
        sum += m_p_image[i] * anImage[i];
    }

    return (sum);
}


template<typename T, unsigned int L> double Image<T, L>::getZNCC(const Image<T, L>& anImage) const
{
    if (m_width != anImage.m_width || m_height != anImage.m_height || m_number_of_slices != anImage.m_number_of_slices)
    {
        throw Exception(__FILE__, __FUNCTION__, __LINE__, "Images of different sizes");
    }

    double mean_test = getMean();
    double mean_ref  = anImage.getMean();

    double sum = 0.0;

#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel reduction(+: sum)
#endif
    for (int i = 0; i < getNumberOfVoxels() * L; ++i)
    {
        sum += (m_p_image[i] - mean_test) * (anImage[i] - mean_ref);
    }

    return (sum / double(getNumberOfVoxels() * L * getStandardDeviation() * anImage.getStandardDeviation()));

//    return (((*this - getMean()) * (anImage - anImage.getMean())).getSum() / double(getNumberOfVoxels() * L * getStandardDeviation() * anImage.getStandardDeviation()));
}


template<typename T, unsigned int L> double Image<T, L>::getRelativeError(const Image<T, L>& aReferenceImage) const
{
    if (m_width != aReferenceImage.m_width || m_height != aReferenceImage.m_height || m_number_of_slices != aReferenceImage.m_number_of_slices)
    {
        throw Exception(__FILE__, __FUNCTION__, __LINE__, "Images of different sizes");
    }

    T min = aReferenceImage.getMin();
    T max = aReferenceImage.getMax();

    T offset = - min + 0.005 * (max - min);

    double sum = 0.0;

#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel reduction(+: sum)
#endif
    for (int i = 0; i < getNumberOfVoxels() * L; ++i)
    {
        sum += std::abs((m_p_image[i] + offset) / (aReferenceImage[i] + offset) - 1.0);
    }

    return (sum / (getNumberOfVoxels() * L));
}


template<typename T, unsigned int L> double Image<T, L>::getCovariance(const Image<T, L>& anImage) const
{
    if (m_width != anImage.m_width || m_height != anImage.m_height || m_number_of_slices != anImage.m_number_of_slices)
    {
        throw Exception(__FILE__, __FUNCTION__, __LINE__, "Images of different sizes");
    }

    return (((*this - getMean()).getDotProduct(anImage - anImage.getMean())) / double(getNumberOfVoxels() * L));
}


template<typename T, unsigned int L> void Image<T, L>::setToConstantValue(const T& aValue)
{
#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel
#endif
    for (int i = 0; i < getNumberOfVoxels() * L; ++i)
    {
        m_p_image[i] = aValue;
    }
}


template<typename T, unsigned int L> void Image<T, L>::destroy()
{
    m_p_image.clear();

    m_width = 0;
    m_height = 0;
    m_number_of_slices = 0;

    m_voxel_width = 1.0;
    m_voxel_height = 1.0;
    m_voxel_depth = 1.0;


    m_is_stats_up_to_date = true;

    for (int i(0); i < L; ++i)
    {
        m_min_value[i] = 0;
        m_max_value[i] = 0;
        m_average[i] = 0;
        m_variance[i] = 0;

    }
}


template<typename T, unsigned int L> std::string Image<T, L>::getFileType(const char* aFileName)
{
    return (getFileType(std::string(aFileName)));
}


//----------------------------------------------------------------
template<typename T, unsigned int L> std::string Image<T, L>::getFileType(const std::string& aFileName)
//----------------------------------------------------------------
{
    // Get the last 4 characters of the file name
    std::string short_extension(aFileName.substr( aFileName.length() - 4 ));

    // Get the last 5 characters of the file name (in case it is ".jpeg")
    std::string long_extension( aFileName.substr( aFileName.length() - 5 ));

    // Use uppercase
    std::transform(
            short_extension.begin(),
            short_extension.end(),
            short_extension.begin(),
            ::toupper);

    std::transform(
            long_extension.begin(),
            long_extension.end(),
            long_extension.begin(),
            ::toupper);

    // It is a JPEG file
    if (short_extension == ".JPG" || long_extension == ".JPEG")
        {
        return (std::string("JPEG"));
        }

    // It is a binary RAW file
    if (short_extension == ".RAW" || short_extension == ".BIN")
        {
        return (std::string("RAW"));
        }

    // It is an ASCII text file
    if (short_extension == ".TXT")
        {
        return (std::string("ASCII"));
        }

    // It is an ASCII text file
    if (short_extension == ".MAT")
        {
        return (std::string("ASCII"));
        }

    // It is a PGM text file
    if (short_extension == ".PGM")
        {
        return (std::string("PGM"));
        }

    // It is a MHD text file
    if (short_extension == ".MHD")
        {
        return (std::string("MHD"));
        }

    // It is a MHA text file
    if (short_extension == ".MHA")
        {
        return (std::string("MHA"));
        }

    // It is not supported
    return (std::string(""));
}


template<typename T, unsigned int L> std::string Image<T, L>::getFileNameWithoutExtension(const char* aFileName)
{
    return (getFileNameWithoutExtension(std::string(aFileName)));
}


template<typename T, unsigned int L> std::string Image<T, L>::getFileNameWithoutExtension(const std::string& aFileName)
{
    size_t last_dot = aFileName.find_last_of(".");
    if (last_dot == std::string::npos) return aFileName;
    return (aFileName.substr(0, last_dot));
}


template<typename T, unsigned int L> std::string Image<T, L>::getFileNameWithoutPath(const char* aFileName)
{
    return (getFileNameWithoutPath(std::string(aFileName)));
}


template<typename T, unsigned int L> std::string Image<T, L>::getFileNameWithoutPath(const std::string& aFileName)
{
    size_t last_slash = aFileName.find_last_of("/");
    size_t last_antislash = aFileName.find_last_of("\\");
    size_t position = std::string::npos;

    if (last_slash != std::string::npos)
    	{
    	position = last_slash;
    	}

    if (last_antislash != std::string::npos)
    	{
    	if (position != std::string::npos)
	    	{
	    	position = std::max(last_slash, last_antislash);
	    	}
	    else
	    	{
	    	position = last_antislash;
	    	}
    	}

    if (position == std::string::npos) return aFileName;
    return (aFileName.substr(position + 1, aFileName.size() - position - 1));
}


template<typename T, unsigned int L> std::string Image<T, L>::getPath(const std::string& aFileName)
{
    size_t last_slash = aFileName.find_last_of("/");
    size_t last_antislash = aFileName.find_last_of("\\");
    size_t position = std::string::npos;

    if (last_slash != std::string::npos)
    	{
    	position = last_slash;
    	}

    if (last_antislash != std::string::npos)
    	{
    	if (position != std::string::npos)
	    	{
	    	position = std::max(last_slash, last_antislash);
	    	}
	    else
	    	{
	    	position = last_antislash;
	    	}
    	}

    if (position == std::string::npos) return ".";
    return (aFileName.substr(0, position));
}


template<typename T, unsigned int L> double Image<T, L>::weightTV1(const double& aTVValue, const double& aSigmaValue) const
{
    // From Eq(4) in An Edge-Preserving Total Variation Denoising Method for DECT Image (2013)
    // by Le Shen and Yuxiang Xing
    return (1.0e-5 / (1.0e-5 + std::pow(aTVValue, aSigmaValue)));
}


template<typename T, unsigned int L> double Image<T, L>::weightTV2(const double& aTVValue, const double& aSigmaValue) const
{
    // From Eq (5) in GPU-based Low Dose CT Reconstruction via Edge-preserving Total Variation Regularization,
    // by Zhen Tian Xun Jia, Kehong Yuan, Tinsu Pan, Steve B. Jiang
    return (std::exp(-std::pow(aTVValue / (aSigmaValue / 25), 2)) / 1000.0);
}


//-----------------------------------------------------------------------------------------------------------------
template<typename T, unsigned int L> void Image<T, L>::loadMHDHeader(std::ifstream& anInputStream,
																	 const char* aMetaHeaderFileName,
																	 std::string& aBinaryFileName,
																	 bool& aSwapByteFlag)
//-----------------------------------------------------------------------------------------------------------------
{
	std::string line;
	aBinaryFileName = "";

	std::getline(anInputStream, line);
	if (line != "ObjectType = Image" && line != "ObjectType = Image\r")
	{
        // Throw an error
        std::string error_message = "Invalid header file (";
        error_message += aMetaHeaderFileName;
        error_message += ")";
        throw Exception(__FILE__, __FUNCTION__, __LINE__, error_message);
	}

	int dimensions = 0;
	aSwapByteFlag = false;

    while (std::getline(anInputStream, line))
	{
        std::istringstream iss(line);

		if (line == "BinaryData = True")
		{
			// Do nothing
		}
		else if (line == "BinaryDataByteOrderMSB = True")
		{
			aSwapByteFlag = true;
		}
		else if (line == "ElementDataFile = LOCAL")
		{
			aBinaryFileName = "LOCAL";
		}
		else if (line == "CompressedData = True")
		{
			throw "CompressedData in MHD files: not supported";
		}
		else
		{
			std::string temp;
			iss >> temp;

			if (temp == "NDims")
			{
				iss >> temp;
				iss >> dimensions;

				if (dimensions != 2 && dimensions != 3)
				{
					// Throw an error
					std::string error_message = "Invalid header file (";
					error_message += aMetaHeaderFileName;
					error_message += ")";
					throw Exception(__FILE__, __FUNCTION__, __LINE__, error_message);
				}
			}
			else if (temp == "DimSize")
			{
				iss >> temp;

				if (dimensions == 2)
				{
					iss >> m_width >> m_height;
					m_number_of_slices = 1;
				}
				else if (dimensions == 3)
				{
					iss >> m_width >> m_height >> m_number_of_slices;
				}
				else
				{
			        // Throw an error
			        std::string error_message = "Invalid header file (";
			        error_message += aMetaHeaderFileName;
			        error_message += ")";
			        throw Exception(__FILE__, __FUNCTION__, __LINE__, error_message);
				}
			}
			else if (temp == "ElementSpacing")
			{
				iss >> temp;

				if (dimensions == 2)
				{
					iss >> m_voxel_width >> m_voxel_height;
					m_voxel_depth = 1;
				}
				else if (dimensions == 3)
				{
					iss >> m_voxel_width >> m_voxel_height >> m_voxel_depth;
				}
				else
				{
			        // Throw an error
			        std::string error_message = "Invalid header file (";
			        error_message += aMetaHeaderFileName;
			        error_message += ")";
			        throw Exception(__FILE__, __FUNCTION__, __LINE__, error_message);
				}
			}
			else if (temp == "ElementType")
			{
				iss >> temp;
				iss >> temp;

            if (temp == "MET_UCHAR" && typeid(T) != typeid(unsigned char))
					{
			        throw "Invalid pixel type";
					}
				else if (temp == "MET_CHAR" && typeid(T) != typeid(char))
					{
			        throw "Invalid pixel type";
					}
				else if (temp == "MET_USHORT" && typeid(T) != typeid(unsigned short))
					{
			        throw "Invalid pixel type";
					}
				else if (temp == "MET_SHORT" && typeid(T) != typeid(short))
					{
			        throw "Invalid pixel type";
					}
				else if (temp == "MET_UINT" && typeid(T) != typeid(unsigned int))
					{
			        throw "Invalid pixel type";
					}
				else if (temp == "MET_INT" && typeid(T) != typeid(int))
					{
			        throw "Invalid pixel type";
					}
				else if ((temp == "MET_FLOAT" || temp == "MET_FLOAT\r") && typeid(T) != typeid(float))
					{
			        throw "Invalid pixel type";
					}
				else if (temp == "MET_DOUBLE" && typeid(T) != typeid(double))
					{
			        throw "Invalid pixel type";
					}
			}
			else if (temp == "ElementDataFile")
			{
				iss >> temp;
				iss >> aBinaryFileName;
			}
			else if (temp == "CompressedData")
			{
				// Do nothing
			}
		}
	}
}


template<typename T, unsigned int L> void Image<T, L>::loadFromRawStream(std::ifstream& anInputStream,
																		 unsigned int aWidth,
																		 unsigned int aHeight,
																		 unsigned int aNumberOfSlices,
																		 bool aSwapByteFlag)
{

    if (L != 1)
    {
        throw "Invalid pixel type";
    }

    // Empty the image
    clear();
    destroy();

    // Memory size
    unsigned int memory_size = 0;
    unsigned int word_size   = 0;

	if (typeid(T) == typeid(unsigned char))
		{
		word_size = sizeof(unsigned char);
		}
	else if (typeid(T) == typeid(char))
		{
		word_size = sizeof(char);
		}
	else if (typeid(T) == typeid(unsigned short))
		{
		word_size = sizeof(unsigned short);
		}
	else if (typeid(T) == typeid(short))
		{
		word_size = sizeof(short);
		}
	else if (typeid(T) == typeid(unsigned int))
		{
		word_size = sizeof(unsigned int);
		}
	else if (typeid(T) == typeid(int))
		{
		word_size = sizeof(int);
		}
	else if (typeid(T) == typeid(float))
		{
		word_size = sizeof(float);
		}
	else if (typeid(T) == typeid(double))
		{
		word_size = sizeof(double);
		}
	else
		{
        throw "Invalid pixel type";
		}

	memory_size = aWidth * aHeight * aNumberOfSlices * word_size;

	// Allocate memory
	float temp_val = 100;
    swapByte(reinterpret_cast<char*>(&temp_val), word_size);

	m_p_image = std::vector<T>(aWidth * aHeight * aNumberOfSlices, temp_val);

    //anInputStream.read(reinterpret_cast<char*>(&m_p_image[0]), memory_size);

	std::vector<char> buffer((
	            std::istreambuf_iterator<char>(anInputStream)),
	            (std::istreambuf_iterator<char>()));

	memcpy(&m_p_image[0], &buffer[0], memory_size);

    if (aSwapByteFlag)
    {
    	for (typename std::vector<T>::iterator ite(m_p_image.begin());
    			ite != m_p_image.end();
    			++ite)
    	{
    	    swapByte(reinterpret_cast<char*>(&(*ite)), word_size);
    	}
    }
    m_width            = aWidth;
    m_height           = aHeight;
    m_number_of_slices = aNumberOfSlices;
    m_is_stats_up_to_date = false;
}


//----------------------------------------------------------
template<typename T, unsigned int L> void Image<T, L>::loadFromASCIIFile(const std::string& aFileName)
//----------------------------------------------------------
{
    loadFromASCIIFile(aFileName.data());
}


//--------------------------------------------------------------------------------------------------
template<typename T, unsigned int L> void Image<T, L>::loadFromMHDFile(const std::string& aFileName)
//--------------------------------------------------------------------------------------------------
{
    loadFromMHDFile(aFileName.data());
}


//--------------------------------------------------------------------------------------------------
template<typename T, unsigned int L> void Image<T, L>::loadFromMHAFile(const std::string& aFileName)
//--------------------------------------------------------------------------------------------------
{
    loadFromMHAFile(aFileName.data());
}


//---------------------------------------------------------------------------------------------
template<typename T, unsigned int L> void Image<T, L>::loadFromASCIIFile(const char* aFileName)
//---------------------------------------------------------------------------------------------
{
    if (L != 1)
    {
        throw "Invalid pixel type";
    }

    // Empty the image
    clear();

    // Open the file in ASCII mode
    std::ifstream input_file(aFileName);

    // The file is not open
    if (!input_file.is_open())
        {
        // Throw an error
        std::string error_message = "ASCII file does not exist (";
        error_message += aFileName;
        error_message += ")";
        throw Exception(__FILE__, __FUNCTION__, __LINE__, error_message);
        }

    // Read the file
    while (input_file >> *this);

    m_number_of_slices = 1;
}


//-------------------------------------------------------------------------------------------
template<typename T, unsigned int L> void Image<T, L>::loadFromMHDFile(const char* aFileName)
//-------------------------------------------------------------------------------------------
{
    if (L != 1)
    {
        throw "Invalid pixel type";
    }

    // Empty the image
    clear();
    destroy();


    // Open the file in ASCII mode
    std::ifstream input_file(aFileName, std::ios::binary);

    // The file is not open
    if (!input_file.is_open())
        {
        // Throw an error
        std::string error_message = "ASCII file does not exist (";
        error_message += aFileName;
        error_message += ")";
        throw Exception(__FILE__, __FUNCTION__, __LINE__, error_message);
        }

    std::string binary_file_name;
    bool swap_byte;
    loadMHDHeader(input_file, aFileName, binary_file_name, swap_byte);

    float voxel_size[3];
    voxel_size[0] = m_voxel_width;
    voxel_size[1] = m_voxel_height;
    voxel_size[2] = m_voxel_depth;

    // Load RAW file
    if (binary_file_name != "LOCAL")
    {

    	loadFromRawFile(getPath(aFileName) + "/" + binary_file_name, m_width, m_height, m_number_of_slices, swap_byte);
    }
    // Load from input_file
    else
    {
    	loadFromRawStream(input_file, m_width, m_height, m_number_of_slices, swap_byte);
    }

    m_voxel_width  = voxel_size[0];
    m_voxel_height = voxel_size[1];
    m_voxel_depth  = voxel_size[2];
}


template<typename T, unsigned int L> void Image<T, L>::swapByte(char* aWord, size_t aWordSize)
{
	for (int i = 0; i < aWordSize / 2; ++i)
	{
		char temp = aWord[i];
		aWord[i] = aWord[aWordSize - 1 - i];
		aWord[aWordSize - 1 - i] = temp;
	}
}


//-------------------------------------------------------------------------------------------
template<typename T, unsigned int L> void Image<T, L>::loadFromMHAFile(const char* aFileName)
//-------------------------------------------------------------------------------------------
{
	throw "Not implemented";
	loadFromMHDFile(aFileName);
}


template<typename T, unsigned int L> void Image<T, L>::loadFromJPEGFile(const char* aFileName)
{
    if (typeid(T) != typeid(unsigned char))
    {
        throw "Invalid pixel type";
    }
}

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


template<typename T, unsigned int L> void Image<T, L>::loadFromRawFile(const std::string& aFileName,
        unsigned int aWidth,
        unsigned int aHeight,
		unsigned int aNumberOfSlices,
		bool aSwapByteFlag)
{
	loadFromRawFile(aFileName.data(), aWidth, aHeight, aNumberOfSlices, aSwapByteFlag);
}


template<typename T, unsigned int L> void Image<T, L>::loadFromRawFile(const char* aFileName,
        unsigned int aWidth,
        unsigned int aHeight,
		unsigned int aNumberOfSlices,
		bool aSwapByteFlag)
{
    // Empty the image
    clear();
    destroy();

	// Read the data from the input file
    std::ifstream p_input_file(aFileName, std::ios::binary);
    if (!p_input_file.is_open())
    {
        std::string error("Can't open \"");
        error += aFileName;
        error += "\"";

        throw  error;
    }

    loadFromRawStream(p_input_file, aWidth, aHeight, aNumberOfSlices, aSwapByteFlag);
}



template<typename T, unsigned int L> void Image<T, L>::savePGMFile(const std::string& aFileName) const
{
    savePGMFile(aFileName.data());
}

//---------------------------------------------------------
template<typename T, unsigned int L> void Image<T, L>::savePGMFile(const char* aFileName) const
//---------------------------------------------------------
{
    if (typeid(T) != typeid(unsigned char) && typeid(T) != typeid(unsigned short))
    {
        throw "Invalid pixel type";
    }

    if (L != 1)
    {
        throw "Invalid pixel type";
    }


    // Open the file
    std::ofstream output_file(aFileName);

    // The file cannot be created
    // Throw an error
    if (!output_file.is_open())
        {
        std::string error_message = "Cannot save PGM file (";
        error_message += aFileName;
        error_message += ")";
        throw Exception(__FILE__, __FUNCTION__, __LINE__, error_message);
        }
    // The file is open
    else
        {
        // Set the image type
        output_file << "P2" << std::endl;

        // Print a comment
        output_file << "# ICP3038 -- Assignment 1" << std::endl;

        // The image size
        output_file << m_width << " " << m_height << std::endl;

        // The get the max value
        if (typeid(T) == typeid(unsigned char))
        {
            output_file << std::numeric_limits<unsigned char>::max() << std::endl;
        }
        else if (typeid(T) == typeid(unsigned short))
        {
            output_file << std::numeric_limits<unsigned short>::max() << std::endl;
        }
        else
        {
            output_file << getMax() << std::endl;
        }

        // Process every line
        for (int j = 0; j < m_height; ++j)
            {
            // It is not the first line of the image
            if (j != 0)
                {
                output_file << std::endl;
                }

            // Process every column
            for (int i = 0; i < m_width; ++i)
                {
                // It is not the first pixel of the line
                if (i != 0)
                    {
                    output_file << " ";
                    }

                // Process the pixel
                int pixel_value(int(round(m_p_image[j * m_width + i])));
                pixel_value = std::max(0, pixel_value);
                pixel_value = std::min(255, pixel_value);

                output_file << pixel_value;
                }
            }
        }
}


//-------------------------
template<typename T, unsigned int L> T Image<T, L>::getMin() const
//-------------------------
{
    return (*std::min_element(m_p_image.begin(), m_p_image.end()));
}


//-------------------------
template<typename T, unsigned int L> T Image<T, L>::getMax() const
//-------------------------
{
    return (*std::max_element(m_p_image.begin(), m_p_image.end()));
}


//-------------------------
template<typename T, unsigned int L> double Image<T, L>::getSum() const
//-------------------------
{
    return (std::accumulate(m_p_image.begin(), m_p_image.end(), 0.0f));
}


//-----------------------------
template<typename T, unsigned int L> double Image<T, L>::getAverage() const
//-----------------------------
{
    return (getSum() / double(getNumberOfVoxels() * L));
}


//--------------------------
template<typename T, unsigned int L> double Image<T, L>::getMean() const
//--------------------------
{
    return (getAverage());
}


//------------------------------
template<typename T, unsigned int L> double Image<T, L>::getVariance() const
//------------------------------
{
    double variance = 0.0;
    double average = getAverage();

    // Process every pixel
    for (int i = 0; i < getNumberOfVoxels() * L; ++i)
        {
        variance += std::pow(m_p_image[i] - average, 2);
        }

    // Use floating-point division (by oposition to integer arythmetic)
    variance /= double(getNumberOfVoxels() * L);

    return (variance);
}


//---------------------------------------
template<typename T, unsigned int L> double Image<T, L>::getStandardDeviation() const
//---------------------------------------
{
    return (std::sqrt(getVariance()));
}


/*
template<typename T, unsigned int L> void Image<T, L>::loadFromJPEGFile(const std::string& aFileName)
{
    loadFromJPEGFile(aFileName.data());
}


template<typename T, unsigned int L> void Image<T, L>::saveJPEGFile(const char* aFileName) const
{
    // Allocate and initialize a JPEG compression object
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    // Specify the destination for the compressed data (eg, a file)
    FILE* p_output_file(fopen(aFileName, "wb"));
    if (!p_output_file)
    {
        std::string error("Can't open \"");
        error += aFileName;
        error += "\"";

        throw  error;
    }
    jpeg_stdio_dest(&cinfo, p_output_file);

    // Set parameters for compression, including image size & colorspace
    cinfo.image_width  = m_width;   // image width in pixels
    cinfo.image_height = m_height;  // image height in pixels
    cinfo.input_components = L;     // number of color components per pixel

    if (L == 1)
        cinfo.in_color_space = JCS_GRAYSCALE; // colorspace of input image
    else if (L == 3)
        cinfo.in_color_space = JCS_RGB; // colorspace of input image

    jpeg_set_defaults(&cinfo);

    // Start compression
    jpeg_start_compress(&cinfo, TRUE);

    // Compress data
    JSAMPROW row_pointer[1];        // pointer to a single row
    int row_stride;                 // physical row width in buffer

    row_stride = m_width * L;   // JSAMPLEs per row in image_buffer

    unsigned char* p_temp;
    if (typeid(T) == typeid(unsigned char))
    {
        const void* temp1 = static_cast<const void*>(&m_p_image[0]);
        const unsigned char* temp2 = static_cast<const unsigned char*>(temp1);
        p_temp = const_cast<unsigned char*>(temp2);
    }
    else
    {
        p_temp = new unsigned char[m_width * m_height * L];

        for (int i = 0; i < m_width * m_height * L; ++i)
        {
            double pixel = floor(m_p_image[i]);
            p_temp[i] = std::min(255.0, std::max(0.0, pixel));
        }
    }


    while (cinfo.next_scanline < cinfo.image_height)
    {
        row_pointer[0] = & p_temp[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    if (typeid(T) != typeid(unsigned char))
    {
        delete [] p_temp;
    }

    // Finish compression
    jpeg_finish_compress(&cinfo);

    // Release the JPEG compression object
    jpeg_destroy_compress(&cinfo);
}



template<typename T, unsigned int L> void Image<T, L>::saveJPEGFile(const std::string& aFileName) const
{
    saveJPEGFile(aFileName.data());
}
*/

template<typename T, unsigned int L> void Image<T, L>::saveRawFile(const char* aFileName) const
{
    // Open the file in binary
    std::ofstream output_file (aFileName, std::ifstream::binary);

    // The file is not open
    if (!output_file.is_open())
        {
        std::string error_message("The file (");
        error_message += aFileName;
        error_message += ") cannot be created";

        throw error_message;//Exception(__FILE__, __FUNCTION__, __LINE__, error_message);
        }

    // Write content to file
    output_file.write(reinterpret_cast<const char*>(&m_p_image[0]), getNumberOfVoxels() * L * sizeof(T));
}



template<typename T, unsigned int L> void Image<T, L>::saveRawFile(const std::string& aFileName) const
{
    saveRawFile(aFileName.data());
}


template<typename T, unsigned int L> Image<T, 3> Image<T, L>::luminance2RGB() const
{
    if (L != 1)
    {
        throw "Invalid image type";
    }

    Image<T, 3> temp_image(m_width , m_height , m_number_of_slices);

#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel for
#endif
    for (int i = 0; i < getNumberOfVoxels(); ++i)
    {
        temp_image[i * 3 + 0]  = m_p_image[i];
        temp_image[i * 3 + 1] += m_p_image[i];
        temp_image[i * 3 + 2] += m_p_image[i];
    }

    return (temp_image);
}


template<typename T, unsigned int L> Image<T, 1> Image<T, L>::RGB2luminance() const
{
    if (L != 3)
    {
        throw "Invalid image type";
    }

    Image<T, 1> temp_image(m_width , m_height , m_number_of_slices);

#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel for
#endif
    for (int i = 0; i < getNumberOfVoxels(); ++i)
    {
        // Compute the relative luminance from RGB data
        // using Photometric/digital ITU-R
        temp_image[i]  = 0.2126 * m_p_image[i * L    ];
        temp_image[i] += 0.7152 * m_p_image[i * L + 1];
        temp_image[i] += 0.0722 * m_p_image[i * L + 2];
    }

    return (temp_image);
}



template<typename T, unsigned int L> unsigned int Image<T, L>::createOpenGLTexture() const
{
    unsigned int texture_id(0);

#ifdef USE_GUI

    // Create one OpenGL texture
    glGenTextures(1, &texture_id);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, texture_id);

    updateOpenGLTexture(texture_id);

    // When MAGnifying the image (no bigger mipmap available), use LINEAR filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // When MINifying the image, use a LINEAR blend of two mipmaps, each filtered LINEARLY too
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Generate mipmaps, by the way.
    //glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

#endif

    return (texture_id);
}


template<typename T, unsigned int L> void Image<T, L>::updateOpenGLTexture(unsigned int& aTextureID) const
{
    throw "Not implemented";

}

/*
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
*/

//-------------------------------------------------------
template<typename T, unsigned int L> void Image<T, L>::addLine(const std::vector<T>& aLineToAdd)
//-------------------------------------------------------
{
    // The image is empty or the image width is the same as the new line size
    if (m_width == 0 || m_width * L == aLineToAdd.size())
        {
        // Save the image width
        m_width = aLineToAdd.size();

        // Increase the number of rows
        ++m_height;

        // Insert the new data at the end of the image
        m_p_image.insert(
                m_p_image.end(),
                aLineToAdd.begin(),
                aLineToAdd.end() );
        }
    // The image is not empty and the image width is different
    // from the new line size
    // Throw an error
    else
        {
        throw Exception(__FILE__, __FUNCTION__, __LINE__,
                "Input line has a different size from image");
        }
}


//-------------------------------------------------------
template<typename T, unsigned int L> Image<T, L> Image<T, L>::contrastStretching(T aNewMinValue,
                                T aNewMaxValue) const
//-------------------------------------------------------
{
    // The new range is not valid
    if (aNewMinValue > aNewMaxValue ||
            std::abs(aNewMinValue - aNewMaxValue) < 1.0E-5)
        {
        throw Exception(__FILE__, __FUNCTION__, __LINE__,
                "The min value is bigger than the max value");
        }

    // Get the old min value
    T old_min_value(getMin());

    // Get the old max value
    T old_max_value(getMax());

    // Compute the old range of pixel values
    double old_range(old_max_value - old_min_value);

    // Compute the new range of pixel values
    double new_range(aNewMaxValue - aNewMinValue);

    // Use arithmetic operators to implement contrast stretching
    return (aNewMinValue + new_range * (*this - old_min_value) / old_range);
}


template<typename T, unsigned int L> Image<T, L> Image<T, L>::toSinograms() const
{
    if (L != 1)
        {
        throw Exception(__FILE__, __FUNCTION__, __LINE__,
                "The image is not in greyscale");
        }

	Image<T, L> image(getWidth(), getNumberOfSlices(), getHeight());
	image.m_voxel_width  = m_voxel_width;
	image.m_voxel_height = m_voxel_depth;
	image.m_voxel_depth  = m_voxel_height;


#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel for schedule(dynamic,1) collapse(3)
#endif
	for (int z = 0; z < m_number_of_slices; ++z)
	{
		for (int y = 0; y < m_height; ++y)
		{
			for (int x = 0; x < m_width; ++x)
			{
				image(x, z, y) = getPixel(x, y, z);
			}
		}
	}

	return (image);
}


template<typename T, unsigned int L> Image<T, L> Image<T, L>::toProjections() const
{
	return (toSinograms());
}


template<typename T, unsigned int L> Image<T, L> Image<T, L>::getROI(int aStartIndex[3], int aSize[3]) const
{
	Image<T, L> roi(aSize[0], aSize[1], aSize[2]);
	roi.m_voxel_width  = m_voxel_width;
	roi.m_voxel_height = m_voxel_height;
	roi.m_voxel_depth  = m_voxel_depth;

#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel for schedule(dynamic,1) collapse(3)
#endif
	for (int k = 0; k < aSize[2]; ++k)
	{
		for (int j = 0; j < aSize[1]; ++j)
		{
			for (int i = 0; i < aSize[0]; ++i)
			{
				for (int l = 0; l < L; ++l)
				{
					roi(i, j, k, l) = getPixel(i + aStartIndex[0], j + aStartIndex[1], k + aStartIndex[2], l);
				}
			}
		}
	}

	return (roi);
}


//------------------------------------
template<typename T, unsigned int L> Image<T, L>& Image<T, L>::operator+=(double aValue)
//------------------------------------
{
    // Process every pixel
    for (typename std::vector<T>::iterator ite(m_p_image.begin());
            ite != m_p_image.end();
            ++ite)
        {
        *ite += aValue;
        }

    // Return the current instance
    return (*this);
}


//------------------------------------
template<typename T, unsigned int L> Image<T, L>& Image<T, L>::operator-=(double aValue)
//------------------------------------
{
    // Process every pixel
    for (typename std::vector<T>::iterator ite(m_p_image.begin());
            ite != m_p_image.end();
            ++ite)
        {
        *ite -= aValue;
        }

    // Return the current instance
    return (*this);
}


//------------------------------------
template<typename T, unsigned int L> Image<T, L>& Image<T, L>::operator*=(double aValue)
//------------------------------------
{
    // Process every pixel
    for (typename std::vector<T>::iterator ite(m_p_image.begin());
            ite != m_p_image.end();
            ++ite)
        {
        *ite *= aValue;
        }

    // Return the current instance
    return (*this);
}


//------------------------------------
template<typename T, unsigned int L> Image<T, L>& Image<T, L>::operator/=(double aValue)
//------------------------------------
{
    // Division by 0
    if (std::abs(aValue) < 1.0E-5)
        {
        throw Exception(__FILE__, __FUNCTION__, __LINE__, "Division by 0");
        }

    // Process every pixel
    for (typename std::vector<T>::iterator ite(m_p_image.begin());
            ite != m_p_image.end();
            ++ite)
        {
        *ite /= aValue;
        }

    // Return the current instance
    return (*this);
}


//----------------------------------------
template<typename T, unsigned int L> Image<T, L> Image<T, L>::operator+(double aValue) const
//----------------------------------------
{
    // Copy the current instance in a temporary variable
    Image temp = *this;

    // Re-use operator+=
    temp += aValue;

    // Return the new image
    return (temp);
}


//----------------------------------------
template<typename T, unsigned int L> Image<T, L> Image<T, L>::operator-(double aValue) const
//----------------------------------------
{
    // Copy the current instance in a temporary variable
    Image temp = *this;

    // Re-use operator-=
    temp -= aValue;

    // Return the new image
    return (temp);
}


//----------------------------------------
template<typename T, unsigned int L> Image<T, L> Image<T, L>::operator*(double aValue) const
//----------------------------------------
{
    // Copy the current instance in a temporary variable
    Image temp = *this;

    // Re-use operator*=
    temp *= aValue;

    // Return the new image
    return (temp);
}


//----------------------------------------
template<typename T, unsigned int L> Image<T, L> Image<T, L>::operator/(double aValue) const
//----------------------------------------
{
    // Division by 0
    if (std::abs(aValue) < 1.0E-5)
        {
        throw Exception(__FILE__, __FUNCTION__, __LINE__, "Division by 0");
        }

    // Copy the current instance in a temporary variable
    Image temp = *this;

    // Re-use operator/=
    temp /= aValue;

    // Return the new image
    return (temp);
}


template<typename T, unsigned int L> Image<T, L> Image<T, L>::operator+(const Image& anImage) const
{
    Image<T, L> temp(std::min(getWidth(), anImage.getWidth()),
            std::min(getHeight(), anImage.getHeight()),
            std::min(getNumberOfSlices(), anImage.getNumberOfSlices()));

    temp.m_voxel_width  = anImage.m_voxel_width;
    temp.m_voxel_height = anImage.m_voxel_height;
    temp.m_voxel_depth  = anImage.m_voxel_depth;

    int width = temp.getWidth();
    int height = temp.getHeight();
    int depth = temp.getNumberOfSlices();

#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel for schedule(dynamic,1) collapse(3)
#endif
    for (int k = 0; k < depth; ++k)
    {
        for (int j = 0; j < height; ++j)
        {
            for (int i = 0; i < width; ++i)
            {
                for (int l = 0; l < L; ++l)
                {
                    temp(i, j, k, l) = getPixel(i, j, k, l) + anImage(i, j, k, l);
                }
            }
        }
    }

    return (temp);
}


template<typename T, unsigned int L> Image<T, L> Image<T, L>::operator-(const Image& anImage) const
{
    Image<T, L> temp(std::min(getWidth(), anImage.getWidth()),
            std::min(getHeight(), anImage.getHeight()),
            std::min(getNumberOfSlices(), anImage.getNumberOfSlices()));

    temp.m_voxel_width  = anImage.m_voxel_width;
    temp.m_voxel_height = anImage.m_voxel_height;
    temp.m_voxel_depth  = anImage.m_voxel_depth;

    int width = temp.getWidth();
    int height = temp.getHeight();
    int depth = temp.getNumberOfSlices();

#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel for schedule(dynamic,1) collapse(3)
#endif
    for (int k = 0; k < depth; ++k)
    {
        for (int j = 0; j < height; ++j)
        {
            for (int i = 0; i < width; ++i)
            {
                for (int l = 0; l < L; ++l)
                {
                    temp(i, j, k, l) = getPixel(i, j, k, l) - anImage(i, j, k, l);
                }
            }
        }
    }

    return (temp);
}


template<typename T, unsigned int L> Image<T, L> Image<T, L>::operator*(const Image& anImage) const
{
    Image<T, L> temp(std::min(getWidth(), anImage.getWidth()),
            std::min(getHeight(), anImage.getHeight()),
            std::min(getNumberOfSlices(), anImage.getNumberOfSlices()));

    temp.m_voxel_width  = anImage.m_voxel_width;
    temp.m_voxel_height = anImage.m_voxel_height;
    temp.m_voxel_depth  = anImage.m_voxel_depth;

    int width = temp.getWidth();
    int height = temp.getHeight();
    int depth = temp.getNumberOfSlices();

#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel for schedule(dynamic,1) collapse(3)
#endif
    for (int k = 0; k < depth; ++k)
    {
        for (int j = 0; j < height; ++j)
        {
            for (int i = 0; i < width; ++i)
            {
                for (int l = 0; l < L; ++l)
                {
                    temp(i, j, k, l) = getPixel(i, j, k, l) * anImage(i, j, k, l);
                }
            }
        }
    }

    return (temp);
}

template<typename T, unsigned int L> Image<T, L> Image<T, L>::operator/(const Image& anImage) const
{
    Image<T, L> temp(std::min(getWidth(), anImage.getWidth()),
            std::min(getHeight(), anImage.getHeight()),
            std::min(getNumberOfSlices(), anImage.getNumberOfSlices()));

    temp.m_voxel_width  = anImage.m_voxel_width;
    temp.m_voxel_height = anImage.m_voxel_height;
    temp.m_voxel_depth  = anImage.m_voxel_depth;

    int width = temp.getWidth();
    int height = temp.getHeight();
    int depth = temp.getNumberOfSlices();

#if defined(USE_OPENMP) && defined(NDEBUG)
#pragma omp parallel for schedule(dynamic,1) collapse(3)
#endif
    for (int k = 0; k < depth; ++k)
    {
        for (int j = 0; j < height; ++j)
        {
            for (int i = 0; i < width; ++i)
            {
                for (int l = 0; l < L; ++l)
                {
                    temp(i, j, k, l) = getPixel(i, j, k, l) / anImage(i, j, k, l);
                }
            }
        }
    }

    return (temp);
}

template<typename T, unsigned int L> Image<T, L>& Image<T, L>::operator+=(const Image& anImage)
{
    Image<T, L> temp((*this) + anImage);
    *this = temp;
    return (*this);
}


template<typename T, unsigned int L> Image<T, L>& Image<T, L>::operator-=(const Image& anImage)
{
    Image<T, L> temp((*this) - anImage);
    *this = temp;
    return (*this);
}


template<typename T, unsigned int L> Image<T, L>& Image<T, L>::operator*=(const Image& anImage)
{
    Image<T, L> temp((*this) * anImage);
    *this = temp;
    return (*this);
}


template<typename T, unsigned int L> Image<T, L>& Image<T, L>::operator/=(const Image& anImage)
{
    Image<T, L> temp((*this) / anImage);
    *this = temp;
    return (*this);
}


//-------------------------------------------------
template<typename T, unsigned int L> Image<T, L> operator+(double aValue, const Image<T, L>& anImage)
//-------------------------------------------------
{
    // Addition is commutative, use Image::operator+
    return (anImage + aValue);
}


//-------------------------------------------------
template<typename T, unsigned int L> Image<T, L> operator-(double aValue, const Image<T, L>& anImage)
//-------------------------------------------------
{
    // Create a new image of the right size
    Image<T, L> temp(anImage.getWidth(), anImage.getHeight(), anImage.getNumberOfSlices());
    temp.m_voxel_width  = anImage.m_voxel_width;
    temp.m_voxel_height = anImage.m_voxel_height;
    temp.m_voxel_depth  = anImage.m_voxel_depth;

    // Process every pixel
    for (int i = 0; i < anImage.getNumberOfVoxels() * L; ++i)
        {
        temp[i] = aValue - anImage[i];
        }

    // Return the new image
    return (temp);
}


//-------------------------------------------------
template<typename T, unsigned int L> Image<T, L> operator*(double aValue, const Image<T, L>& anImage)
//-------------------------------------------------
{
    // Multiplication is commutative, use Image::operator*
    return (anImage * aValue);
}


//-------------------------------------------------
template<typename T, unsigned int L> Image<T, L> operator/(double aValue, const Image<T, L>& anImage)
//-------------------------------------------------
{
    // Create a new image of the right size
    Image<T, L> temp(anImage.getWidth(), anImage.getHeight(), anImage.getNumberOfSlices());
    temp.m_voxel_width  = anImage.m_voxel_width;
    temp.m_voxel_height = anImage.m_voxel_height;
    temp.m_voxel_depth  = anImage.m_voxel_depth;

    // Process every pixel
    for (int i = 0; i < anImage.getNumberOfVoxels() * L; ++i)
        {
        // Division by 0
        if (std::abs(anImage[i]) < 1.0E-5)
            {
            throw Exception(__FILE__, __FUNCTION__, __LINE__, "Division by 0");
            }

        temp[i] = aValue / anImage[i];
        }

    // Return the new image
    return (temp);
}


//---------------------------------------------------------------------------
template<typename T, unsigned int L> std::ostream& operator<< (std::ostream& anOutputStream, const Image<T, L>& anImage)
//---------------------------------------------------------------------------
{
    if (L != 1)
    {
        throw "Invalid pixel type";
    }

    if (anImage.getNumberOfSlices() > 1)
    {
        throw "Invalid image size";
    }

    // The image has pixels, i.e. it is not empty
    if (anImage.getWidth() * anImage.getHeight())
        {
        // Process every row
        for (int j = 0; j < anImage.getHeight(); ++j)
            {
            // No new line before 1st row
            if (j != 0)
                {
                anOutputStream << std::endl;
                }

            // Process every column
            for (int i = 0; i < anImage.getWidth(); ++i)
                {
                // No tab before 1st pixel of the row
                if (i != 0)
                    {
                    anOutputStream << "\t";
                    }

                // Get the pixel and output its value in the stream
                anOutputStream << anImage(i, j);
                }
            }
        }
    // The image has no pixel, i.e. it is empty
    else
        {
        //Output a comment in the stream
        anOutputStream << "# empty matrix";
        }

    // Return the modified stream
    return (anOutputStream);
}


//--------------------------------------------------------------------
template<typename T, unsigned int L> std::istream& operator>> (std::istream& anInputStream, Image<T, L>& anImage)
//--------------------------------------------------------------------
{
    if (L != 1)
    {
        throw "Invalid pixel type";
    }

    // Array to store new pixels from the stream
    std::vector<T> temp_data;

    // Get the current size of the image
    unsigned int image_width = anImage.getWidth();

    // Get a new line from the stream
    std::string line;
    while (std::getline(anInputStream, line))
        {
        // The line does not start with '#', it is not a comment, read the line
        if (line[0] != '#')
            {
            // Get every pixel of the line
            std::istringstream iss(line);
            T temp;
            while (iss >> temp)
                {
                // Add the new pixel
                temp_data.push_back(temp);
                }

            // There was already pixels in the image
            if (image_width)
                {
                // The new line has a different number of pixels,
                // it is not `compatible' with the image
                if (image_width != temp_data.size() && temp_data.size())
                    {
                    std::string error_message =
                            "Input stream has lines of different sizes";

                    throw Exception(__FILE__, __FUNCTION__, __LINE__,
                            error_message);
                    }
                }
            // The image was empty
            else
                {
                // Update the image size
                image_width = temp_data.size();
                }

            // Add the line to the image
            if (temp_data.size())
            {
                anImage.addLine(temp_data);
            }

            // Clear the array
            temp_data.clear();
            }
        }

    // Return the modified stream
    return (anInputStream);
}
