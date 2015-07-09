/*
 * IEEE@UIC
 * VISION HEADER
 *
 */
#ifndef SOBLETRYING_HPP_
#define SOBLETRYING_HPP_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>



using namespace std;

typedef unsigned short  WORD;
typedef unsigned int   	DWORD;
typedef long            LONG;


const int ROWS = 180;
const int COLS = 280;
const int MAX_BUFF = ROWS * COLS;


//Global pointers

char *b1;		// img1 raw data buff1
char *b2;		// img2 raw data buff2
char **input1;	// grayed img1
char **input2;	// grayed img2
char **dfram1;	// delta fram2 gen1
char **dframe2;	// delta frame gen2


inline int FindMedian(char **buff)
{

	/*
	 * ToDo Need to get the buffer filled with RGB first
	 *
	 *
	 */

	char tmp = 0;
	int cp  = 0;
	int count = 0;
	char ch = 0;

	char* tbuff;
	tbuff = (char*)malloc(sizeof(char) * MAX_BUFF);

	//move from 2d array into single buffers
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			tbuff[cp] = buff[ROWS][COLS];
			cp++;

		}
	}

	//ascending order sorting
	for(int i = 0; i < MAX_BUFF; i++)
	{
		for(int j = 0; j < MAX_BUFF - i; j++)
		{
			if(tbuff[j] > tbuff[j+1])
			{
				tmp = tbuff[j];
				tbuff[j] = tbuff[j+1];
				tbuff[j+1] = tmp;
			}
		}
	}

	//do not use until rgb buf is created
	return 0;
}

inline char* fillBuffer(string inFileName)
{

	char* tbuff;
	tbuff = (char*)malloc(sizeof(char) * MAX_BUFF);

	FILE *fp;
	fp = fopen(inFileName.c_str(), "r");
	if(fp != NULL)
	{
		while ((fgets(tbuff, sizeof(tbuff), fp)) != NULL)
		{
			//printf("%s", tbuff);
		}

		fclose(fp);
	}

	return tbuff;
}

inline char** readInput(char *buff)
{

	printf("%s",buff);
	char **RedB;
	char **output;


	int MatVal = 0;
	int cp = 0;
	int ch;

	//Inititialize Arrays

	RedB = (char**)malloc(sizeof(char *) * ROWS);
	for (int k = 0; k < ROWS; k++)
		RedB[k] = (char*)malloc(sizeof(char) * COLS);

	output = (char**)malloc(sizeof(char *) * ROWS);
	for (int k = 0; k < ROWS; k++)
		output[k] = (char*)malloc(sizeof(char) * COLS);


	//cout<<"allocated arrays"<<endl;

	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			while (true)
			{
				ch = buff[cp];

				if ((48 <= ch) && (ch <= 57))
				{
					MatVal = (MatVal * 10) + ch;
					//cout<<"		MatVal -> "<< MatVal << endl;

				}
				else
				{
					RedB[i][j] = MatVal;
					//cout<< (char)MatVal << endl;
 					MatVal = 0;
					cp++;

					if (ch == 10 || ch == 20 || ch == 3 || ch == 32)
					{
						cp++;
					}

					break;
				}
				cp++;
			}
			output[i][j] = RedB[i][j];
		}

	}

	for(int i = 0; i < ROWS; i++)
		for(int j = 0; j < COLS; j++)
			if( output[i][j] != NULL)
				cout<<"i: "<< i <<" j: "<< j <<" output: "<< output[i][j] << endl;

	free(RedB);
	return output;
}

inline char** DeltaFrameGeneration(char** in1, char** in2)
{
/*
 * After subtracting the two gray scale images we then refine the photo even deeper
 * with DeltaFrameGeneration
 *
 */
	char **seg;
	seg = (char**)malloc(sizeof(char *) * ROWS);
	for (int k = 0; k < ROWS; k++)
		seg[k] = (char*)malloc(sizeof(char) * COLS);

	char **seg1;
	seg1 = (char**)malloc(sizeof(char *) * ROWS);
	for (int k = 0; k < ROWS; k++)
		seg1[k] = (char*)malloc(sizeof(char) * COLS);


	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			seg[i][j] = in1[i][j] - in2[i][j];

			if(seg[i][j] > 20)
				seg1[i][j] = 255;
			else
				seg1[i][j] = 0;

		}
	}

	free(seg);
	return seg1;
}

