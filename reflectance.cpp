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
 * \file reflectance.cpp
 * \brief Implementation of functions related to the calculation of reflectance maps.
 * \author Antoine Toisoul Le Cann
 * \date September, 11th, 2016
 *
 * Implementation of functions related to the calculation of reflectance maps.
 */

#include "reflectance.h"

using namespace std;
using namespace cv;

/**
 * Function to compute the reflectance maps given the path to the data folder and a bool that says if the
 * cross polarised data exists.
 * @brief computeMaps
 * @param pathToFolder
 * @param isCrossData
 */
void computeMaps(string pathToFolder, bool isCrossData)
{

    //Load the mask object
    //Mask that represent the area where the calculations are done
    Mat mask = imread(pathToFolder + "/mask.JPG", CV_LOAD_IMAGE_COLOR);

    if(!mask.data)
    {
        cerr << "Could not load image : " << pathToFolder + "/mask.JPG" << endl;
        exit(-1);
    }
    else
    {
        mask.convertTo(mask, CV_32FC3);
        mask /= 255.0;
    }

    Mat parallelData[NUMBER_OF_GRADIENT_ILLUMINATION];
    Mat crossData[NUMBER_OF_GRADIENT_ILLUMINATION];

    ostringstream osstream;
    unsigned int imageNumberPar = 2855;
    unsigned int imageNumberCross = 2869;

    /*--Load images parallelPolarised ---*/
    for(int i = 0 ; i<NUMBER_OF_GRADIENT_ILLUMINATION ; i++)
    {
        int imageNumber = imageNumberPar +i;

        osstream <<  pathToFolder << "/par/IMG_" << imageNumber << ".JPG";
        parallelData[i] = imread(osstream.str(), CV_LOAD_IMAGE_COLOR);

        if(!parallelData[i].data)
        {
            cerr << "Could not load image : " << osstream.str() << endl;
            exit(-1);
        }
        else
        {
            parallelData[i].convertTo(parallelData[i], CV_32FC3);
            parallelData[i] /= 255.0;

            //Remove Gamma
            removeGammaCorrection(parallelData[i], parallelData[i], 2.2);
        }

        osstream.str("");
    }

    /*---Load the ambient illumination---*/
    Mat ambientPar = imread(pathToFolder + "/par/ambient.JPG", CV_LOAD_IMAGE_COLOR);

    if(!ambientPar.data)
    {
        cerr << "Could not load image : " << pathToFolder + "/par/ambient.JPG" << endl;
        exit(-1);
    }
    else
    {
        ambientPar.convertTo(ambientPar, CV_32FC3);
        ambientPar /= 255.0;

        removeAmbientIllumination(parallelData, NUMBER_OF_GRADIENT_ILLUMINATION, ambientPar);
    }

    /*---Load images cross polarised---*/
    if(isCrossData)
    {
        for(int i = 0 ; i<NUMBER_OF_GRADIENT_ILLUMINATION ; i++)
        {
            int imageNumber = imageNumberCross +i;

            osstream << pathToFolder << "/cross/IMG_" << imageNumber << ".JPG";
            crossData[i] = imread(osstream.str(), CV_LOAD_IMAGE_COLOR);

            if(!crossData[i].data)
            {
                cerr << "Could not load image : " << osstream.str() << endl;
                exit(-1);
            }
            else
            {
                crossData[i].convertTo(crossData[i], CV_32FC3);
                crossData[i] /= 255.0;

                //Remove Gamma correction
                removeGammaCorrection(crossData[i], crossData[i], 2.2);
            }


            osstream.str("");
        }


        /*---Remove the ambient illumination---*/
        Mat ambientCross = imread(pathToFolder + "/cross/ambient.JPG", CV_LOAD_IMAGE_COLOR);

        if(!ambientCross.data)
        {
            cerr << "Could not load image : " << pathToFolder + "/par/ambient.JPG" << endl;
            exit(-1);
        }
        else
        {
            ambientCross.convertTo(ambientCross, CV_32FC3);
            ambientCross /= 255.0;

            removeAmbientIllumination(crossData, NUMBER_OF_GRADIENT_ILLUMINATION, ambientCross);
        }

        //Computations : diffuse, specular normals and roughness
        /*--Scale values with checkerchart ---*/
        checkerchartScaling(parallelData, crossData, mask, pathToFolder);

        diffuseSpecularSeparation(parallelData, crossData, mask, pathToFolder);

        computeNormals(parallelData, mask, pathToFolder);

        computeRoughness(parallelData, pathToFolder);

    }
    else
    {

        //Computations : specular normals and roughness
        /*--Scale values with checkerchart ---*/
        checkerchartScaling(parallelData,mask, pathToFolder);

        /*-Specular data-*/
        Mat specular = parallelData[0].clone();

        //Scale down to 01 range and save the result
        scaleTo01Range(specular, mask);

        savePFM(specular, pathToFolder + "/textures/specular.pfm");

        computeNormals(parallelData, mask, pathToFolder);

        computeRoughness(parallelData, pathToFolder);

    }
}

