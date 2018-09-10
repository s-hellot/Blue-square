/*

Copyright (c) 2017, Dr Franck P. Vidal (franck.p.vidal@fpvidal.net),
http://www.fpvidal.net/
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

3. Neither the name of the Bangor University nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


#ifndef __Image_h
#define __Image_h


/**
********************************************************************************
*
*   @file       Image.h
*
*   @brief      Class to handle a greyscale image.
*
*   @version    1.0
*
*   @todo       Fix problem in marchingCubes (normals?).
*               See bug in demos (ImplicitSurface and SimpleBackProjection)
*
*   @date       30/09/2016
*
*   @author     Franck Vidal
*
*
********************************************************************************
*/


//******************************************************************************
//  Include
//******************************************************************************
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <string>
#include <vector>
#include <fstream>

#ifdef USE_CUDA
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#endif

#ifdef USE_GUI
#include <GLFW/glfw3.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#endif


//******************************************************************************
//  namespace
//******************************************************************************


//==============================================================================
/**
*   @class  Image
*   @brief  Image is a class to manage a greyscale image.
*/
//==============================================================================
template<typename T, unsigned int L = 1> class Image
//------------------------------------------------------------------------------
{
//******************************************************************************
public:
    //--------------------------------------------------------------------------
    /// Default constructor.
    //--------------------------------------------------------------------------
    Image();


    //--------------------------------------------------------------------------
    /// Copy constructor.
    /**
     *  @param anImage: the image to copy
     */
    //--------------------------------------------------------------------------
    Image(const Image<T, L>& anImage);


    //--------------------------------------------------------------------------
    /// Constructor.
    /**
     *  @param aFileName: the name of the file to load
     */
    //--------------------------------------------------------------------------
    Image(const char* aFileName);


    //--------------------------------------------------------------------------
    /// Constructor.
    /**
     *  @param aFileName: the name of the file to load
     */
    //--------------------------------------------------------------------------
    Image(const std::string& aFileName);


    //--------------------------------------------------------------------------
    /// Constructor from an array.
    /**
     *  @param apData:          the array to copy
     *  @param aWidth:          the width of the image
     *  @param aHeight:         the height of the image
     *  @param aNumberOfSlices: the number of slices (default value: 1)
     */
    //--------------------------------------------------------------------------
    Image(const T* apData,
                 unsigned int aWidth,
                 unsigned int aHeight,
                 unsigned int aNumberOfSlices = 1);


    //--------------------------------------------------------------------------
    /// Constructor to build a black image.
    /**
     *  @param aWidth:          the width of the image
     *  @param aHeight:         the height of the image
     *  @param aNumberOfSlices: the number of slices (default value: 1)
     *  @param aDefaultValue:   the pixel value (default value: 0)
     */
    //--------------------------------------------------------------------------
    Image(unsigned int aWidth,
                 unsigned int aHeight,
                 unsigned int aNumberOfSlices = 1,
                 T aDefaultValue = 0);


    //--------------------------------------------------------------------------
    /// Destructor.
    //--------------------------------------------------------------------------
    ~Image();


    Image<T, L>& operator=(const Image<T, L>& anImage);


    void clear();


    //--------------------------------------------------------------------------
    /// Load the image from a file. Supported extensions:
    /// jpg, jpeg, txt.
    /**
     * @param aFileName: the name of the file to load
     */
    //--------------------------------------------------------------------------
    void loadFromFile(const std::string& aFileName);
    void loadFromFile(const char* aFileName);


    //--------------------------------------------------------------------------
    /// Load the image from a JPEG file.
    /**
     * @param aFileName: the name of the file to load
     */
    //--------------------------------------------------------------------------
    void loadFromJPEGFile(const std::string& aFileName);
    void loadFromJPEGFile(const char* aFileName);


    //--------------------------------------------------------------------------
    /// Load the image from an ASCII file.
    /// Lines starting with character '#' are comments.
    /// Each row corresponds to a line in the text file.
    /// Each column is separated from other by space or tab characters.
    /**
     * @param aFileName: the name of the file to load
     */
    //--------------------------------------------------------------------------
    void loadFromASCIIFile(const std::string& aFileName);
    void loadFromASCIIFile(const char* aFileName);


    //--------------------------------------------------------------------------
    /// Load the image from a MHD file.
    /// Lines starting with character '#' are comments.
    /// Each row corresponds to a line in the text file.
    /// Each column is separated from other by space or tab characters.
    /**
     * @param aFileName: the name of the file to load
     */
    //--------------------------------------------------------------------------
    void loadFromMHDFile(const std::string& aFileName);
    void loadFromMHDFile(const char* aFileName);


    //--------------------------------------------------------------------------
    /// Load the image from a MHA file.
    /// Lines starting with character '#' are comments.
    /// Each row corresponds to a line in the text file.
    /// Each column is separated from other by space or tab characters.
    /**
     * @param aFileName: the name of the file to load
     */
    //--------------------------------------------------------------------------
    void loadFromMHAFile(const std::string& aFileName);
    void loadFromMHAFile(const char* aFileName);


    //--------------------------------------------------------------------------
    /// Load the image from a binaray raw file.
    /**
     * @param aFileName: the name of the file to load
     * @param aWidth: the number of columns
     * @param aHeight: the number of rows
     */
    //--------------------------------------------------------------------------
    void loadFromRawFile(const std::string& aFileName,
            unsigned int aWidth,
            unsigned int aHeight,
			unsigned int aNumberOfSlices,
			bool aSwapByteFlag);
    void loadFromRawFile(const char* aFileName,
            unsigned int aWidth,
            unsigned int aHeight,
			unsigned int aNumberOfSlices,
			bool aSwapByteFlag);


    //--------------------------------------------------------------------------
    /// Save the image into a file. Supported extensions:
    /// jpg, jpeg, txt, raw, pgm.
    /**
     * @param aFileName: the name of the file to write
     */
    //--------------------------------------------------------------------------
    void saveFile(const std::string& aFileName) const;
    void saveFile(const char* aFileName) const;


    //--------------------------------------------------------------------------
    /// Save the image in a PGM file
    /**
     * @param aFileName: the name of the file to write
     */
    //--------------------------------------------------------------------------
    void savePGMFile(const std::string& aFileName) const;
    void savePGMFile(const char* aFileName) const;


    //--------------------------------------------------------------------------
    /// Save the image in a JPEG file
    /**
     * @param aFileName: the name of the file to write
     */
    //--------------------------------------------------------------------------
    void saveJPEGFile(const std::string& aFileName) const;
    void saveJPEGFile(const char* aFileName) const;


    //--------------------------------------------------------------------------
    /// Save the image in a ASCII file
    /**
     * @param aFileName: the name of the file to write
     */
    //--------------------------------------------------------------------------
    void saveASCIIFile(const std::string& aFileName) const;
    void saveASCIIFile(const char* aFileName) const;


    //--------------------------------------------------------------------------
    /// Save the image in a MHD file
    /**
     * @param aFileName: the name of the file to write
     */
    //--------------------------------------------------------------------------
    void saveMHDFile(const std::string& aFileName) const;
    void saveMHDFile(const char* aFileName) const;


    //--------------------------------------------------------------------------
    /// Save the image in a MHA file
    /**
     * @param aFileName: the name of the file to write
     */
    //--------------------------------------------------------------------------
    void saveMHAFile(const std::string& aFileName) const;
    void saveMHAFile(const char* aFileName) const;


    //--------------------------------------------------------------------------
    /// Save the image in a RAW file
    /**
     * @param aFileName: the name of the file to write
     */
    //--------------------------------------------------------------------------
    void saveRawFile(const std::string& aFileName) const;
    void saveRawFile(const char* aFileName) const;


    Image<T, 3> luminance2RGB() const;
    Image<T, 1> RGB2luminance() const;


    unsigned int getWidth() const;
    unsigned int getHeight() const;
    unsigned int getNumberOfSlices() const;
    unsigned int getNumberOfVoxels() const;

    unsigned int getNumberOfColourChannels() const;
    unsigned int getNumberOfColorChannels() const;

    void setVoxelWidth(const double& aSize);
    void setVoxelHeight(const double& aSize);
    void setVoxelDepth(const double& aSize);

    const double& getVoxelWidth() const;
    const double& getVoxelHeight() const;
    const double& getVoxelDepth() const;


    T* getRawDataPointer();
    const T* getRawDataPointer() const;


    Image<T, L> flipVertically() const;
    Image<T, L> flipHorizontally() const;

    Image<T, L> operator!() const;

    Image<T, L> normaliseMeanStdDev() const;
    Image<T, L> normaliseZeroToOne() const;
    Image<T, L> log() const;

    Image<T, L> abs() const;

    //--------------------------------------------------------------------------
    /// Find the smallest pixel value.
    /**
     * @return the smallest pixel value
     */
    //--------------------------------------------------------------------------
    T getMin() const;


    //--------------------------------------------------------------------------
    /// Find the largest pixel value.
    /**
     * @return the largest pixel value
     */
    //--------------------------------------------------------------------------
    T getMax() const;


    //--------------------------------------------------------------------------
    /// Compute the sum of all pixel value of the image.
    /**
     * @return the sum
     */
    //--------------------------------------------------------------------------
    double getSum() const;


    //--------------------------------------------------------------------------
    /// Compute the average pixel value.
    /**
     * @return the average pixel value
     */
    //--------------------------------------------------------------------------
    double getAverage() const;


    //--------------------------------------------------------------------------
    /// Compute the average pixel value.
    /**
     * @return the average pixel value
     */
    //--------------------------------------------------------------------------
    double getMean() const;


    //--------------------------------------------------------------------------
    /// Compute the variance of pixel values.
    /**
     * @return the variance
     */
    //--------------------------------------------------------------------------
    double getVariance() const;


    //--------------------------------------------------------------------------
    /// Compute the standard deviation of pixel values.
    /**
     * @return the standard deviation
     */
    //--------------------------------------------------------------------------
    double getStandardDeviation() const;


    double getSAE(const Image<T, L>& anImage) const;
    double getEuclideanDistance(const Image<T, L>& anImage) const;
    double getMAE(const Image<T, L>& anImage) const;
    double getSSE(const Image<T, L>& anImage) const;
    double getMSE(const Image<T, L>& anImage) const;
    double getRMSE(const Image<T, L>& anImage) const;

    double getSAD(const Image<T, L>& anImage) const;
    double getMAD(const Image<T, L>& anImage) const;
    double getSSD(const Image<T, L>& anImage) const;
    double getMSD(const Image<T, L>& anImage) const;
    double getRMSD(const Image<T, L>& anImage) const;

    double getL1Norm(const Image<T, L>& anImage) const;
    double getL2Norm(const Image<T, L>& anImage) const;

    double getSNR(const Image<T, L>& aReferenceImage) const;
    double getPSNR(const Image<T, L>& aReferenceImage) const;
    double getSSIM(const Image<T, L>& aReferenceImage) const;
    double getDSSIM(const Image<T, L>& aReferenceImage) const;

    double getIsotropicTotalVariation(unsigned int aFlag = 0, const double& aSigmaValue = 2) const;
    double getAnisotropicTotalVariation(unsigned int aFlag = 0, const double& aSigmaValue = 2) const;

    double getDotProduct(const Image<T, L>& anImage) const;

    double getZNCC(const Image<T, L>& anImage) const;
    double getRelativeError(const Image<T, L>& aReferenceImage) const;

    double getCovariance(const Image<T, L>& anImage) const;

    unsigned int createOpenGLTexture() const;
    void updateOpenGLTexture(unsigned int& aTextureID) const;

    void setToConstantValue(const T& aValue);


    //--------------------------------------------------------------------------
    /// Add a new row to the image.
    /// Note: if there is already at least a row, the new row must has
    /// the same number of columns
    /**
     * @param aLineToAdd: the row to add
     */
    //--------------------------------------------------------------------------
    void addLine(const std::vector<T>& aLineToAdd);


    //--------------------------------------------------------------------------
    /// Accessor on a pixel given its 2-D index in the image.
    /**
     * @param i: the column number
     * @param j: the row number
     * @return the pixel (reference)
     */
    //--------------------------------------------------------------------------
    T& getPixel(unsigned int i, unsigned int j, unsigned int k = 0, unsigned int l = 0);
    const T& getPixel(unsigned int i, unsigned int j, unsigned int k = 0, unsigned int l = 0) const;


    //--------------------------------------------------------------------------
    /// Accessor on a pixel given its 2-D index in the image.
    /**
     * @param i: the column number
     * @param j: the row number
     * @return the pixel (reference)
     */
    //--------------------------------------------------------------------------
    T& operator()(unsigned int i, unsigned int j, unsigned int k = 0, unsigned int l = 0);


    //--------------------------------------------------------------------------
    /// Accessor on a pixel given its 2-D index in the image.
    /**
     * @param i: the column number
     * @param j: the row number
     * @return the pixel (reference)
     */
    //--------------------------------------------------------------------------
    const T& operator()(unsigned int i, unsigned int j, unsigned int k = 0, unsigned int l = 0) const;


    //--------------------------------------------------------------------------
    /// Accessor on a pixel given its 1-D index in the data array.
    /**
     * @param anElementIndex: the 1-D index
     * @return the pixel (reference)
     */
    //--------------------------------------------------------------------------
    T& operator[](unsigned int anElementIndex);


    //--------------------------------------------------------------------------
    /// Accessor on a pixel given its 1-D index in the data array.
    /**
     * @param anElementIndex: the 1-D index
     * @return the pixel (reference)
     */
    //--------------------------------------------------------------------------
    const T& operator[](unsigned int anElementIndex) const;


    //--------------------------------------------------------------------------
    /// Addition-assignment operator. Add aValue to every pixel of the image.
    /// Allows the use of:
    /// Image temp;
    /// ...
    /// double a;
    /// temp += a;
    /**
     * @param aValue: the value for the addition
     * @return the updated version of the current image
     */
    //--------------------------------------------------------------------------
    Image& operator+=(double aValue);


    //--------------------------------------------------------------------------
    /// Subtraction-assignment operator. Subtract aValue to every pixel of
    //  the image.
    /// Allows the use of:
    /// Image temp;
    /// ...
    /// double a;
    /// temp -= a;
    /**
     * @param aValue: the value for the subtraction
     * @return the updated version of the current image
     */
    //--------------------------------------------------------------------------
    Image& operator-=(double aValue);


    //--------------------------------------------------------------------------
    /// Multiplication-assignment operator. Multiply every pixel of the image
    /// by aValue.
    /// Allows the use of:
    /// Image temp;
    /// ...
    /// double a;
    /// temp *= a;
    /**
     * @param aValue: the value for the multiplication
     * @return the updated version of the current image
     */
    //--------------------------------------------------------------------------
    Image& operator*=(double aValue);


    //--------------------------------------------------------------------------
    /// Division-assignment operator. Divide every pixel of the image by aValue.
    /// Allows the use of:
    /// Image temp;
    /// ...
    /// double a;
    /// temp /= a;
    /**
     * @param aValue: the value for the division
     * @return the updated version of the current image
     */
    //--------------------------------------------------------------------------
    Image& operator/=(double aValue);


    //--------------------------------------------------------------------------
    /// Addition operator. Add aValue to every pixel of the image.
    /// Allows the use of:
    /// Image temp;
    /// ...
    /// double a;
    /// Image result = temp + a;
    ///
    /// Note: temp + a is equivalent to a + temp (the addition is commutative)
    /**
     * @param aValue: the value for the addition
     * @return the new image
     */
    //--------------------------------------------------------------------------
    Image operator+(double aValue) const;


    //--------------------------------------------------------------------------
    /// Subtraction operator. Subtract aValue to every pixel of the image.
    /// Allows the use of:
    /// Image temp;
    /// ...
    /// double a;
    /// Image result = temp - a;
    ///
    /// Note: temp - a is not equivalent to a - temp
    /// (the subtraction is commutative)
    /**
     * @param aValue: the value for the subtraction
     * @return the new image
     */
    //--------------------------------------------------------------------------
    Image operator-(double aValue) const;


    //--------------------------------------------------------------------------
    /// Multiplication operator. Multiply every pixel of the image by aValue.
    /// Allows the use of:
    /// Image temp;
    /// ...
    /// double a;
    /// Image result = temp * a;
    ///
    /// Note: temp * a is equivalent to a * temp
    /// (the multiplication is commutative)
    /**
     * @param aValue: the value for the multiplication
     * @return the new image
     */
    //--------------------------------------------------------------------------
    Image operator*(double aValue) const;


    Image operator+(const Image& anImage) const;
    Image operator-(const Image& anImage) const;
    Image operator*(const Image& anImage) const;
    Image operator/(const Image& anImage) const;

    Image& operator+=(const Image& anImage);
    Image& operator-=(const Image& anImage);
    Image& operator*=(const Image& anImage);
    Image& operator/=(const Image& anImage);


    //--------------------------------------------------------------------------
    /// Division operator. Divide every pixel of the image by aValue.
    /// Allows the use of:
    /// Image temp;
    /// ...
    /// double a;
    /// Image result = temp / a;
    ///
    /// Note: temp / a is not equivalent to a / temp
    /// (the division is not commutative)
    /**
     * @param aValue: the value for the division
     * @return the new image
     */
    //--------------------------------------------------------------------------
    Image operator/(double aValue) const;


    //--------------------------------------------------------------------------
    /// Contrast stretching.
    /**
     * @param aNumberOfBins: the number of bins in the histogram
     * @param aFileName: the name of the file to save
     * @return the new image after contrast stretching
     */
    //--------------------------------------------------------------------------
    Image contrastStretching(T aNewMinValue,
            T aNewMaxValue) const;


    Image toSinograms() const;


    Image toProjections() const;


    Image getROI(int aStartIndex[3], int aSize[3]) const;


    static std::string getFileType(const char* aFileName);


    static std::string getFileType(const std::string& aFileName);


    static std::string getFileNameWithoutExtension(const char* aFileName);


    static std::string getFileNameWithoutExtension(const std::string& aFileName);


    static std::string getFileNameWithoutPath(const char* aFileName);


    static std::string getFileNameWithoutPath(const std::string& aFileName);


    static std::string getPath(const std::string& aFileName);


    static void swapByte(char* aWord, size_t aWordSize);


//******************************************************************************
protected:
    void destroy();




    double weightTV1(const double& aTVValue, const double& aSigmaValue) const;
    double weightTV2(const double& aTVValue, const double& aSigmaValue) const;


    void loadMHDHeader(std::ifstream& anInputStream,
    		const char* aFileName,
			std::string& aBinaryFileName,
		    bool& aSwapByteFlag);


    void loadFromRawStream(std::ifstream& anInputStream,
    		unsigned int aWidth,
            unsigned int aHeight,
    		unsigned int aNumberOfSlices,
			bool aSwapByteFlag);

    /// Number of pixel along the horizontal axis
    unsigned int m_width;


    /// Number of pixel along the vertical axis
    unsigned int m_height;


    /// Number of slices
    unsigned int m_number_of_slices;


    double m_voxel_width;
    double m_voxel_height;
    double m_voxel_depth;

    /// The pixel data
    std::vector<T> m_p_image;


    /// Flag set to true if the statistic data is up-to-date, to false if it is not
    bool m_is_stats_up_to_date;


    /// The lowest pixel value of the image
    T m_min_value[L];


    /// The biggest pixel value of the image
    T m_max_value[L];


    /// The average pixel value of the image
    T m_average[L];


    /// The variance of the pixel values of the image
    T m_variance[L];
};