inline char **MedianFilter(char **seg)
{

	int bbr = 0;
	char **firstfilter;
	firstfilter = (char**)malloc(sizeof(char *) * ROWS);
	for (int k = 0; k < ROWS; k++)
		firstfilter[k] = (char*)malloc(sizeof(char) * COLS);

	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			if( i != 0 || j != 0 || i != ROWS || j != COLS )
				bbr = FindMedian(seg);
			firstfilter[i][j] = bbr;
		}
	}

	free(seg);
	return firstfilter;

}

inline char **Thresholding(char **filter)
{

	char **EdgeImage;
	EdgeImage = (char**)malloc(sizeof(char *) * ROWS);
	for (int k = 0; k < ROWS; k++)
		EdgeImage[k] = (char*)malloc(sizeof(char) * COLS);

	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			if(filter[i][j] > 40)
				filter[i][j] = 255;
			else
				filter[i][j] = 0;

			EdgeImage[i][j] = 255;
		}
	}
	return EdgeImage;
}

inline char **EdgeDetection(char **filter)
{
	char **EdgeImage;
	EdgeImage = (char**)malloc(sizeof(char *) * ROWS);
	for (int k = 0; k < ROWS; k++)
		EdgeImage[k] = (char*)malloc(sizeof(char) * COLS);

	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			if( i == 0 && j == 0)
			{
				if(filter[j][i+1] && filter[j+1][i+1] &&
						filter[j+1][i] == 255)
					EdgeImage[j][i] = 0;

			}
			else if(i == 0 && j == ROWS)
			{
				if(filter[j+1][i] && filter[j+1][i-1] &&
						filter[j][i-1] == 255)
					EdgeImage[j][i] = 0;

			}
			else if(i == ROWS && j == 0)
			{
				if(filter[j-1][i] && filter[j-1][i+1] &&
						filter[j][i+1] == 255)
					EdgeImage[j][i] = 0;

			}
			else if(i == ROWS && j == ROWS)
			{
				if(filter[j-1][i-1] && filter[j][i-1] &&
						filter[j-1][i] == 225)
					EdgeImage[j][i] = 0;

			}
			else if(i == 1)
			{
				if(filter[j][j-1] && filter[j+1][i-1] &&
						filter[j+1][i] && filter[j+1][i+1] &&
						filter[j][i+1] == 255)
					EdgeImage[j][i] = 0;

			}
			else if(i == ROWS)
			{
				if(filter[j-1][i-1] && filter[j+1][i-1] &&
						filter[j+1][i] && filter[j-1][i] &&
						filter[j][i-1] == 255)
					EdgeImage[j][i] = 0;
			}
			else if(j == 1)
			{
				if(filter[j][i-1] && filter[j-1][i-1] &&
						filter[j-1][i] && filter[j-1][i+1] &&
						filter[j][i+1] == 255)
					EdgeImage[j][i] = 0;
			}
			else{
				if(filter[j-1][i-1] && filter[j+1][i-1] &&
						filter[j+1][i] && filter[j+1][i+1] &&
						filter[j][i-1] && filter[j-1][i] == 255)
					EdgeImage[j][i] = 0;

			}
		}
	}
	return EdgeImage;
}

inline char** EnchanceImage(char **in1,char **in2)
{
	char **seg;
	seg = (char**)malloc(sizeof(char *) * ROWS);
	for (int k = 0; k < ROWS; k++)
		seg[k] = (char*)malloc(sizeof(char) * COLS);

	char **store;
	store = (char**)malloc(sizeof(char *) * ROWS);
	for (int k = 0; k < ROWS; k++)
		store[k] = (char*)malloc(sizeof(char) * COLS);

	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			seg[i][j] = in1[i][j] - in2[i][j];

			//Not sure if this is correct check document
			if(seg[i][j] == 1)
				store[i][j] = seg[i][j];

		}
	}

	free(seg);
	return store;


}

/*
 * BMP image loading
 */

#pragma pack(push, 1)