/**
 * Scale the value of parallel polarised data to the value of the checkerchart.
 * @brief checkerchartScaling
 * @param parallelData
 * @param mask
 * @param pathToFolder
 */
void checkerchartScaling(Mat parallelData[], const Mat &mask, string pathToFolder)
{
    /*--Read checkerchart values---*/
    string RParPicture, GParPicture, BParPicture;
    string checkerchartPar;
    float ratioParR = 0.0, ratioParG = 0.0, ratioParB = 0.0;

    ifstream checkerFile(pathToFolder + "/checker.txt", ios::in);

    if(checkerFile)
    {
        //First line is parallel and second is cross polarised
        checkerFile >> RParPicture >> GParPicture >> BParPicture >> checkerchartPar;

        ratioParR = atof(checkerchartPar.c_str())/atof(RParPicture.c_str());
        ratioParG = atof(checkerchartPar.c_str())/atof(GParPicture.c_str());
        ratioParB = atof(checkerchartPar.c_str())/atof(BParPicture.c_str());

        cout << "Parallel " << ratioParR << " - " << ratioParG << " - "<< ratioParB << endl;

        int width = parallelData[0].cols;
        int height = parallelData[0].rows;

        /*--Scale parallel polarised values---*/
        //White balancing with the checkerchart
        for(int k = 0 ; k<NUMBER_OF_GRADIENT_ILLUMINATION ; k++)
        {
            for(int i = 0 ; i<height ; i++)
            {
                for(int j = 0 ; j<width ; j++)
                {
                    //OpenCV is in BGR
                    parallelData[k].at<Vec3f>(i,j).val[0] *= ratioParB;
                    parallelData[k].at<Vec3f>(i,j).val[1] *= ratioParG;
                    parallelData[k].at<Vec3f>(i,j).val[2] *= ratioParR;
                }
            }
        }

        //Scale down between 0 and 1 for the computation
        for(int k = 0 ; k<NUMBER_OF_GRADIENT_ILLUMINATION ; k++)
        {
            //Scale down to 01 range and save the result
            scaleTo01Range(parallelData[k], mask);
        }
    }
    else
    {
        cerr << "Could not the file image : " << pathToFolder << "/checker.txt" << endl;
        exit(-1);
    }

}

/**
 * Scale the value of parallel and cross polarised data to the value of the checkerchart.
 * @brief checkerchartScaling
 * @param parallelData
 * @param crossData
 * @param mask
 * @param pathToFolder
 */
