/*
 * Author: Tristan Breen
 * Date: 10/26/2023
 * Version: 1
 * Description: Main file that can be run in command prompt
 *       
 *      gcc -o myprogram BreenFilter.c PixelProcessor.c BmpProcessor.c -pthread
        myprogram.exe -o out.bmp -i test1wonderbread.bmp -f b
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BmpProcessor.h"
#include "PixelProcessor.h"


int main(int argc, char *argv[])
{
    char filterType = 'c';

    FILE *file_input = NULL;
    FILE *file_output = NULL;

    for (int i = 1; i < argc; i+=2) {
        if (strcmp(argv[i], "-i") == 0)
        {
            file_input = fopen((argv[i + 1]),"rb");
            //printf("Input File Argument: %s\n", argv[i + 1]);
        }
        if (strcmp(argv[i], "-o") == 0)
        {
            file_output = fopen((argv[i + 1]), "wb");
            //printf("Output File Argument: %s\n", argv[i + 1]);
        }
        if (strcmp(argv[i], "-f") == 0)
        {
            filterType = argv[i + 1][0];
            //printf("Filter Type Argument: %s\n", argv[i + 1]);
        }

    }
    if(filterType != 'c' && filterType != 'b')
    {
        printf("Invalid filter type. Please choose 'c' or 'b' \n");
        return 1; 
    }
    if (file_input == NULL) 
    {
        printf("Missing input file\n");
        return 1;  
    }
    if (file_output == NULL) 
    {
        file_output = fopen("OUTPUT.bmp", "wb");
    } 


    struct Pixel** pArr;

    struct BMP_Header header;
    struct DIB_Header header_dib;
            
    readBMPHeader(file_input,&header);
    readDIBHeader(file_input,&header_dib);
    pArr = (struct Pixel**)malloc(sizeof(struct Pixel*) * header_dib.ImageHeight);

    for (int i = 0; i < header_dib.ImageHeight; i++) 
    {
        pArr[i] = (struct Pixel*)malloc(sizeof(struct Pixel) * header_dib.ImageWidth);
    }

    readPixelsBMP(file_input,pArr,header_dib.ImageWidth,header_dib.ImageHeight);
    writeBMPHeader(file_output,&header);
    writeDIBHeader(file_output,&header_dib);

    if(filterType == 'c')
    {
        cheeseFilter(pArr,header_dib.ImageWidth,header_dib.ImageHeight);
    }
    if(filterType == 'b')
    {
        blur(pArr,header_dib.ImageWidth,header_dib.ImageHeight);
    }
  
    writePixelsBMP(file_output,pArr,header_dib.ImageWidth,header_dib.ImageHeight,0);

 
    for (int i = 0; i < header_dib.ImageHeight; i++) 
    {
        free(pArr[i]);
    }
    free(pArr);
  
    fclose(file_output);
    fclose(file_input);

    printf("PROGRAM DONE");
    return 0;
}