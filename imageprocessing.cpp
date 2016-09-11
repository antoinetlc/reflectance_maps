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
 * \file imageprocessing.h
 * \brief Implementation of functions related to image processing.
 * \author Antoine Toisoul Le Cann
 * \date September, 11th, 2016
 *
 * Implementation of functions related to image processing.
 */

#include "imageprocessing.h"

using namespace cv;
using namespace std;

/**
 * Function that scales a float image to the 0;1 range.
 * Divides each color channel by the maximum. The maximum is calculated in the region of the image defined by the mask.
 * @brief scaleTo01Range
 * @param image
 */
void scaleTo01Range(Mat &image, Mat &maskObject)
{
    int width = image.cols;
    int height = image.rows;
    float R = 0.0, G = 0.0, B = 0.0;
    float maximumOfRGB = 0.0;
    float currentMaximumRGB = 0.0;

    //Find maximum
    for(int i = 0 ; i<height ; i++)
    {
        for(int j = 0 ; j<width ; j++)
        {
            //Only calculate the maximum inside the mask
            if(maskObject.at<Vec3f>(i,j).val[2]>0.9)
            {
                R = image.at<Vec3f>(i,j).val[2];
                G = image.at<Vec3f>(i,j).val[1];
                B = image.at<Vec3f>(i,j).val[0];

                currentMaximumRGB = max(R,max(G,B));

                if(currentMaximumRGB>maximumOfRGB)
                {
                    maximumOfRGB = currentMaximumRGB;
                }
            }

        }
    }

    //Divide RGB by the same value to avoid color shifting
    for(int i = 0 ; i<height ; i++)
    {
        for(int j = 0 ; j<width ; j++)
        {
            if(abs(maximumOfRGB>0.0))
            {
                image.at<Vec3f>(i,j).val[2] /= maximumOfRGB;
                image.at<Vec3f>(i,j).val[1] /= maximumOfRGB;
                image.at<Vec3f>(i,j).val[0] /= maximumOfRGB;
            }
        }
    }
}

/**
 * For each pixel of each image sets RGB to 0 if any of R, G, B is 0.
 * @brief setNegativePixelsTo0
 * @param images
 * @param numberOfImages
 */
void setNegativePixelsTo0(Mat images[], int numberOfImages)
{
    for(int k = 0 ; k<numberOfImages ; k++)
    {
        setNegativePixelsTo0(images[k]);
    }
}

/**
 * For each pixel of the image sets RGB to 0 if any of R, G, B is 0
 * @brief setNegativePixelsTo0
 * @param images
 */
void setNegativePixelsTo0(Mat &image)
{
    int width = image.cols;
    int height = image.rows;

    float R = 0.0, G = 0.0, B = 0.0;

    for(int i = 0 ; i<height ; i++)
    {
        for(int j = 0 ; j<width ; j++)
        {
            R = image.at<Vec3f>(i,j).val[2];
            G = image.at<Vec3f>(i,j).val[1];
            B = image.at<Vec3f>(i,j).val[0];

            if(R<0.0 || G<0.0 || B<0.0)
            {
                 image.at<Vec3f>(i,j).val[2] = 0.0;
                 image.at<Vec3f>(i,j).val[1] = 0.0;
                 image.at<Vec3f>(i,j).val[0] = 0.0;
            }
        }
    }
}

/**
 * Apply a gamma correction to a RGB image (OpenCV Mat image).
 * @param INPUT : rgbImage is the image to which the gamma correction is applied.
 * @param OUTPUT : rgbImageWithGamma is the rgbImage with the gamma applied.
 * @param INPUT : gamma is a double corresponding to the value of the gamma correction.
 */
void gammaCorrection(const Mat &rgbImage, Mat &rgbImageWithGamma, double gamma)
{
    Mat channel[3],channel32F[3], channelWithGamma[3];

    split(rgbImage, channel);

    channel[0].convertTo(channel32F[0], CV_32F);
    channel[1].convertTo(channel32F[1], CV_32F);
    channel[2].convertTo(channel32F[2], CV_32F);

    pow(channel32F[0], 1.0/gamma, channelWithGamma[0]);
    pow(channel32F[1], 1.0/gamma, channelWithGamma[1]);
    pow(channel32F[2], 1.0/gamma, channelWithGamma[2]);

    merge(channelWithGamma,3,rgbImageWithGamma);
}

/**
 * Remove the gamma correction of a RGB image (OpenCV Mat image).
 * @param INPUT : rgbImage is the image to which the gamma correction is removed. rgbImage is an OpenCV CV_32FC3 matrix. (matrix of 3 channels of 32 bits floats).
 * @param OUTPUT : rgbImageWithGamma is the rgbImage with the gamma removed. It is a CV_32FC3 matrix (matrix of 3 channels of 32 bits floats).
 * @param INPUT : gamma is a double corresponding to the value of the gamma correction.
 */
void removeGammaCorrection(const Mat &rgbImage, Mat &rgbImageWithoutGamma, double gamma)
{
    Mat channel[3], channel32F[3], channelWithoutGamma[3];

    split(rgbImage, channel);

    pow(channel32F[0], gamma, channelWithoutGamma[0]);
    pow(channel32F[1], gamma, channelWithoutGamma[1]);
    pow(channel32F[2], gamma, channelWithoutGamma[2]);

    merge(channelWithoutGamma,3,rgbImageWithoutGamma);
}