void checkerchartScaling(Mat parallelData[], Mat crossData[], const Mat &mask, string pathToFolder)
{
    /*--Read checkerchart values---*/
    string RParPicture, GParPicture, BParPicture;
    string checkerchartPar;
    float ratioParR = 0.0, ratioParG = 0.0, ratioParB = 0.0;

    string RCrossPicture, GCrossPicture, BCrossPicture;
    string checkerchartCross;
    float ratioCrossR = 0.0, ratioCrossG = 0.0, ratioCrossB = 0.0;

    ifstream checkerFile(pathToFolder + "/checker.txt", ios::in);

    //If the file has been correctly opened
    if(checkerFile)
    {
        //First line is parallel and second is cross polarised
        checkerFile >> RParPicture >> GParPicture >> BParPicture >> checkerchartPar;

        ratioParR = atof(checkerchartPar.c_str())/atof(RParPicture.c_str());
        ratioParG = atof(checkerchartPar.c_str())/atof(GParPicture.c_str());
        ratioParB = atof(checkerchartPar.c_str())/atof(BParPicture.c_str());

        checkerFile >> RCrossPicture >> GCrossPicture >> BCrossPicture >> checkerchartCross;

        ratioCrossR = atof(checkerchartCross.c_str())/atof(RCrossPicture.c_str());
        ratioCrossG = atof(checkerchartCross.c_str())/atof(GCrossPicture.c_str());
        ratioCrossB = atof(checkerchartCross.c_str())/atof(BCrossPicture.c_str());

        cout << "Parallel " << ratioParR << " - " << ratioParG << " - "<< ratioParB << endl;
        cout << "Cross " << ratioCrossR << " - " << ratioCrossG << " - "<< ratioCrossB << endl;

        int width = parallelData[0].cols;
        int height = parallelData[0].rows;

        /*--Scale parallel polarised values---*/
        //White balancing with the checkerchart
        for(int k = 0 ; k<NUMBER_OF_GRADIENT_ILLUMINATION ; k++)
        {

            for(int i = 0 ; i<height ; i++)
            {

                for(int j = 0 ; j<width ; j++)
                {

                    //OpenCV is in BGR
                    parallelData[k].at<Vec3f>(i,j).val[0] *= ratioParB;
                    parallelData[k].at<Vec3f>(i,j).val[1] *= ratioParG;
                    parallelData[k].at<Vec3f>(i,j).val[2] *= ratioParR;

                    crossData[k].at<Vec3f>(i,j).val[0] *= ratioCrossB;
                    crossData[k].at<Vec3f>(i,j).val[1] *= ratioCrossG;
                    crossData[k].at<Vec3f>(i,j).val[2] *= ratioCrossR;
                }
            }
         }


        //After applying the checkerchart some pixels values might be above 1
        //Scale down between 0 and 1 for the computation
        for(int k = 0 ; k<NUMBER_OF_GRADIENT_ILLUMINATION ; k++)
        {
            //Scale down to 01 range and save the result
            scaleTo01Range(crossData[k], mask);
            scaleTo01Range(parallelData[k], mask);
        }

    }
    else
    {
        cerr << "Could not the file image : " << pathToFolder << "/checker.txt" << endl;
        exit(-1);
    }

}

/**
 * Compute the diffuse and specular albedo given parallel and cross polarised data.
 * Also requires a mask on which data is computed.
 * @brief checkerchartScaling
 * @param parallelData
 * @param mask
 * @param pathToFolder
 */
void diffuseSpecularSeparation(Mat parallelData[], Mat crossData[], const Mat &mask, string pathToFolder)
{
    //Cross data contains diffuse only
    //Parallel data contains diffuse+specular
    Mat diffuse = crossData[0].clone();
    Mat specular = parallelData[0].clone();

    //Remove the diffuse component
    specular -= diffuse;

    setNegativePixelsTo0(specular);

    //Scale down to 01 range and save the result
    scaleTo01Range(diffuse, mask);
    scaleTo01Range(specular, mask);

    savePFM(diffuse, pathToFolder + "/textures/diffuse.pfm");
    savePFM(specular, pathToFolder + "/textures/specular.pfm");
}

/**
 * Compute the specular normals given parallel and cross data.
 * Also requires a mask on which data is computed.
 * The sample is assumed to be almost flat without big variations in the z component of the normal.
 * Therefore the measurements have been made without the zGradients.
 * @param parallelData
 * @param crossData
 * @param mask
 * @param pathToFolder
 */