typedef struct tagBITMAPFILEHEADER
{
    WORD bfType;  //specifies the file type
    DWORD bfSize;  //specifies the size in bytes of the bitmap file
    WORD bfReserved1;  //reserved; must be 0
    WORD bfReserved2;  //reserved; must be 0
    DWORD bOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
}BITMAPFILEHEADER;

#pragma pack(pop)

#pragma pack(push, 1)

typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize;  //specifies the number of bytes required by the struct
    LONG biWidth;  //specifies width in pixels
    LONG biHeight;  //species height in pixels
    WORD biPlanes; //specifies the number of color planes, must be 1
    WORD biBitCount; //specifies the number of bit per pixel
    DWORD biCompression;//spcifies the type of compression
    DWORD biSizeImage;  //size of image in bytes
    LONG biXPelsPerMeter;  //number of pixels per meter in x axis
    LONG biYPelsPerMeter;  //number of pixels per meter in y axis
    DWORD biClrUsed;  //number of colors used by th ebitmap
    DWORD biClrImportant;  //number of colors that are important
}BITMAPINFOHEADER;

#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
    uint8_t  rgbtRed;
    uint8_t  rgbtBlue;
    uint8_t  rgbtGreen;
}
RGBTRIPLE;
#pragma pack(pop)


unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
    FILE *filePtr; //our file pointer
    BITMAPFILEHEADER bitmapFileHeader; //our bitmap file header
    unsigned char *bitmapImage;  //store image data
    int imageIdx=0;  //image index counter
    unsigned char tempRGB;  //our swap variable

    //open filename in read binary mode
    filePtr = fopen(filename,"rb");
    if (filePtr == NULL)
        return NULL;

    //read the bitmap file header
    fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,filePtr);

    //verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader.bfType !=0x4D42)
    {
        fclose(filePtr);
        return NULL;
    }

    //read the bitmap info header
    fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,filePtr); // small edit. forgot to add the closing bracket at sizeof

    //move file point to the begging of bitmap data
    fseek(filePtr, bitmapFileHeader.bOffBits, SEEK_SET);

    //allocate enough memory for the bitmap image data

    bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);



    //verify memory allocation
    if (!bitmapImage)
    {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }

    //read in the bitmap image data
    //will read the
    fread(bitmapImage,bitmapInfoHeader->biSizeImage,1,filePtr);

    //make sure bitmap image data was read
    if (bitmapImage == NULL)
    {
        fclose(filePtr);
        return NULL;
    }

    //swap the r and b values to get RGB (bitmap is BGR)
    for (imageIdx = 0;imageIdx < bitmapInfoHeader->biSizeImage;imageIdx+=3)
    {
        tempRGB = bitmapImage[imageIdx];
        bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
        bitmapImage[imageIdx + 2] = tempRGB;
    }

    //close file and return bitmap iamge data
    fclose(filePtr);
    return bitmapImage;
}

void PrintHeaderInfo(BITMAPINFOHEADER *bitmapInfoHeader)
{

    cout<< bitmapInfoHeader->biSize  <<" //specifies the number of bytes required by the struct" << endl;
    cout<< bitmapInfoHeader->biWidth <<" //specifies width in pixels"<< endl;
    cout<< bitmapInfoHeader->biHeight <<" //species height in pixels" << endl;
    cout<< bitmapInfoHeader->biPlanes <<" //specifies the number of color planes, must be 1"<<endl;
    cout<< bitmapInfoHeader->biBitCount <<" //specifies the number of bit per pixel" <<endl;
    cout<< bitmapInfoHeader->biCompression <<" //spcifies the type of compression" <<endl;
    printf("%d //size of image in bytes\n", bitmapInfoHeader->biSizeImage);

    cout<< bitmapInfoHeader->biXPelsPerMeter <<" //number of pixels per meter in x axis" <<endl;
    cout<< bitmapInfoHeader->biYPelsPerMeter  <<" //number of pixels per meter in y axis" <<endl;
    printf("%d //number of colors used by the bitmap\n", bitmapInfoHeader->biClrUsed);
    printf("%d //number of colors that are important\n", bitmapInfoHeader->biClrImportant);

}

#endif /* SOBLETRYING_HPP_ */
