#include "bmpHeader3.h"
#include <stdio.h>
#include <stdlib.h>

#define	ubyte unsigned char

int main(int argc, char **argv)
{
	ubyte *inimg, *outimg;
	BITMAPINFOHEADER bmpInfoHeader;
	BITMAPFILEHEADER bmpHeader;
	int size, elemSize;
	ubyte R, G, B;
	int x, y;
	int Imagesize;
	FILE* fp;
	RGBQUAD *palrgb;

	if(argc != 3) {
		fprintf(stderr, "Error!");
		return -1;
	}
	
	if((fp=fopen(argv[1], "rb")) == NULL) {
		fprintf(stderr, "Error : Failed to open file1..\n");
		return -1;
	}

	fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	Imagesize = bmpInfoHeader.biWidth * bmpInfoHeader.biHeight * 3;
	elemSize = bmpInfoHeader.biBitCount / 8;

	inimg = (ubyte*)malloc(sizeof(ubyte)*Imagesize);
	outimg = (ubyte*)malloc(sizeof(ubyte)*bmpInfoHeader.biWidth * bmpInfoHeader.biHeight);
	
	fread(inimg, sizeof(ubyte), Imagesize, fp);
	fclose(fp);
	
	size = bmpInfoHeader.biWidth * 3; 

	for(y = 0; y < bmpInfoHeader.biHeight*bmpInfoHeader.biWidth; y++){
		B = *(inimg + y*elemSize + 0);
		G = *(inimg + y*elemSize + 1);
		R = *(inimg + y*elemSize + 2);

		printf("R : %d, G : %d, B : %d\n", R, G, B);
		*(outimg + y)
			=(R*0.299F)+(G*0.587F)+(B*0.114F);
	}

	if((fp = fopen(argv[2], "wb")) == NULL) {
		fprintf(stderr, "Error");
		return -1;
	}

	palrgb = (RGBQUAD*)malloc(sizeof(RGBQUAD) * 256);

	for(x = 0; x < 256; x++) {
        palrgb[x].rgbBlue = palrgb[x].rgbGreen = palrgb[x].rgbRed = x;
        palrgb[x].rgbReserved = 0;
    }
	
	bmpInfoHeader.biBitCount = 8;
	bmpInfoHeader.biCompression = 0;
	bmpInfoHeader.biClrUsed = 256;
	bmpInfoHeader.SizeImage = bmpInfoHeader.biWidth * bmpInfoHeader.biHeight;
	
	bmpHeader.bfSize = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) +
			sizeof(RGBQUAD)*256 + bmpInfoHeader.SizeImage;
	bmpHeader.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER)
							+ sizeof(RGBQUAD)*256;

	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(palrgb, sizeof(RGBQUAD), 256, fp);
	fwrite(outimg, sizeof(ubyte), bmpInfoHeader.biWidth * bmpInfoHeader.biHeight, fp);

	free(inimg);
	free(outimg);

	return 0;
}

