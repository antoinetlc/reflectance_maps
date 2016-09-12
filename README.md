# reflectance_maps
This project is an open-source implementation of the papers : 

* Rapid Acquisition of Specular and Diffuse Normal Maps from Polarized Spherical Gradient Illumination. [Ma et al. 2007]
* Estimating Specular Roughness and Anisotropy from Second Order Spherical Gradient Illumination. [Ghosh et al. 2009]

It is distributed under the LGPL license.

#### Version

Version 1.0

## Principle

This program can compute reflectance maps from pictures taken with polarised gradient illumination. These reflectance maps include spatially varying diffuse and specular albedo, normal map and roughness map.

**Important note** : In this implementation the sample is supposed to be almost flat and illuminated by a flat light source (e.g LCD screen) and not a light stage.

During the measurements the gradients illumination used were : order 0 (full white illumination), +X gradient, -X gradient, +Y gradient, -Y gradient, second order X gradient and Y second order gradient. The +Z and -Z gradients were omited due to the planar assumption of the surface (see note). The light source was also linearly polarised (LCD screen) and a polariser was added on the camera to allow the diffuse/specular separation. Please refer to the papers for more details.

## Compilation
This program has been compiled and tested on a Windows environment.
It requires the following libraries/API in order to compile :

* OpenCV (tested with version 2.4.11)

A "reflectance_maps.pro" file is provided for compilation with QtCreator IDE. Please update the libraries paths to match your installation.

## Installation
After the measurements, create a directory with the following folders and files : 
* par/
* cross/
* texture/
* checker.txt
* mask.jpg

The "par" and "cross" folders contain the measurements with parallel polarised illumination and cross polarised illumination respectively. Each folder must contain the measurements with the 7 gradients in the following order : 

* order 0 gradient
* order 1 +x gradient
* order 1 -x gradient
* order 1 +y gradient
* order 1 -y gradient
* order 2 x gradient
* order 2 y gradient

Each folder must also contain an ambient.jpg file that corresponds to the sample taken without the LCD screen switched on (recording of the ambient illumination of the room).

The results will be saved in the "texture" folder. 

Finally two files have to be added. 

* A mask.jpg file i.e a mask that describes the area of the image where the sample is. This is important to avoid computations on the background of the image. The region of the sample must have a red color RGB = (255, 0 , 0).
* A checker.txt file for the color calibration. 

A picture of a ColorChecker must be taken during the measurements for correct color calibration. These values are stored in the checker.txt file. It must have two lines. The first line corresponds to the RGB value of a given reflective patch on the ColorChecker for the measurement with parallel polarisation. The second line one to the values for measurements with cross polarisation.

```
R_par G_par B_par PatchReflectance
R_cross G_cross B_cross PatchReflectance
```

Call the compute_maps function with its first parameter set to the path of this directory to start the computation (see main.cpp).

## License

Reflectance Maps. Author :  Antoine TOISOUL LE CANN. Copyright © 2016 Antoine TOISOUL LE CANN, Imperial College London. All rights reserved.

Reflectance Maps is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. PFM_ReadWrite is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details. You should have received a copy of the GNU Lesser General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.