void computeNormals(Mat parallelData[], Mat crossData[], const Mat &mask, string pathToFolder)
{
    Mat xGradient, minusXGradient;
    Mat yGradient, minusYGradient;;
    Mat normals = Mat(parallelData[1].rows, parallelData[1].cols, CV_32FC3);

    //The specular normals are computed : remove the cross data from the parallel data.
    //X direction
    xGradient = parallelData[1].clone()-crossData[1].clone();
    minusXGradient = parallelData[2].clone()-crossData[2].clone();

    //The specular normals are computed : remove the cross data from the parallel data.
    //Y direction
    yGradient = parallelData[3].clone()-crossData[3].clone();
    minusYGradient = parallelData[4].clone()-crossData[4].clone();

    float x = 0.0, y = 0.0, z = 0.0;
    float norm = 0.0;

    int width = parallelData[0].cols;
    int height = parallelData[0].rows;

    //Compute all the normals and normalize them
    for(int i = 0 ; i<height ; i++)
    {
        for(int j = 0 ; j<width ; j++)
        {
            //RGB = xyz
            //Calculations with green channel
            //Reflection : the camera sees xGradient as -xGradient and conversely

            //x component of the normals : -xGradient - +xGradient
            //y component of the normals : yGradient - -yGradient
            x = minusXGradient.at<Vec3f>(i,j).val[1]-xGradient.at<Vec3f>(i,j).val[1];
            y = yGradient.at<Vec3f>(i,j).val[1]-minusYGradient.at<Vec3f>(i,j).val[1];
            z = sqrt(1.0-x*x-y*y);
            norm = sqrt(x*x+y*y+z*z);

            //Normalise the reflection vector
            x /= norm;
            y /= norm;
            z /= norm;

            //(x,y,z) is the reflection vector
            //The normal is the hal vector V+R. V = (0,0,1)
            z += 1.0;

            //Normalise the normal
            norm = sqrt(x*x+y*y+z*z);

            x /= norm;
            y /= norm;
            z /= norm;

            normals.at<Vec3f>(i,j).val[2] = x;
            normals.at<Vec3f>(i,j).val[1] = y;
            normals.at<Vec3f>(i,j).val[0] = z;

        }
    }

    alignAverageSurfaceNormal(normals, mask);

    //Color mapping
    for(int i = 0 ; i<height ; i++)
    {

        for(int j = 0 ; j<width ; j++)
        {
            normals.at<Vec3f>(i,j).val[2] = (normals.at<Vec3f>(i,j).val[2]+1.0)/2.0;
            normals.at<Vec3f>(i,j).val[1] = (normals.at<Vec3f>(i,j).val[1]+1.0)/2.0;
            normals.at<Vec3f>(i,j).val[0] = (normals.at<Vec3f>(i,j).val[0]+1.0)/2.0;
        }
    }


    normals *= 255.0;
    normals.convertTo(normals, CV_8UC3);
    imwrite(pathToFolder + "/textures/normalMap.bmp", normals);
}

/**
 * Compute the specular normals given parallel and cross data.
 * Also requires a mask on which data is computed.
 * @brief computeNormals
 * @param parallelData
 * @param crossData
 * @param mask
 * @param pathToFolder
 */