//******************************************************************************
//  Function declarations
//******************************************************************************

//------------------------------------------------------------------------------
/// operator<<. Print an image in an output stream.
/// Allows the use of:
/// Image temp;
/// ofstream ascii_file("filename.txt");
/// ...
/// cout << temp << endl;
/// ascii_file << temp << endl;
/**
* @param anOutputStream: the output stream
* @param anImage: the image to print in the stream
* @return the output stream
*/
//------------------------------------------------------------------------------
template<typename T, unsigned int L> std::ostream& operator<< (std::ostream& anOutputStream, const Image<T, L>& anImage);


//------------------------------------------------------------------------------
/// operator>>. Set an image from an input stream.
/// Allows the use of:
/// Image temp;
/// ifstream ascii_file("filename.txt");
/// ...
/// while(ascii_file >> temp);
/**
* @param anInputStream: the input stream
* @param anImage: the image to read from the stream
* @return the input stream
*/
//------------------------------------------------------------------------------
template<typename T, unsigned int L> std::istream& operator>> (std::istream& anInputStream, Image<T, L>& anImage);


//------------------------------------------------------------------------------
/// Addition operator. Add aValue to every pixel of anImage.
/// Allows the use of:
/// Image temp;
/// ...
/// double a;
/// Image result = a + temp;
///
/// Note: a + temp is equivalent to temp + a (the addition is commutative)
/**
* @param aValue: the value for the addition (left operand)
* @param anImage: the image to use (right operand)
* @return the new image
*/
//------------------------------------------------------------------------------
template<typename T, unsigned int L> Image<T, L> operator+(double aValue, const Image<T, L>& anImage);


