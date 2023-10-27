/*
 * Author: Tristan Breen
 * Date: 9/28/2023
 * Version: 1
 * Description: used to handle taking in bmp files
 */

#include <stdio.h>
#include <stdlib.h>
#include "BmpProcessor.h"
#include "PixelProcessor.h"


/**
 * read BMP header of a file. Useful for converting files from PPM to BMP.
 *
 * @param  file: A pointer to the file being read or written
 * @param  header: Pointer to the destination BMP header
 */
void readBMPHeader(FILE* file, struct BMP_Header* header)
{
    fread(&header->signature, (sizeof(char) * 2), 1, file);
    fread(&header->size, sizeof(int), 1, file);
    fread(&header->reserve1, sizeof(short), 1, file);
    fread(&header->reserve2, sizeof(short), 1, file);
    fread(&header->offset_pixel_array, sizeof(int), 1, file);
}

/**
 * write BMP header of a file. Useful for converting files from PPM to BMP.
 *
 * @param  file: A pointer to the file being read or written
 * @param  header: The header made by makeBMPHeader function
 */
void writeBMPHeader(FILE* file, struct BMP_Header* header)
{
    fwrite(&header->signature, sizeof(char), 2, file);
    fwrite(&header->size, sizeof(int), 1, file);
    fwrite(&header->reserve1, sizeof(short), 1, file);
    fwrite(&header->reserve2, sizeof(short), 1, file);
    fwrite(&header->offset_pixel_array, sizeof(int), 1, file);
}

/**
 * read DIB header from a file. Useful for converting files from PPM to BMP.
 *
 * @param  file: A pointer to the file being read or written
 * @param  header: Pointer to the destination DIB header
 */
void readDIBHeader(FILE* file, struct DIB_Header* header)
{
    fread(&header->DIBHeaderSize, sizeof(int), 1, file);
    fread(&header->ImageWidth, sizeof(int), 1, file);
    fread(&header->ImageHeight, sizeof(int), 1, file);
    fread(&header->Planes, sizeof(short), 1, file);
    fread(&header->BitsPerPixel, sizeof(short), 1, file);
    fread(&header->Compression, sizeof(int), 1, file);
    fread(&header->ImageSize, sizeof(int), 1, file);
    fread(&header->XPixelsPerMeter, sizeof(int), 1, file);
    fread(&header->YPixelsPerMeter, sizeof(int), 1, file);
    fread(&header->ColorsInTable, sizeof(int), 1, file);
    fread(&header->ImportantColorCount, sizeof(int), 1, file);
}
/**
 * write DIB header of a file. Useful for converting files from PPM to BMP.
 *
 * @param  file: A pointer to the file being read or written
 * @param  header: The header made by makeDIBHeader function
 */
void writeDIBHeader(FILE* file, struct DIB_Header* header)
{
    fwrite(&header->DIBHeaderSize, sizeof(int), 1, file);
    fwrite(&header->ImageWidth, sizeof(int), 1, file);
    fwrite(&header->ImageHeight, sizeof(int), 1, file);
    fwrite(&header->Planes, sizeof(short), 1, file);
    fwrite(&header->BitsPerPixel, sizeof(short), 1, file);
    fwrite(&header->Compression, sizeof(int), 1, file);
    fwrite(&header->ImageSize, sizeof(int), 1, file);
    fwrite(&header->XPixelsPerMeter, sizeof(int), 1, file);
    fwrite(&header->YPixelsPerMeter, sizeof(int), 1, file);
    fwrite(&header->ColorsInTable, sizeof(int), 1, file);
    fwrite(&header->ImportantColorCount, sizeof(int), 1, file);
}

