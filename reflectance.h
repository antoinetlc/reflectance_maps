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
 * \file reflectance.h
 * \brief Implementation of functions related to the calculation of reflectance maps.
 * \author Antoine Toisoul Le Cann
 * \date September, 11th, 2016
 *
 * Implementation of functions related to the calculation of reflectance maps.
 */

#ifndef REFLECTANCE
#define REFLECTANCE

#define M_PI 3.14159265358979323846
#define NUMBER_OF_GRADIENT_ILLUMINATION 7

#include "imageprocessing.h"
#include "mathfunctions.h"
#include "PFMReadWrite.h"

/*---- OpenCV ----*/
#include <opencv2/core/core.hpp>

/*---- Standard library ----*/
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <fstream>

/*---- Qt ----*/
#include <QObject>

#include "mathfunctions.h"
#include "imageprocessing.h"

/**
 * Function to compute the reflectance maps given the name on an object and a bool that says if the
 * cross polarised data exists.
 * @brief computeMaps
 * @param object
 * @param isCrossData
 */
void computeMaps(std::string object, bool isCrossData);

/**
 * Scale the value of parallel and cross polarised data to the value of the checkerchart.
 * @brief checkerchartScaling
 * @param parallelData
 * @param crossData
 * @param object
 */
void checkerchartScaling(cv::Mat parallelData[], cv::Mat crossData[], std::string object);

/**
 * Scale the value of parallel polarised data to the value of the checkerchart.
 * @brief checkerchartScaling
 * @param parallelData
 * @param object
 */
void checkerchartScaling(cv::Mat parallelData[], std::string object);

/**
 * Compute the diffuse and specular albedo given parallel and cross polarised data.
 * Also requires a mask on which data is computed.
 * @brief checkerchartScaling
 * @param parallelData
 * @param object
 */
void diffuseSpecularSeparation(cv::Mat parallelData[], cv::Mat crossData[], std::string object);

/**
 * Compute the specular normals given parallel and cross data.
 * Also requires a mask on which data is computed.
 * @brief computeNormals
 * @param parallelData
 * @param crossData
 * @param object
 */
void computeNormals(cv::Mat parallelData[], cv::Mat crossData[], std::string object);


/**
 * Compute the specular normals given parallel data.
 * Also requires a mask on which data is computed.
 * @brief computeNormals
 * @param parallelData
 * @param crossData
 * @param object
 */
void computeNormals(cv::Mat parallelData[], std::string object);

/**
 * Remove ambient illumination from a set of images.
 * @brief removeAmbientIllumination
 * @param images
 * @param numberOfImages
 * @param ambient
 */
void removeAmbientIllumination(cv::Mat images[], int numberOfImages, const cv::Mat &ambient);

/**
 * Rotates the normals so that the average surface normal is (0,0,1)
 * @brief alignAverageSurfaceNormal
 * @param normals
 * @param object
 */
void alignAverageSurfaceNormal(cv::Mat &normals, std::string object);

/**
 * Calculates the roughness using both cross and parallel polarised data.
 * @brief computeRoughness
 * @param parallelData
 * @param crossData
 * @param object
 */
void computeRoughness(cv::Mat parallelData[], cv::Mat crossData[], std::string object);

/**
 * Calculates the roughness using only parallel polarised data.
 * @brief computeRoughness
 * @param parallelData
 * @param object
 */
void computeRoughness(cv::Mat parallelData[], std::string object);

#endif // REFLECTANCE