void computeNormals(Mat parallelData[], const Mat &mask, string pathToFolder)
{
    Mat xGradient, minusXGradient;
    Mat yGradient, minusYGradient;;
    Mat normals = Mat(parallelData[1].rows, parallelData[1].cols, CV_32FC3);

    xGradient = parallelData[1].clone();
    minusXGradient = parallelData[2].clone();

    yGradient = parallelData[3].clone();
    minusYGradient = parallelData[4].clone();

    float x = 0.0, y = 0.0, z = 0.0;
    float norm = 0.0;

    int width = parallelData[0].cols;
    int height = parallelData[0].rows;

    for(int i = 0 ; i<height ; i++)
    {

        for(int j = 0 ; j<width ; j++)
        {

            //RGB = xyz
            //Calculations with green channel
            //Reflection : the camera sees xGradient as -xGradient and conversely
            x = minusXGradient.at<Vec3f>(i,j).val[1]-xGradient.at<Vec3f>(i,j).val[1];
            y = yGradient.at<Vec3f>(i,j).val[1]-minusYGradient.at<Vec3f>(i,j).val[1];
            z = sqrt(1.0-x*x-y*y);
            norm = sqrt(x*x+y*y+z*z);

            //Normalise the reflection vector
            x /= norm;
            y /= norm;
            z /= norm;

            //(x,y,z) is the reflection vector
            //The normal is the hal vector V+R. V = (0,0,1)
            z += 1.0;

            //Normalise the normal
            norm = sqrt(x*x+y*y+z*z);

            x /= norm;
            y /= norm;
            z /= norm;

            normals.at<Vec3f>(i,j).val[2] = x;
            normals.at<Vec3f>(i,j).val[1] = y;
            normals.at<Vec3f>(i,j).val[0] = z;

        }
    }

    //Align the average surface normal with (0,0,1) (flat sample assumption)
    alignAverageSurfaceNormal(normals, mask);

    //Color mapping RGB = XYZ
    for(int i = 0 ; i<height ; i++)
    {
        for(int j = 0 ; j<width ; j++)
        {
            normals.at<Vec3f>(i,j).val[2] = (normals.at<Vec3f>(i,j).val[2]+1.0)/2.0;
            normals.at<Vec3f>(i,j).val[1] = (normals.at<Vec3f>(i,j).val[1]+1.0)/2.0;
            normals.at<Vec3f>(i,j).val[0] = (normals.at<Vec3f>(i,j).val[0]+1.0)/2.0;
        }
    }


    normals *= 255.0;
    normals.convertTo(normals, CV_8UC3);
    //Save as BMP : no gamma!
    imwrite(pathToFolder + "/textures/normalMap.bmp", normals);
}

/**
 * Remove ambient illumination from a set of images.
 * @brief removeAmbientIllumination
 * @param images
 * @param numberOfImages
 * @param ambient
 */
void removeAmbientIllumination(Mat images[], int numberOfImages, const Mat &ambient)
{
    for(int k = 0 ; k<numberOfImages ; k++)
    {
        images[k] -= ambient;
    }

    setNegativePixelsTo0(images, numberOfImages);
}

/**
 * Rotates the normals so that the average surface normal is (0,0,1)
 * @brief alignAverageSurfaceNormal
 * @param normals
 * @param mask
 */
