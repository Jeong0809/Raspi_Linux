#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>                     /* USHRT_MAX 상수를 위해서 사용 */
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include "bmpHeader.h"

#define FBDEVFILE            "/dev/fb0"
#define NO_OF_COLOR    24/8

#define LIMIT_USHRT(n) (n>USHRT_MAX)?USHRT_MAX:(n<0)?0:n
#define LIMIT_UBYTE(n) (n>UCHAR_MAX)?UCHAR_MAX:(n<0)?0:n

typedef unsigned char ubyte;

extern int readBmp(char *filename, ubyte **pData, int *cols, int *rows);

unsigned short makepixel(unsigned char r, unsigned char g, unsigned char b) {
    return (unsigned short)(((r>>3)<<11)|((g>>2)<<5)|(b>>3));
}

unsigned char clip(int value, int min, int max);
unsigned char clip(int value, int min, int max)
{
	return(value > max? max : value > min ? min : value);
}

void cvtBGR2Gray(ubyte *dest, ubyte *src, int n)
{
	for(int i = 0; i < n; i++){
		int b = *src++;
		int g = *src++;
		int r = *src++;

		int y = (r * 77) + (g * 151) + (b * 28);
		*dest++ = (y>>8);
		*dest++ = (y>>8);
		*dest++ = (y>>8);
	}
}

int readBmp(char *filename, unsigned char **data, int *cols, int *rows)
{
	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER bmpInfoHeader;
	FILE *fp;

	fp = fopen(filename, "rb");
	if(fp == NULL) {
		perror("ERROR\n");
		return -1;
	}

	fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

	fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	if(bmpInfoHeader.biBitCount != 24) {
		perror("This image file doesn't supports 24bit color \n");
		fclose(fp);
		return -1;
	}

	*cols = bmpInfoHeader.biWidth;
	*rows = bmpInfoHeader.biHeight;

	printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);
	printf("Bit Count : %d\n", bmpInfoHeader.biBitCount);

	fseek(fp, bmpHeader.bfOffBits, SEEK_SET);
	fread(*data, 1, bmpHeader.bfSize-bmpHeader.bfOffBits, fp);

	fclose(fp);

	return 0;
}

int main(int argc, char **argv)
{
    int cols, rows, color = 24;
    int width, height;
    ubyte *pData;
    unsigned char r, g, b, a = 255;
    unsigned short *pBmpData, *pfbmap, pixel;
	ubyte *pImageData;
    struct fb_var_screeninfo vinfo;
    int fbfd;
    int x, y, k, t;

    if(argc != 2) {
        printf("\nUsage: ./%s xxx.bmp\n", argv[0]);
        return 0;
    }

    /* 프레임버퍼를 오픈한다. */
    fbfd = open(FBDEVFILE, O_RDWR);
    if(fbfd < 0) {
        perror("open( )");
        return -1;
    }

    /* 현재 프레임버퍼에 대한 고정된 화면 정보를 얻어온다. */
    if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
        perror("ioctl( ) : FBIOGET_VSCREENINFO");
        return -1;
    }

    cols = vinfo.xres;
    rows = vinfo.yres;

    /* BMP 출력을 위한 변수의 메모리 할당 */
    pBmpData = (unsigned short *)malloc(cols * rows * sizeof(unsigned short));
    pData = (unsigned char *)malloc(cols * rows * sizeof(unsigned char) * NO_OF_COLOR);
	pImageData = (ubyte*)malloc(vinfo.xres*vinfo.yres*sizeof(ubyte)*color/8);

    /* 프레임버퍼에 대한 메모리 맵을 수행한다. */
    pfbmap = (unsigned short *)mmap(0, cols*rows*2, \
                PROT_READ|PROT_WRITE,  MAP_SHARED, fbfd, 0);
        if((unsigned)pfbmap == (unsigned)-1) {
        perror("mmap( )");
        return -1;
    }

    /* BMP 파일에서 헤더 정보를 가져온다. */
    if(readBmp(argv[1], &pImageData, &cols, &rows) < 0) {
        perror("readBMP( )");
        return -1;
    }

	cvtBGR2Gray(pData, pImageData, vinfo.xres * vinfo.yres);

    printf("\nWidth : %d, Height : %d\n", width, height);

    /* 24 비트의 BMP 이미지 데이터를 16비트 이미지 데이터로 변경 */
    for(y = 0; y < height; y++) {
        k = (height-y-1)*width*NO_OF_COLOR;
        t = y*cols;
 
        for(x = 0; x < width; x++) {
            b = LIMIT_UBYTE(pData[k+x*NO_OF_COLOR+0]);
            g = LIMIT_UBYTE(pData[k+x*NO_OF_COLOR+1]);
            r = LIMIT_UBYTE(pData[k+x*NO_OF_COLOR+2]);

			b += 50;
			g += 50;
			r += 50;

			clip(b, 0, 255);
			clip(g, 0, 255);
			clip(r, 0, 255);

            pixel = LIMIT_USHRT(makepixel(r, g, b));
            pBmpData[t+x] = pixel; 
        };
    };

    /* 앞에서 생성한 16비트 BMP 데이터 메모리맵된 메모리 공간으로 복사 */
    memcpy(pfbmap, pBmpData, cols*rows*2);

    /* 사용한 메모리맵 공간과 설정된 메모리를 해제하고 프레임버퍼 파일을 닫음 */
    munmap(pfbmap, cols*rows*2);
    free(pBmpData);
    free(pData);

    close(fbfd);

    return 0;
}