/**
 * make BMP header based on width and height. 
 * The purpose of this is to create a new BMPHeader struct using the information 
 * from a PPMHeader when converting from PPM to BMP.
 *
 * @param  header: Pointer to the destination DIB header
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 */
void makeBMPHeader(struct BMP_Header* header, int width, int height)
{
    header->signature[0] = 'B';
    header->signature[1] = 'M';

    /* The code is calculating the padding required for each row of pixels in a BMP file. In BMP files,
    each row of pixels must be a multiple of 4 bytes. The formula `(4 - (width * 3) % 4) % 4`
    calculates the number of padding bytes needed for each row. */
    int padding = (4 - (width * 3) % 4) % 4;
    header->size = (54+width*height*3+(padding*width));

    header->reserve1 = 0;
    header->reserve2 = 0;
    header->offset_pixel_array = 54;
}

 /**
 * Makes new DIB header based on width and height. Useful for converting files from PPM to BMP.
 *
 * @param  header: Pointer to the destination DIB header
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 */
void makeDIBHeader(struct DIB_Header* header, int width, int height)
{
    header->DIBHeaderSize = 40;
    header->ImageWidth = width;
    header->ImageHeight = height;
    header->Planes = 1;
    header->BitsPerPixel = 24;
    header->Compression = 0;

    /* The code is calculating the padding required for each row of pixels in a BMP file. In BMP files,
    each row of pixels must be a multiple of 4 bytes. The formula `(4 - (width * 3) % 4) % 4`
    calculates the number of padding bytes needed for each row. */
    int padding = (4 - (width * 3) % 4) % 4;
    header->ImageSize = (width*height*3+(padding*width));

    header->XPixelsPerMeter = 0;
    header->YPixelsPerMeter = 0;
    header->ColorsInTable = 0;
    header->ImportantColorCount = 0;
}


/**
 * read Pixels from BMP file based on width and height.
 *
 * @param  file: A pointer to the file being read or written
 * @param  pArr: Pixel array of the image that this header is for
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 */

void readPixelsBMP(FILE* file, struct Pixel** pArr, int width, int height)
{
    *pArr = (struct Pixel*)malloc(sizeof(struct Pixel) * width * height);

    int padding = (4 - (width * 3) % 4) % 4;

    /* The code is reading the pixel values from a BMP file and storing them in a pixel array. */
    for (int i = 0; i < height; i++) 
    {
        for (int j = 0; j < width; j++) 
        {
            fread(&((*pArr)[i * width + j].blue), sizeof(unsigned char), 1, file);
            fread(&((*pArr)[i * width + j].green), sizeof(unsigned char), 1, file);
            fread(&((*pArr)[i * width + j].red), sizeof(unsigned char), 1, file);
        }

        fseek(file, sizeof(char) * padding, SEEK_CUR);
    }
}


/**
 * write Pixels from BMP file based on width and height.
 *
 * @param  file: A pointer to the file being read or written
 * @param  pArr: Pixel array of the image that this header is for
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 */
void writePixelsBMP(FILE* file, struct Pixel** pArr, int width, int height, int invert)
{
    int padding = (4 - (width * 3) % 4) % 4;

    /* The code block is responsible for writing the pixel values from a BMP file to a new file.
    However, if the `invert` parameter is set to 1, it will write the pixel values in reverse order,
    starting from the bottom of the image and moving upwards. This effectively flips the image
    vertically. */
    if(invert == 1)
    {
        for (int i = height; i > -1; i--) 
        {
            for (int j = 0; j < width; j++) 
            {
                fwrite(&((*pArr)[i * width + j].blue), sizeof(unsigned char), 1, file);
                fwrite(&((*pArr)[i * width + j].green), sizeof(unsigned char), 1, file);
                fwrite(&((*pArr)[i * width + j].red), sizeof(unsigned char), 1, file);
            }

            for (int k = 0; k < padding; k++) 
            {
                unsigned char paddingByte = 0;
                fwrite(&paddingByte, sizeof(unsigned char), 1, file);
            }
        }
    }
    /* The `else` block is responsible for writing the pixel values from a BMP file to a new file
    without flipping the image vertically. It follows the same logic as the previous block, but
    instead of starting from the bottom of the image and moving upwards, it starts from the top and
    moves downwards. This block is executed when the `invert` parameter is set to 0. */
    else
    {
        for (int i = 0; i < height; i++) 
        {
            for (int j = 0; j < width; j++) 
            {
                fwrite(&((*pArr)[i * width + j].blue), sizeof(unsigned char), 1, file);
                fwrite(&((*pArr)[i * width + j].green), sizeof(unsigned char), 1, file);
                fwrite(&((*pArr)[i * width + j].red), sizeof(unsigned char), 1, file);
            }

            for (int k = 0; k < padding; k++) 
            {
                unsigned char paddingByte = 0;
                fwrite(&paddingByte, sizeof(unsigned char), 1, file);
            }
        }
    }
}
