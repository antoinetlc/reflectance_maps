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
 * \file main.cpp
 * \brief Example of how to compute the reflectance maps.
 * \author Antoine Toisoul Le Cann
 * \date September, 11th, 2016
 *
 * Example of how to compute the reflectance maps.
 */

#include <iostream>
#include <string>

#include "reflectance.h"

using namespace std;

int main(void)
{
    //Call this function to compute the maps
    //The first parameter is a path to the folder that contains the illumination measurements : par, cross, checkert.txt, mask.jpg
    //The second parameter is set to true to use the cross polarised measurements or false otherwise.
    //NOTE : the loading of the image file is currently hardcoded.
    //The images are supposed to have a name : IMG_XXXX where XXXX is a number
    //Within a folder (e.g parallel data) the pictures are supposed to have consecutive numbers.
    computeMaps(string("path_to_folder"), true);

    return 0;
}
