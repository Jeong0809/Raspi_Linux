#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>                     /* USHRT_MAX 상수를 위해서 사용한다. */
#include <unistd.h>

#include "bmpHeader.h"

/* 이미지 데이터의 경계 검사를 위한 매크로 */
#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)

typedef unsigned char ubyte;

int main(int argc, char** argv) 
{
    FILE* fp; 
    BITMAPFILEHEADER bmpHeader;             /* BMP FILE INFO */
    BITMAPINFOHEADER bmpInfoHeader;     /* BMP IMAGE INFO */
    RGBQUAD *palrgb;
    ubyte *inimg, *outimg;
    int x, y, z, imageSize, outimageSize;

    if(argc != 3) {
        fprintf(stderr, "usage : %s input.bmp output.bmp\n", argv[0]);
        return -1;
    }
    
    /***** read bmp *****/ 
    if((fp=fopen(argv[1], "rb")) == NULL) { 
        fprintf(stderr, "Error : Failed to open file...₩n"); 
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

    /* 트루 컬러를 지원하면 변환할 수 없다. */
    if(bmpInfoHeader.biBitCount != 24) {
        perror("This image file doesn't supports 24bit color\n");
        fclose(fp);
        return -1;
    }
    
    int elemSize = bmpInfoHeader.biBitCount/8;
    int size = bmpInfoHeader.biWidth*elemSize;
    imageSize = size * bmpInfoHeader.biHeight; 
    //outimageSize = (bmpInfoHeader.biWidth+2)*elemSize *
					(bmpInfoHeader.biHeight+2); 


    /* 이미지의 해상도(넓이 × 깊이) */
    printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);
    printf("Bit Count : %d\n", bmpInfoHeader.biBitCount);     /* 픽셀당 비트 수(색상) */
    printf("Image Size : %d\n", imageSize);
    printf("Out Image Size : %d\n", outimageSize);

    inimg = (ubyte*)malloc(sizeof(ubyte)*imageSize); 
    outimg = (ubyte*)malloc(sizeof(ubyte)*imageSize);
    fread(inimg, sizeof(ubyte), imageSize, fp); 
    
    fclose(fp);

    // define the kernel
    float kernel[3][3] = { {1/9.0, 1/9.0, 1/9.0},
                           {1/9.0, 1/9.0, 1/9.0},
                           {1/9.0, 1/9.0, 1/9.0} };

    memset(outimg, 0, sizeof(ubyte)*outimageSize);
    int arr[9] = {0};

    for(z = 0; z < elemSize; z++) {

    for(y = 1; y < bmpInfoHeader.biHeight - 1; y++) { 
        for(x = 1; x < (bmpInfoHeader.biWidth - 1) * elemSize; x+=elemSize) {
	    if ((x > 0) && (x < bmpInfoHeader.biWidth * elemSize) && (y > 0) 
			&& (y < bmpInfoHeader.biHeight)){
            
                    float sum = 0.0;
                    for(int i = -1; i < 2; i++) {
                    	for(int j = -1; j < 2; j++) {
                        	sum += kernel[i+1][j+1]*inimg[(x-i*elemSize)+(y-j)*size+z];
                    	}
                    }
                	outimg[x+y*size+z] = sum;
            	}
       	    }
	}


    for(x = 1; x < (bmpInfoHeader.biWidth - 2) * elemSize; x++) {
	float sum = 0.0;
	arr[0] = arr[3] = inimg[(x - 1) * elemSize + z] * kernel[0][1];
	arr[1] = arr[4] = inimg[x * elemSize + z] * kernel[1][1];
	arr[2] = arr[5] = inimg[(x + 1) * elemSize + z] * kernel[2][1];
	arr[6] = inimg[(x - 1) * elemSize + size + z] * kernel[0][2];
	arr[7] = inimg[x * elemSize + size + z] * kernel[1][2];
	arr[8] = inimg[(x + 1) * elemSize + size + z] * kernel[2][2];

	sum = arr[0] + arr[1] + arr[2] + arr[3] + arr[4] + arr[5] + arr[6]
		+ arr[7] + arr[8];
	outimg[x * elemSize + z] = sum;
    }

   for(x = 1; x < (bmpInfoHeader.biWidth - 2) * elemSize; x++) {
	float sum = 0.0;
	arr[6] = arr[3] = inimg[(x - 1) * elemSize +  
		size*(bmpInfoHeader.biHeight - 1) + z] * kernel[0][1];

	arr[7] = arr[4] = inimg[x * elemSize +
	       	size*(bmpInfoHeader.biHeight - 1) + z] * kernel[1][1];

	arr[8] = arr[5] = inimg[(x + 1) * elemSize +
	       	size*(bmpInfoHeader.biHeight - 1) + z] * kernel[2][1];

	arr[0] = inimg[(x - 1) * elemSize +
	       	size*(bmpInfoHeader.biHeight - 2) + z] * kernel[0][0];

	arr[1] = inimg[x * elemSize +
	       	size*(bmpInfoHeader.biHeight - 2) + z] * kernel[1][0];

	arr[2] = inimg[(x + 1) * elemSize +
	      	size*(bmpInfoHeader.biHeight - 2) + z] * kernel[2][0];
	
	sum = arr[0] + arr[1] + arr[2] + arr[3] + arr[4] + arr[5] + arr[6]
		+ arr[7] + arr[8];
	outimg[x * elemSize +
	       	size*(bmpInfoHeader.biHeight - 1) + z] = sum;

    }

   for(y = 1; y < bmpInfoHeader.biHeight - 1; y++) {
	float sum = 0.0;
	arr[0] = arr[1] = inimg[(y - 1) * size + z] * kernel[1][0];
	arr[3] = arr[4] = inimg[(y) * size + z] * kernel[1][1];
	arr[6] = arr[7] = inimg[(y + 1) * size] * kernel[1][2];
	arr[2] =  inimg[elemSize + (y - 1) * size + z] * kernel[2][0]; 
	arr[5] =  inimg[elemSize + (y) * size + z] * kernel[2][1]; 
	arr[8] =  inimg[elemSize + (y + 1) * size + z] * kernel[2][2]; 	
	
	sum = arr[0] + arr[1] + arr[2] + arr[3] + arr[4] + arr[5] + arr[6]
		+ arr[7] + arr[8];
	outimg[(y) * size + z] = sum;

   }

   for(y = 1; y < bmpInfoHeader.biHeight - 1; y++) {
	float sum = 0.0;
	arr[2] = arr[1] = inimg[elemSize * (bmpInfoHeader.biWidth - 1) + 
		(y - 1) * size + z] * kernel[1][0];

	arr[5] = arr[4] = inimg[elemSize * (bmpInfoHeader.biWidth - 1) + 
		y * size + z] * kernel[1][1];

	arr[8] = arr[7] = inimg[elemSize * (bmpInfoHeader.biWidth - 1) + 
		(y + 1) * size] * kernel[1][2];

	arr[0] =  inimg[elemSize * (bmpInfoHeader.biWidth - 2) + 
		(y - 1) * size + z] * kernel[0][0]; 

	arr[3] =  inimg[elemSize * (bmpInfoHeader.biWidth - 2) + 
		y * size + z] * kernel[0][1]; 

	arr[6] =  inimg[elemSize * (bmpInfoHeader.biWidth - 2) + 
		(y + 1) * size + z] * kernel[0][2]; 
	
	sum = arr[0] + arr[1] + arr[2] + arr[3] + arr[4] + arr[5] + arr[6]
		+ arr[7] + arr[8];
	outimg[elemSize * (bmpInfoHeader.biWidth - 1) + 
		y * size + z] = sum;

   }

   //left-Top
   float sum = 0.0;
   arr[4] = arr[0] = arr[1] = arr[3] = inimg[z]*kernel[1][1];
   arr[5] = arr[2] = inimg[elemSize + z] * kernel[2][1];
   arr[7] = arr[6] = inimg[size + z]*kernel[1][2];
   arr[8] = inimg[elemSize + size + z]*kernel[2][2];

   sum = arr[0] + arr[1] + arr[2] + arr[3] + arr[4] + arr[5] + arr[6]
		+ arr[7] + arr[8];
   outimg[z] = sum;


   //left-Bottom
   sum = 0.0;
   arr[4] = arr[3] = arr[6] = arr[7] = inimg[size * (bmpInfoHeader.biHeight - 1) + z]
	   * kernel[1][1];
   arr[1] = arr[0] = inimg[size * (bmpInfoHeader.biHeight - 2) + z]*kernel[1][0];
   arr[5] = arr[8] = inimg[elemSize + size*(bmpInfoHeader.biHeight - 1) + z] 
	   * kernel[2][1];
   arr[2] = inimg[elemSize + size*(bmpInfoHeader.biHeight - 2) + z]*kernel[2][0];

   sum = arr[0] + arr[1] + arr[2] + arr[3] + arr[4] + arr[5] + arr[6]
		+ arr[7] + arr[8];
   outimg[size * (bmpInfoHeader.biHeight - 1) + z] = sum;

   //right-Top
   sum = 0.0;
   arr[4] = arr[1] = arr[2] = arr[5] = inimg[elemSize * (bmpInfoHeader.biWidth - 1) + z]
	   *kernel[1][1];
   arr[3] = arr[0] = inimg[elemSize * (bmpInfoHeader.biWidth - 2) + z] * kernel[0][1];
   arr[7] = arr[8] = inimg[elemSize * (bmpInfoHeader.biWidth - 1) + size + z]*kernel[1][2];
   arr[6] = inimg[elemSize * (bmpInfoHeader.biWidth - 2) + size + z]*kernel[0][2];
   sum = arr[0] + arr[1] + arr[2] + arr[3] + arr[4] + arr[5] + arr[6]
		+ arr[7] + arr[8];
   outimg[elemSize * (bmpInfoHeader.biWidth - 1) + z] = sum;


   //right-Bottom
   sum = 0.0;
   arr[4] = arr[5] = arr[7] = arr[8] = inimg[elemSize * (bmpInfoHeader.biWidth - 1)
	   + size * (bmpInfoHeader.biHeight - 1) + z]*kernel[1][1];
   arr[1] = arr[2] = inimg[elemSize * (bmpInfoHeader.biWidth - 1) 
	   + size * (bmpInfoHeader.biHeight - 2) + z] * kernel[1][0];
   arr[3] = arr[6] = inimg[elemSize * (bmpInfoHeader.biWidth - 2) 
	   + size * (bmpInfoHeader.biHeight - 1) + z]*kernel[0][1];
   arr[0] = inimg[elemSize * (bmpInfoHeader.biWidth - 2) 
	   + size * (bmpInfoHeader.biHeight - 2) + z]*kernel[0][0];
   
   sum = arr[0] + arr[1] + arr[2] + arr[3] + arr[4] + arr[5] + arr[6]
		+ arr[7] + arr[8];
   outimg[elemSize * (bmpInfoHeader.biWidth - 1)
	   + size * (bmpInfoHeader.biHeight - 1) + z] = sum;


    }
     
    /***** write bmp *****/ 
    if((fp=fopen(argv[2], "wb"))==NULL) { 
        fprintf(stderr, "Error : Failed to open file...₩n"); 
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

    //fwrite(inimg, sizeof(ubyte), imageSize, fp); 
    fwrite(outimg, sizeof(ubyte), imageSize, fp);

    fclose(fp); 
    
    free(inimg); 
    free(outimg);
    
    return 0;
}