void alignAverageSurfaceNormal(Mat &normals, const Mat &mask)
{
    int height = normals.rows;
    int width = normals.cols;


    /*----Compute average surface normal---*/
    //On the sample only !
    Mat averageNormal = Mat::zeros(3,1, CV_32FC1);
    double numberOfNormalsAccounted = 0.0;
    for(int i = 0 ; i<height ; i++)
    {
        for(int j = 0 ; j<width ; j++)
        {
            //Calculate the average surface normal on the mask only
            if(mask.at<Vec3f>(i,j).val[2] >0.9)
            {
                if(isnan(normals.at<Vec3f>(i,j).val[2]) || isnan(normals.at<Vec3f>(i,j).val[1]) || isnan(normals.at<Vec3f>(i,j).val[0]))
                {
                    cout << "Nan Skip : " << i << " " << j << endl;
                }
                else
                {
                    averageNormal.at<float>(0,0) += normals.at<Vec3f>(i,j).val[2];
                    averageNormal.at<float>(1,0) += normals.at<Vec3f>(i,j).val[1];
                    averageNormal.at<float>(2,0) += normals.at<Vec3f>(i,j).val[0];
                    numberOfNormalsAccounted += 1.0;
                }
            }
        }
    }


    averageNormal /= numberOfNormalsAccounted;
    normalizeVector(averageNormal);

    cout << "Average surface normal : " << averageNormal << endl;

    /*----Compute the rotation matrix---*/
    //Align all the normals with normal with (0,0,1)
    Point3f zVector = Point3f(0.0, 0.0, 1.0);
    Point3f averageNormal3f = Point3f(averageNormal.at<float>(0,0), averageNormal.at<float>(1,0), averageNormal.at<float>(2,0));

    //Point3f axis = zVector.cross(averageNormal3f);
    Point3f axis = averageNormal3f.cross(zVector);

    float cos = zVector.dot(averageNormal3f);
    float sin = sqrt(axis.x*axis.x+axis.y*axis.y+axis.z*axis.z);

    Mat rotationMatrix = makeRotationMatrix(axis, sin, cos);

    /*----Align all the normals---*/
    for(int i = 0 ; i<height ; i++)
    {
        for(int j = 0 ; j<width ; j++)
        {
            Mat currentNormal = Mat::zeros(3,1, CV_32FC1);

            currentNormal.at<float>(0,0) =  normals.at<Vec3f>(i,j).val[2];
            currentNormal.at<float>(1,0) =  normals.at<Vec3f>(i,j).val[1];
            currentNormal.at<float>(2,0) =  normals.at<Vec3f>(i,j).val[0];

            currentNormal = rotationMatrix*currentNormal;

            normals.at<Vec3f>(i,j).val[2] = currentNormal.at<float>(0,0) ;
            normals.at<Vec3f>(i,j).val[1] = currentNormal.at<float>(1,0) ;
            normals.at<Vec3f>(i,j).val[0] = currentNormal.at<float>(2,0) ;
        }
    }
}

/**
 * Calculates the roughness using both cross and parallel polarised data.
 * @brief computeRoughness
 * @param parallelData
 * @param crossData
 * @param pathToFolder
 */
void computeRoughness(Mat parallelData[], Mat crossData[], string pathToFolder)
{
    Mat xGradient, minusXGradient;
    Mat yGradient, minusYGradient;;

    //Remove the cross data from the parallel data for the L1 moments
    xGradient = parallelData[1].clone()-crossData[1].clone();
    minusXGradient = parallelData[2].clone()-crossData[2].clone();

    yGradient = parallelData[3].clone()-crossData[3].clone();
    minusYGradient = parallelData[4].clone()-crossData[4].clone();

    Mat horizontalGradient = minusXGradient-xGradient;
    Mat verticalGradient = yGradient-minusYGradient;

    //The general formula for the roughness is sigma^2 = L1^2/L0-L2/L0 with L1 and L2 the first and second order moments.
    //Compute it in the x direction and the y direction
    divide(horizontalGradient, parallelData[0]-crossData[0], horizontalGradient);
    divide(verticalGradient, parallelData[0]-crossData[0], verticalGradient);

    Mat secondOrderGradientX = parallelData[5];
    Mat secondOrderGradientY = parallelData[6];

    divide(secondOrderGradientX, parallelData[0]-crossData[0], secondOrderGradientX);
    divide(secondOrderGradientY, parallelData[0]-crossData[0], secondOrderGradientY);

    secondOrderGradientX -= horizontalGradient.mul(horizontalGradient);
    secondOrderGradientY -= verticalGradient.mul(verticalGradient);

    //Calculate the final roughness is sigma^2 = sqrt(sigma_x^4+sigma_y^4)/4
    Mat roughness = Mat::zeros(secondOrderGradientX.rows, secondOrderGradientX.cols, CV_32FC3);

    int height = parallelData[0].rows;
    int width = parallelData[0].cols;

    for(int i = 0 ; i<height ; i++)
    {
        for(int j = 0 ; j<width ; j++)
        {
            roughness.at<Vec3f>(i,j).val[0] = sqrt(secondOrderGradientX.at<Vec3f>(i,j).val[1]*secondOrderGradientX.at<Vec3f>(i,j).val[1]
                    +secondOrderGradientY.at<Vec3f>(i,j).val[1]*secondOrderGradientY.at<Vec3f>(i,j).val[1]);

            roughness.at<Vec3f>(i,j).val[1] = sqrt(secondOrderGradientX.at<Vec3f>(i,j).val[1]*secondOrderGradientX.at<Vec3f>(i,j).val[1]
                    +secondOrderGradientY.at<Vec3f>(i,j).val[1]*secondOrderGradientY.at<Vec3f>(i,j).val[1]);

            roughness.at<Vec3f>(i,j).val[2] = sqrt(secondOrderGradientX.at<Vec3f>(i,j).val[1]*secondOrderGradientX.at<Vec3f>(i,j).val[1]
                    +secondOrderGradientY.at<Vec3f>(i,j).val[1]*secondOrderGradientY.at<Vec3f>(i,j).val[1]);
        }
    }

    roughness /= 4.0;

    savePFM(roughness, pathToFolder + "/textures/roughness.pfm");

}

