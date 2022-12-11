#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>                     /* USHRT_MAX 상수를 위해서 사용한다. */
#include <unistd.h>

#include "bmpHeader3.h"

/* 이미지 데이터의 경계 검사를 위한 매크로 */
#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)
#define SWAP(x,y,t) ((t)=(x),(x)=(y),(y)=(t))

typedef unsigned char ubyte;

void quicksort(int arr[], int left, int right) {
	if (left >= right) {//정렬할 데이터 수 1의 경우
		return;
	}
	int pivot = left;//피봇을 맨 왼쪽으로 설정한 경우
	int i = left + 1;//i는 피봇 바로 오른쪽
	int j = right;   //j는 맨 오른쪽
	int temp;
	
	while (i <= j) {//i가 j이하일 때까지
		while (arr[i] <= arr[pivot])//i의 값이 피봇 이하이면
			i++;
		while (arr[j] >= arr[pivot] && j > left)
		//j의 값이 피봇보다 크고, j가 left보다 큰 경우에 한하여
			j--;
		if(i>j)//i와 j가 크로스 되었을 경우
			SWAP(arr[j], arr[pivot], temp);
		else//i가 j보다 작은 경우는 피봇과 교체하지 않음.
			SWAP(arr[i], arr[j], temp);
	}
	quicksort(arr, left, j - 1);//피봇 기준 왼쪽 영역, 피봇보다 작은수
	quicksort(arr, j+1, right); //피봇 기준 오른쪽 영역, 피봇보다 큰수
}


int main(int argc, char** argv) 
{
    FILE* fp; 
    BITMAPFILEHEADER bmpHeader;             /* BMP FILE INFO */
    BITMAPINFOHEADER bmpInfoHeader;     /* BMP IMAGE INFO */
    RGBQUAD *palrgb;
    ubyte *inimg, *padimg, *outimg;
    int x, y, z, imageSize;

    if(argc != 3) {
        fprintf(stderr, "usage : %s input.bmp output.bmp\n", argv[0]);
        return -1;
    }
    
    /***** read bmp *****/ 
    if((fp=fopen(argv[1], "rb")) == NULL) { 
        fprintf(stderr, "Error : Failed to open file...\n"); 
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

    /* 이미지의 해상도(넓이 × 깊이) */
    printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);
    printf("Bit Count : %d\n", bmpInfoHeader.biBitCount);     /* 픽셀당 비트 수(색상) */
    printf("Image Size : %d\n", imageSize);

    inimg = (ubyte*)malloc(sizeof(ubyte)*imageSize); 
    outimg = (ubyte*)malloc(sizeof(ubyte)*imageSize);
    fread(inimg, sizeof(ubyte), imageSize, fp); 
    fclose(fp);

    int padSize = (bmpInfoHeader.biWidth + 2) * elemSize;
    int addSize = (padSize + bmpInfoHeader.biHeight)*2;
    padimg = (ubyte*)malloc(sizeof(ubyte)*(imageSize + addSize));

    /* make padding image */
    memset(padimg, 0, (sizeof(ubyte)*imageSize + addSize));
    //memset(outimg, 0, sizeof(ubyte)*imageSize);
    for(y = 0; y < bmpInfoHeader.biHeight; y++) {
        for(x = 0; x < bmpInfoHeader.biWidth * elemSize; x+=elemSize) {
            for(z = 0; z < elemSize; z++) {
                padimg[(x+elemSize)+(y+1)*padSize+z]=inimg[x+y*size+z];
            }
        }
    }

    for(y = 0; y < bmpInfoHeader.biHeight; y++) { 
        for(z = 0; z < elemSize; z++) {
            padimg[0+(y+1)*padSize+z]=inimg[0+y*size+z];
            padimg[padSize-elemSize+(y+1)*padSize+z]=inimg[size-elemSize+y*size+z];
        }
    }

    for(x = 0; x < bmpInfoHeader.biWidth*elemSize; x++) { 
        padimg[elemSize+x]=inimg[x];
        padimg[elemSize+x+(bmpInfoHeader.biHeight+1)*padSize]=inimg[x+(bmpInfoHeader.biHeight-1)*size];
    }

    for(z = 0; z < elemSize; z++) {
       padimg[z]=inimg[z];
       padimg[padSize-elemSize+z]=inimg[size-elemSize+z];
       padimg[(bmpInfoHeader.biHeight+1)*padSize+z]=inimg[(bmpInfoHeader.biHeight-1)*size+z];
       padimg[(bmpInfoHeader.biHeight+1)*padSize+padSize-elemSize+z]=inimg[(bmpInfoHeader.biHeight-1)*size+size-elemSize+z];
    }

    memset(outimg, 0, sizeof(ubyte)*imageSize);
    for(y = 1; y < bmpInfoHeader.biHeight + 1; y++) { 
        for(x = elemSize; x < padSize; x+=elemSize) {
            for(z = 0; z < elemSize; z++) {

				int median[9] = {0};
				int cnt = 0;

                for(int i = -1; i < 2; i++) {
                    for(int j = -1; j < 2; j++) {
                        median[cnt++] = padimg[(x+i*elemSize)+(y+j)*padSize+z];
                    }
                }
				quicksort(median, 0, 8);
                outimg[(x-elemSize)+(y-1)*size+z] = median[4];
            }
        }
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
    //fwrite(padimg, sizeof(ubyte), imageSize, fp);
    fwrite(outimg, sizeof(ubyte), imageSize, fp);

    fclose(fp); 

    free(inimg); 
    free(outimg);
    free(padimg);
    
    return 0;
}
