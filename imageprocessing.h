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
 * \file imageprocessing.cpp
 * \brief Implementation of functions related to image processing.
 * \author Antoine Toisoul Le Cann
 * \date September, 11th, 2016
 *
 * Implementation of functions related to image processing.
 */

#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include "mathfunctions.h"

#define M_PI 3.14159265358979323846

#include <iostream>
#include <cmath>
#include <vector>

#include <QApplication>
#include <QVector3D>
#include <QMatrix3x3>
#include <QGenericMatrix>

#include <opencv2/core/core.hpp>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/features2d/features2d.hpp>

/**
 * Function that scales a float image to the 0;1 range.
 * Divides each color channel by the maximum. The maximum is calculated in the region of the image defined by the mask.
 * @brief scaleTo01Range
 * @param image
 */
void scaleTo01Range(cv::Mat &image, const cv::Mat &maskObject);

/**
 * For each pixel of each image sets RGB to 0 if any of R, G, B is 0
 * @brief setNegativePixelsTo0
 * @param images
 * @param numberOfImages
 */
void setNegativePixelsTo0(cv::Mat images[], int numberOfImages);

/**
 * For each pixel of the image sets RGB to 0 if any of R, G, B is 0
 * @brief setNegativePixelsTo0
 * @param images
 */
void setNegativePixelsTo0(cv::Mat &image);

/**
 * Apply a gamma correction to a RGB image (OpenCV Mat image).
 * @param INPUT : rgbImage is the image to which the gamma correction is applied.
 * @param OUTPUT : rgbImageWithGamma is the rgbImage with the gamma applied.
 * @param INPUT : gamma is a double corresponding to the value of the gamma correction.
 */
void gammaCorrection(const cv::Mat &rgbImage, cv::Mat &rgbImageWithGamma, double gamma);

/**
 * Remove the gamma correction of a RGB image (OpenCV Mat image).
 * @param INPUT : rgbImage is the image to which the gamma correction is removed. rgbImage is an OpenCV CV_32FC3 matrix. (matrix of 3 channels of 32 bits floats).
 * @param OUTPUT : rgbImageWithGamma is the rgbImage with the gamma removed. It is a CV_32FC3 matrix (matrix of 3 channels of 32 bits floats).
 * @param INPUT : gamma is a double corresponding to the value of the gamma correction.
 */
void removeGammaCorrection(const cv::Mat &rgbImage, cv::Mat &rgbImageWithoutGamma, double gamma);

#endif // IMAGEPROCESSING_H