/**
 * Calculates the roughness using only parallel polarised data.
 * @brief computeRoughness
 * @param parallelData
 * @param pathToFolder
 */
void computeRoughness(Mat parallelData[], string pathToFolder)
{
    Mat xGradient, minusXGradient;
    Mat yGradient, minusYGradient;;

    xGradient = parallelData[1].clone();
    minusXGradient = parallelData[2].clone();

    yGradient = parallelData[3].clone();
    minusYGradient = parallelData[4].clone();

    //The general formula for the roughness is sigma^2 = L1^2/L0-L2/L0 with L1 and L2 the first and second order moments.
    //Compute it in the x direction and the y direction
    Mat horizontalGradient = minusXGradient-xGradient;
    Mat verticalGradient = yGradient-minusYGradient;

    divide(horizontalGradient, parallelData[0], horizontalGradient);
    divide(verticalGradient, parallelData[0], verticalGradient);

    Mat secondOrderGradientX = parallelData[5];
    Mat secondOrderGradientY = parallelData[6];

    divide(secondOrderGradientX, parallelData[0], secondOrderGradientX);
    divide(secondOrderGradientY, parallelData[0], secondOrderGradientY);

    secondOrderGradientX -= horizontalGradient.mul(horizontalGradient);
    secondOrderGradientY -= verticalGradient.mul(verticalGradient);

    //Calculate the final roughness is sigma^2 = sqrt(sigma_x^4+sigma_y^4)/4
    Mat roughness = Mat::zeros(secondOrderGradientX.rows, secondOrderGradientX.cols, CV_32FC3);

    int height = parallelData[0].rows;
    int width = parallelData[0].cols;

    for(int i = 0 ; i<height ; i++)
    {
        for(int j = 0 ; j<width ; j++)
        {
            //sigma^4 = sqrt(sigmaSquaredX^2+sigmaSquaredY^2)
            roughness.at<Vec3f>(i,j).val[0] = sqrt(sqrt(secondOrderGradientX.at<Vec3f>(i,j).val[1]*secondOrderGradientX.at<Vec3f>(i,j).val[1]
                    +secondOrderGradientY.at<Vec3f>(i,j).val[1]*secondOrderGradientY.at<Vec3f>(i,j).val[1]));

            roughness.at<Vec3f>(i,j).val[1] = sqrt(sqrt(secondOrderGradientX.at<Vec3f>(i,j).val[1]*secondOrderGradientX.at<Vec3f>(i,j).val[1]
                    +secondOrderGradientY.at<Vec3f>(i,j).val[1]*secondOrderGradientY.at<Vec3f>(i,j).val[1]));

            roughness.at<Vec3f>(i,j).val[2] = sqrt(sqrt(secondOrderGradientX.at<Vec3f>(i,j).val[1]*secondOrderGradientX.at<Vec3f>(i,j).val[1]
                    +secondOrderGradientY.at<Vec3f>(i,j).val[1]*secondOrderGradientY.at<Vec3f>(i,j).val[1]));
          }
    }

    roughness /= 4.0;
    savePFM(roughness, pathToFolder + "/textures/roughness.pfm");
}
