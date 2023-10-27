#include <stdio.h>
#include <stdlib.h>
#ifndef PIXELPROCESSOR_H
#define PIXELPROCESSOR_H

struct Pixel 
{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
};

/**
 * Shift color of Pixel array. The dimension of the array is width * height. The shift value of RGB is 
 * rShift, gShift，bShift. Useful for color shift.
 *
 * @param  pArr: Pixel array of the image that this header is for
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 * @param  rShift: the shift value of color r shift
 * @param  gShift: the shift value of color g shift 
 * @param  bShift: the shift value of color b shift 
 */
void colorShiftPixels(struct Pixel** pArr, int width, int height, int rShift, int gShift, int bShift);

/**
 * Apply a cheese filter to the Pixel array. The dimension of the array is width * height. 
 * Useful for applying a cheese filter effect to the image.
 *
 * @param  pArr: Pixel array of the image that this header is for
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 */
void cheeseFilter(struct Pixel** pArr, int width, int height);

/**
 * Apply a blur effect to the Pixel array. The dimension of the array is width * height. 
 * Useful for applying a blur effect to the image.
 *
 * @param  pArr: Pixel array of the image that this header is for
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 */
void blur(struct Pixel** pArr, int width, int height);

#endif