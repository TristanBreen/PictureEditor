/*
 * Author: Tristan Breen
 * Date: 9/28/2023
 * Version: 1
 * Description: used to handle pixel shifting and filter adding
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include "PixelProcessor.h"

int globalCount = 0;
int globalWidth;
int globalHeight;
struct Pixel** globalArr;
struct Pixel** globalTempArr;
int globalNumCircles;
int globalAvgRadius;
int radiusVals[] = {-10, -5, 0, -5, -10};

int THREAD_COUNT = 4;


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
void colorShiftPixels(struct Pixel** pArr, int width, int height, int rShift, int gShift, int bShift)
{
    for (int i = 0; i < height; i++) 
    {
        for (int j = 0; j < width; j++) 
        {
            int shiftedRed = (*pArr)[i * width + j].red + rShift;
            int shiftedGreen = (*pArr)[i * width + j].green + gShift;
            int shiftedBlue = (*pArr)[i * width + j].blue + bShift;

            if (shiftedRed < 0) 
            {
                shiftedRed = 0;
            } else if (shiftedRed > 255) 
            {
                shiftedRed = 255;
            }

            if (shiftedGreen < 0) 
            {
                shiftedGreen = 0;
            } else if (shiftedGreen > 255) 
            {
                shiftedGreen = 255;
            }

            if (shiftedBlue < 0) 
            {
                shiftedBlue = 0;
            } else if (shiftedBlue > 255) 
            {
                shiftedBlue = 255;
            }

            // Assign clamped values back to the pixels
            (*pArr)[i * width + j].red = (unsigned char)shiftedRed;
            (*pArr)[i * width + j].green = (unsigned char)shiftedGreen;
            (*pArr)[i * width + j].blue = (unsigned char)shiftedBlue;
        }
    }
}

//online standardized formula for Mid-Point Circle Drawing Algorithm adjusted for our implementation
int** getCircumference(int xMidpt, int yMidpt, int radius)
{
    int xCounter = radius; 
    int yCounter = 0;
    int count = 0;

    int** circlePoints = (int**)malloc(2 * sizeof(int*));
    for (int i = 0; i < 2; i++)
    {
        //slightly bigger than needed
        circlePoints[i] = (int*)malloc((2 *radius * 3.5) * sizeof(int));
    }

    circlePoints[0][count] = xCounter + xMidpt;
    circlePoints[1][count] = yCounter+ yMidpt;
    count++;

    circlePoints[0][count] = -xCounter + xMidpt;
    circlePoints[1][count] = yCounter + yMidpt;
    count++;

    circlePoints[0][count] = yCounter + xMidpt;
    circlePoints[1][count] = xCounter + yMidpt;
    count++;

    circlePoints[0][count] = yCounter + xMidpt;
    circlePoints[1][count] = -xCounter + yMidpt;
    count++;

    int tempPoint = 1 - radius;
    while (xCounter > yCounter)
    {
        yCounter++;

        if (tempPoint <= 0)
            tempPoint = tempPoint + 2 * yCounter + 1;
        else
        {
            xCounter--;
            tempPoint = tempPoint + 2 * yCounter - 2 * xCounter + 1;
        }

        if (xCounter < yCounter)
            break;

        circlePoints[0][count] = xCounter + xMidpt;
        circlePoints[1][count] = yCounter + yMidpt;
        count++;

        circlePoints[0][count] = -xCounter + xMidpt;
        circlePoints[1][count] = yCounter + yMidpt;
        count++;

        circlePoints[0][count] = xCounter + xMidpt;
        circlePoints[1][count] = -yCounter + yMidpt;
        count++;

        circlePoints[0][count] = -xCounter + xMidpt;
        circlePoints[1][count] = -yCounter + yMidpt;
        count++;

        if (xCounter != yCounter)
        {
            circlePoints[0][count] = yCounter + xMidpt;
            circlePoints[1][count] = xCounter + yMidpt;
            count++;

            circlePoints[0][count] = -yCounter + xMidpt;
            circlePoints[1][count] = xCounter + yMidpt;
            count++;

            circlePoints[0][count] = yCounter + xMidpt;
            circlePoints[1][count] = -xCounter + yMidpt;
            count++;

            circlePoints[0][count] = -yCounter + xMidpt;
            circlePoints[1][count] = -xCounter + yMidpt;
            count++;
        }
    }
    globalCount = count;
    return circlePoints;
}

void drawCircle(int x, int y, int radius, struct Pixel** pArr, int width, int height)
{
    int** circlePoints = getCircumference(x, y, radius);

    for (int i = 0; i < globalCount; i+=2)
    {
        int j = i + 1;
        
        while(circlePoints[0][i] != circlePoints[0][j])
        {
            if(circlePoints[0][i] > circlePoints[0][j])
            {
                if(((circlePoints[0][i] * width + circlePoints[1][j]) < width*height-20) && (circlePoints[0][i] * width + circlePoints[1][j]) > 0)
                {
                    (*pArr)[circlePoints[0][i] * width + circlePoints[1][j]].red = 0;
                    (*pArr)[circlePoints[0][i] * width + circlePoints[1][j]].green = 0;
                    (*pArr)[circlePoints[0][i] * width + circlePoints[1][j]].blue = 0;
                }
                //printf("Black: (%d, %d)\n", circlePoints[0][i], circlePoints[1][i]);
                (circlePoints[0][i])--;
            }
            if(circlePoints[0][i] < circlePoints[0][j])
            {
                if(((circlePoints[0][i] * width + circlePoints[1][j]) < width*height-20) && (circlePoints[0][i] * width + circlePoints[1][j]) > 0)
                {
                    (*pArr)[circlePoints[0][i] * width + circlePoints[1][j]].red = 0;
                    (*pArr)[circlePoints[0][i] * width + circlePoints[1][j]].green = 0;
                    (*pArr)[circlePoints[0][i] * width + circlePoints[1][j]].blue = 0;
                }
                //printf("Black: (%d, %d)\n", circlePoints[0][i], circlePoints[1][i]);
                (circlePoints[0][i])++;
            }
            if(circlePoints[0][i] == circlePoints[0][j])
            {
                if(((circlePoints[0][i] * width + circlePoints[1][j]) < width*height-20) && (circlePoints[0][i] * width + circlePoints[1][j]) > 0)
                {
                    (*pArr)[circlePoints[0][i] * width + circlePoints[1][j]].red = 0;
                    (*pArr)[circlePoints[0][i] * width + circlePoints[1][j]].green = 0;
                    (*pArr)[circlePoints[0][i] * width + circlePoints[1][j]].blue = 0;
                }
                //printf("Black: (%d, %d)\n", circlePoints[0][i], circlePoints[1][i]);
            }
        }
        while(circlePoints[1][i] != circlePoints[1][j])
        {
            
            if(circlePoints[1][i] > circlePoints[1][j])
            {
                if(((circlePoints[0][i] * width + circlePoints[1][j]) < width*height-20) && (circlePoints[0][i] * width + circlePoints[1][j]) > 0)
                {
                    (*pArr)[circlePoints[0][i] * width + circlePoints[1][j]].red = 0;
                    (*pArr)[circlePoints[0][i] * width + circlePoints[1][j]].green = 0;
                    (*pArr)[circlePoints[0][i] * width + circlePoints[1][j]].blue = 0;
                }
                //printf("Black: (%d, %d)\n", circlePoints[0][i], circlePoints[1][i]);
                (circlePoints[1][i])--;
            }
            if(circlePoints[1][i] < circlePoints[1][j])
            {
                if(((circlePoints[0][i] * width + circlePoints[1][j]) < width*height-20) && (circlePoints[0][i] * width + circlePoints[1][j]) > 0)
                {
                    (*pArr)[circlePoints[0][i] * width + circlePoints[1][j]].red = 0;
                    (*pArr)[circlePoints[0][i] * width + circlePoints[1][j]].green = 0;
                    (*pArr)[circlePoints[0][i] * width + circlePoints[1][j]].blue = 0;
                }
                //printf("Black: (%d, %d)\n", circlePoints[0][i], circlePoints[1][i]);
                (circlePoints[1][i])++;
            }
            if(circlePoints[1][i] == circlePoints[1][j])
            {
                if(((circlePoints[0][i] * width + circlePoints[1][j]) < width*height-20) && (circlePoints[0][i] * width + circlePoints[1][j]) > 0)
                {
                    (*pArr)[circlePoints[0][i] * width + circlePoints[1][j]].red = 0;
                    (*pArr)[circlePoints[0][i] * width + circlePoints[1][j]].green = 0;
                    (*pArr)[circlePoints[0][i] * width + circlePoints[1][j]].blue = 0;
                }
                //printf("Black: (%d, %d)\n", circlePoints[0][i], circlePoints[1][i]);
            }
        }
    }

}

void* print_chunk_cheese(void* arg) 
{
    int circleArray[globalNumCircles][3];

    srand(time(NULL));

    for(int i = 0; i < globalNumCircles; i++) 
    {
        int x = rand() % globalWidth;
        
        int y = rand() % globalHeight;
        //printf("globalheight: %d y: %d\n", globalHeight, y);
        int radius = globalAvgRadius + radiusVals[rand() % (sizeof(radiusVals) / sizeof(radiusVals[0]))];

        // Store x, y, and radius values in the 2D array
        circleArray[i][0] = x;
        circleArray[i][1] = y;
        circleArray[i][2] = radius;
    }

    int thread_number = *((int*)arg);
    int chunk_size = globalNumCircles / THREAD_COUNT;
    int start_index = thread_number * chunk_size;
    int end_index;

    if (thread_number == THREAD_COUNT - 1) 
    {
        end_index = globalNumCircles;
    } else 
    {
        end_index = start_index + chunk_size;
    }

    for (int i = start_index; i < end_index; i++) 
    {
        drawCircle(circleArray[i][0],circleArray[i][1],circleArray[i][2], globalArr, globalWidth, globalHeight);
        //printf("x: %d y: %d radius: %d width: %d height: %d\n",circleArray[i][0],circleArray[i][1],circleArray[i][2], globalWidth, globalHeight );
    }

    return NULL;
}

void cheeseFilter(struct Pixel** pArr, int width, int height)
{
    colorShiftPixels(pArr,width,height,100,100,0);

    int smallSide;
    if(height > width)
    {
        smallSide = width;
    }
    else
    {
        smallSide = height;
    }

    int numCircles = floor(.1 * smallSide);
    int avgRadius = numCircles;

    globalWidth = width;
    globalHeight = height;
    globalArr = pArr;
    globalNumCircles = numCircles;
    globalAvgRadius = avgRadius;

    pthread_t threads[THREAD_COUNT];
    int thread_numbers[THREAD_COUNT];

    for (int i = 0; i < THREAD_COUNT; ++i) 
    {
        thread_numbers[i] = i;
        pthread_create(&threads[i], NULL, print_chunk_cheese, (void*)&thread_numbers[i]);
    }

    for (int i = 0; i < THREAD_COUNT; ++i) 
    {
        pthread_join(threads[i], NULL);
    }
    
}

void* print_chunk_blur(void* arg) 
{
    int thread_number = *((int*)arg);
    int chunk_size = globalHeight / THREAD_COUNT;
    int start_index = thread_number * chunk_size;
    int end_index;

    if (thread_number == THREAD_COUNT - 1) 
    {
        end_index = globalHeight;
    } else 
    {
        end_index = start_index + chunk_size;
    }

    for (int y = start_index; y < end_index; y++) 
    {
        for (int x = 0; x < globalWidth; x++) 
        {
            int avgRed = 0;
            int avgGreen = 0;
            int avgBlue = 0;
            int count = 0;

            for (int i = -1; i <= 1; i++) 
            {
                for (int j = -1; j <= 1; j++) 
                {

                    if ((y + j) >= 0 && (y + j) < globalHeight && (x + i) >= 0 && (x + i) < globalWidth) 
                    {
                        avgRed += (*globalArr)[(y + j) * globalWidth + (x + i)].red;
                        avgGreen += (*globalArr)[(y + j) * globalWidth + (x + i)].green;
                        avgBlue += (*globalArr)[(y + j) * globalWidth + (x + i)].blue;
                        count++;
                    }
                }
            }
        
            avgRed = floor(avgRed / count);
            avgGreen = floor(avgGreen / count);
            avgBlue = floor(avgBlue / count);
            
            globalTempArr[y][x].red = avgRed;
            globalTempArr[y][x].green = avgGreen;
            globalTempArr[y][x].blue = avgBlue;

            (*globalArr)[y * globalWidth + x].red = globalTempArr[y][x].red;
            (*globalArr)[y * globalWidth + x].green = globalTempArr[y][x].green;
            (*globalArr)[y * globalWidth + x].blue = globalTempArr[y][x].blue;
        }
    }

    return NULL;
}

void blur(struct Pixel** pArr, int width, int height) 
{
    globalTempArr = malloc(sizeof(struct Pixel*) * height);
    for (int i = 0; i < height; i++) 
    {
        globalTempArr[i] = malloc(sizeof(struct Pixel) * width);
    }
    globalWidth = width;
    globalHeight = height;
    globalArr = pArr;

    pthread_t threads[THREAD_COUNT];
    int thread_numbers[THREAD_COUNT];

    for (int i = 0; i < THREAD_COUNT; ++i) 
    {
        thread_numbers[i] = i;
        pthread_create(&threads[i], NULL, print_chunk_blur, (void*)&thread_numbers[i]);
    }

    for (int i = 0; i < THREAD_COUNT; ++i) 
    {
        pthread_join(threads[i], NULL);
    }
}