//------------------------------------------------------------------------------
/// Subtraction operator. Subtract pixel values to aValue.
/// Allows the use of:
/// Image temp;
/// ...
/// double a;
/// Image result = a - temp;
///
/// Note: a - temp is not equivalent to temp - a
/// (the subtraction is not commutative)
/**
* @param aValue: the value for the subtraction (left operand)
* @param anImage: the image to use (right operand)
* @return the new image
*/
//------------------------------------------------------------------------------
template<typename T, unsigned int L> Image<T, L> operator-(double aValue, const Image<T, L>& anImage);


//------------------------------------------------------------------------------
/// Multiplication operator. Multiply every pixel of anImage by aValue.
/// Allows the use of:
/// Image temp;
/// ...
/// double a;
/// Image result = a * temp;
///
/// Note: a * temp is equivalent to temp * a (the multiplication is commutative)
/**
* @param aValue: the value for the multiplication (left operand)
* @param anImage: the image to use (right operand)
* @return the new image
*/
//------------------------------------------------------------------------------
template<typename T, unsigned int L> Image<T, L> operator*(double aValue, const Image<T, L>& anImage);


//------------------------------------------------------------------------------
/// Division operator. Divide aValue by pixel values.
/// Allows the use of:
/// Image temp;
/// ...
/// double a;
/// Image result = a / temp;
///
/// Note: a / temp is not equivalent to temp / a
/// (the division is not commutative)
/**
* @param aValue: the value for the division (left operand)
* @param anImage: the image to use (right operand)
* @return the new image
*/
//------------------------------------------------------------------------------
template<typename T, unsigned int L> Image<T, L> operator/(double aValue, const Image<T, L>& anImage);







/*template<typename T> Image<T> operator*(const T& aValue,
                                        const Image<T>& anImage);


template<typename T> Image<T> operator/(const T& aValue,
                                        const Image<T>& anImage);


template<typename T> Image<T> operator+(const T& aValue,
                                        const Image<T>& anImage);


template<typename T> Image<T> operator-(const T& aValue,
                                        const Image<T>& anImage);


template<typename T> Image<T> log(const Image<T>& anImage);


template<typename T> Image<T> abs(const Image<T>& anImage);


std::string getPixelType(const std::string& aFileName);


std::string getPixelType(const char* aFileName);
*/


//******************************************************************************
#include "Image.inl"


#endif // __Image_h
