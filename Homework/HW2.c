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
	unsigned int Imagesize;
	FILE* fp;
	RGBQUAD *palrgb;

	if(argc != 3) {
		printf(stderr, "Error!");
		return -1;
	}
	
	if((fp=fopen(argv[1], "rb")) == NULL) {
		fprintf(stderr, "Error : Failed to open file...₩n");
		return -1;
	}

	fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	Imagesize = bmpInfoHeader.biWidth * bmpInfoHeader.biHeight * 3;
	elemSize = bmpInfoHeader.bibitCount / 8;

	inimg = (ubyte*)malloc(sizeof(ubyte)*Imagesize);
	outimg = (ubyte*)malloc(sizeof(ubyte)*bmpInfoHeader.biWidth * bmpInfoHeader.biHeight);
	
	fread(inimg, sizeof(ubyte), Imagesize, fp);

	size = bmpInfoHeader.biWidth * 3; 

	for(y = 0; y < bmpInfoHeader.biHeight; y++){
		for(x = 0; x < bmpInfoHeader.biWidth*elemSize; x += 3){
				B = inimg[x + y*size + 0];
				G = inimg[x + y*size + 1];
				R = inimg[x + y*size + 2];
				
				outimg[x / elemSize + y * bmpInfoHeader.biWidth]
					=(R*0.299F)+(G*0.587F)+(B*0.114F);
		}
	}

	if(fp = fopen(argv[2], "wb") == NULL) {
		fprintf(stderr, "Error");
		return -1;
	}
	
	bmpHeader.biBitCount = 8;
	bmpHeader.biCompression = 0;
	bmpHeader.biClrUsed = 256;
	
	bmpInfoHeader.bfSize = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) +
			sizeof(RGBQUAD) + bmpInfoHeader.SizeImage;
	bmpInfoHeader.bf0ffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);



	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	palrgb = (RGBQUAD*)malloc(sizeof(RGBQUAD) * bmpInfoHeader.ClrUsed);

	fwrite(&palrgb, sizeof(RGBQUAD), bmpInfoHeader.ClrUsed, fp);
	fwrite(outimg, sizeof(bmpInfoHeader.biWidth * bmpInfoHeader.biHeight), 1, fp);

	free(inimg);
	free(outimg);

	return 0;
}

