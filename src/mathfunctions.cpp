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
 * \file mathfunctions.cpp
 * \brief Implementation of mathematical functions.
 * \author Antoine Toisoul Le Cann
 * \date September, 1st, 2016
 *
 * Implementation of a several mmathematical functions.
 */

#include "mathfunctions.h"

using namespace std;
using namespace cv;

/**
 * Clamp a number so it is in the range [inf ; sup].
 * If number<inf, then the function returns inf.
 * If number>sup, then the function returns sup.
 * If number is in the range [inf ; sup] then the function returns the number.
 * @param INPUT : value is a float which will be clamped.
 * @param INPUT : inf is the lower bound of the range.
 * @param INPUT : sup is the upper bound of the range.
 * @return The clamped value in the range [inf ; sup]. If sup < inf then the function returns the number.
 */
float clamp(float value, float inf, float sup)
{
    float result = 0.0f;

    if(inf<=sup)
    {
        if(value < inf)
            result = inf;
        else if(value > sup)
            result = sup;
        else
            result = value;
    }
    else
    {
        cerr << "sup must be greater than inf !" << endl;
        return value;
    }

    return result;
}

/**
 * Normalizes a 3x1 vector stored in an OpenCV Mat.
 * @brief normalizeVector
 * @param vector
 */
void normalizeVector(Mat &vector)
{
    float x = vector.at<float>(0,0);
    float y = vector.at<float>(1,0);
    float z = vector.at<float>(2,0);
    float norm = sqrt(x*x+y*y+z*z);

    vector.at<float>(0,0) = x/norm;
    vector.at<float>(1,0) = y/norm;
    vector.at<float>(2,0) = z/norm;

}

/**
 * Given a rotation axis and an angle defined by its cosine and sin, returns the corresponding rotation matrix.
 * @brief makeRotationMatrix
 * @param axis
 * @param sin
 * @param cos
 * @return
 */
Mat makeRotationMatrix(Point3f axis, float sin, float cos)
{
    Mat rotationMatrix = Mat::zeros(3,3, CV_32FC1);

    //Q is the skew symmetric matrix
    Mat Q = Mat::zeros(3,3, CV_32FC1);

    Q.at<float>(0,1) = -axis.z;
    Q.at<float>(0,2) = axis.y;

    Q.at<float>(1,0) = axis.z;
    Q.at<float>(1,2) = -axis.x;

    Q.at<float>(2,0) = axis.y;
    Q.at<float>(2,1) = -axis.x;

    //Calculates the rotation matrix
    rotationMatrix += Mat::eye(3,3, CV_32FC1);
    rotationMatrix += sin*Q;
    rotationMatrix += (1.0-cos)*Q*Q;

    return rotationMatrix;
}
