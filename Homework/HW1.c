#include "bmpHeader3.h"
#include <stdio.h>
#include <stdlib.h>

#define	ubyte unsigned char

int main(int argc, char **argv)
{
	ubyte *inimg, *outimg;
	BITMAPINFOHEADER bmpInfoHeader;
	BITMAPFILEHEADER bmpHeader;
	int size;
	ubyte R, G, B;
	int x, y;
	unsigned int Imagesize;
	FILE* fp;
	//const char filepath[1000] = {0};
	
	/*scanf("%s", filepath);
	FILE *fp = fopen(filepath, "r+b");*/

	if((fp=fopen(argv[1], "rb")) == NULL) {
		fprintf(stderr, "Error : Failed to open file...₩n");
		return -1;
	}

	fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	Imagesize = bmpInfoHeader.biWidth * bmpInfoHeader.biHeight * 3;

	inimg = (ubyte*)malloc(sizeof(ubyte)*Imagesize);
	fread(inimg, sizeof(ubyte), Imagesize, fp);


	size = bmpInfoHeader.biWidth * 3; 

	for(y = 0; y < bmpInfoHeader.biHeight; y++){
		for(x = 0; x < bmpInfoHeader.biWidth*3; x += 3){
				B = inimg[x + y*size + 0];
				G = inimg[x + y*size + 1];
				R = inimg[x + y*size + 2];

				printf("R : %d, G : %d, B : %d\n", R, G, B);
		}
	}

	free(inimg);

	return 0;
}

