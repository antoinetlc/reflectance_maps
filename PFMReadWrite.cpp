/*
 *     Reflectance Maps
 *
 *     Authors:  Antoine TOISOUL LE CANN
 *
 *     Copyright © 2016 Antoine TOISOUL LE CANN
 *              All rights reserved
 *
 *
 * Reflectance Maps is free software: you can redistribute it and/or modify
 *
 * it under the terms of the GNU Lesser General Public License as published by
 *
 * the Free Software Foundation, either version 3 of the License, or
 *
 * (at your option) any later version.
 *
 * Reflectance Maps is distributed in the hope that it will be useful,
 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 *
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file PFMReadWrite.cpp
 * \brief Implementation of loadPFM and savePFM functions.
 * \author Antoine Toisoul Le Cann
 * \date September, 1st, 2016
 *
 * Implementation of loadPFM and savePFM functions.
 */

#include "PFMReadWrite.h"

using namespace std;
using namespace cv;

/**
 * Loads a PFM image stored in little endian and returns the image as an OpenCV Mat.
 * @brief loadPFM
 * @param filePath
 * @return
 */
Mat loadPFM(const string filePath)
{

    //Open binary file
    ifstream file(filePath.c_str(),  ios::in | ios::binary);

    Mat imagePFM;

    //If file correctly openened
    if(file)
    {
        //Read the type of file plus the 0x0a UNIX return character at the end
        char type[3];
        file.read(type, 3*sizeof(char));

        //Read the width and height
        unsigned int width(0), height(0);
        file >> width >> height;

        //Read the 0x0a UNIX return character at the end
        char endOfLine;
        file.read(&endOfLine, sizeof(char));

        int numberOfComponents(0);
        //The type gets the number of color channels
        if(type[1] == 'F')
        {
            imagePFM = Mat(height, width, CV_32FC3);
            numberOfComponents = 3;
        }
        else if(type[1] == 'f')
        {
            imagePFM = Mat(height, width, CV_32FC1);
            numberOfComponents = 1;
        }

        //TODO Read correctly depending on the endianness
        //Read the endianness plus the 0x0a UNIX return character at the end
        //Byte Order contains -1.0 or 1.0
        char byteOrder[4];
        file.read(byteOrder, 4*sizeof(char));

        //Find the last line return 0x0a before the pixels of the image
        char findReturn = ' ';
        while(findReturn != 0x0a)
        {
          file.read(&findReturn, sizeof(char));
        }

        //Read each RGB colors as 3 floats and store it in the image.
        float *color = new float[numberOfComponents];
        for(unsigned int i = 0 ; i<height ; ++i)
        {
            for(unsigned int j = 0 ; j<width ; ++j)
            {
                file.read((char*) color, numberOfComponents*sizeof(float));

                //In the PFM format the image is upside down
                if(numberOfComponents == 3)
                {
                    //OpenCV stores the color as BGR
                    imagePFM.at<Vec3f>(height-1-i,j) = Vec3f(color[2], color[1], color[0]);
                }
                else if(numberOfComponents == 1)
                {
                    //OpenCV stores the color as BGR
                    imagePFM.at<float>(height-1-i,j) = color[0];
                }
            }
        }

        delete[] color;

        //Close file
        file.close();
    }
    else
    {
        cerr << "Could not open the file : " << filePath << endl;
    }

    return imagePFM;
}


/**
 * Saves the image as a PFM file.
 * @brief savePFM
 * @param image
 * @param filePath
 * @return
 */
bool savePFM(const cv::Mat image, const std::string filePath)
{
    //Open the file as binary!
    ofstream imageFile(filePath.c_str(), ios::out | ios::trunc | ios::binary);

    if(imageFile)
    {
        int width(image.cols), height(image.rows);
        int numberOfComponents(image.channels());

        //Write the type of the PFM file and ends by a line return
        char type[3];
        type[0] = 'P';
        type[2] = 0x0a;

        if(numberOfComponents == 3)
        {
            type[1] = 'F';
        }
        else if(numberOfComponents == 1)
        {
            type[1] = 'f';
        }

        imageFile << type[0] << type[1] << type[2];

        //Write the width and height and ends by a line return
        imageFile << width << " " << height << type[2];

        //Assumes little endian storage and ends with a line return 0x0a
        //Stores the type
        char byteOrder[10];
        byteOrder[0] = '-'; byteOrder[1] = '1'; byteOrder[2] = '.'; byteOrder[3] = '0';
        byteOrder[4] = '0'; byteOrder[5] = '0'; byteOrder[6] = '0'; byteOrder[7] = '0';
        byteOrder[8] = '0'; byteOrder[9] = 0x0a;

        for(int i = 0 ; i<10 ; ++i)
        {
            imageFile << byteOrder[i];
        }

        //Store the floating points RGB color upside down, left to right
        float* buffer = new float[numberOfComponents];

        for(int i = 0 ; i<height ; ++i)
        {
            for(int j = 0 ; j<width ; ++j)
            {
                if(numberOfComponents == 1)
                {
                    buffer[0] = image.at<float>(height-1-i,j);
                }
                else
                {
                    Vec3f color = image.at<Vec3f>(height-1-i,j);

                   //OpenCV stores as BGR
                    buffer[0] = color.val[2];
                    buffer[1] = color.val[1];
                    buffer[2] = color.val[0];
                }

                //Write the values
                imageFile.write((char *) buffer, numberOfComponents*sizeof(float));

            }
        }

        delete[] buffer;

        imageFile.close();
    }
    else
    {
        cerr << "Could not open the file : " << filePath << endl;
        return false;
    }

    return true;
}
