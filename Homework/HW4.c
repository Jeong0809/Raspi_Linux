#include "bmpHeader3.h"
#include <stdio.h>
#include <stdlib.h>

#define	ubyte unsigned char

int main(int argc, char** argv)
{
	ubyte* inimg, * outimg;
	BITMAPINFOHEADER bmpInfoHeader;
	BITMAPFILEHEADER bmpHeader;
	int size, elemSize;
	ubyte R, G, B;
	int x, y;
	int Imagesize;
	FILE* fp;
	RGBQUAD* palrgb;

	// 입력하는 인자의 갯수 ( ./HW4 lena.bmp out.bmp )
	if (argc != 3) {
		fprintf(stderr, "Error!");
		return -1;
	}

	/*****read bmp*****/
	if ((fp = fopen(argv[1], "rb")) == NULL) {
		fprintf(stderr, "Error : Failed to open file1..\n");
		return -1;
	}

	/* bmpHeader의 순서에 따라 FILEHEADER, INFOHEADER, RGBQUAD, 픽셀 데이터 순으로
	  이미지 데이터를 읽어들임 */
	fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	// 24bit 이미지이므로 width X height X 3을 해줌 (RGB)
	Imagesize = bmpInfoHeader.biWidth * bmpInfoHeader.biHeight * 3;
	elemSize = bmpInfoHeader.biBitCount / 8;

	// 각 이미지들의 공간을 동적 할당, 입력 이미지는 24bit의 트루컬러 이미지
	inimg = (ubyte*)malloc(sizeof(ubyte) * Imagesize);
	// 출력 이미지는 8bit의 grayscale 이미지
	outimg = (ubyte*)malloc(sizeof(ubyte) * bmpInfoHeader.biWidth * bmpInfoHeader.biHeight);

	// 입력 이미지를 24bit 트루 컬러 이미지 사이즈에 맞게 읽어들임
	fread(inimg, sizeof(ubyte), Imagesize, fp);
	fclose(fp);

	for (y = 0; y < bmpInfoHeader.biHeight * bmpInfoHeader.biWidth; y++) {
		// 24bit 입력 이미지의 R, G, B 값을 추출해서 8bit 출력 이미지의 픽셀 데이터로 넣어줌
		B = *(inimg + y * elemSize + 0);
		G = *(inimg + y * elemSize + 1);
		R = *(inimg + y * elemSize + 2);

		printf("R : %d, G : %d, B : %d\n", R, G, B);

		*(outimg + y) = (R * 0.299F) + (G * 0.587F) + (B * 0.114F);
	}

	/*write bmp*/
	if ((fp = fopen(argv[2], "wb")) == NULL) {
		fprintf(stderr, "Error");
		return -1;
	}

	// 팔레트를 추가하기 위해 동적 할당
	palrgb = (RGBQUAD*)malloc(sizeof(RGBQUAD) * 256);

	// 출력 이미지의 픽셀 데이터를 팔레트에 저장
	for (x = 0; x < 256; x++) {
		palrgb[x].rgbBlue = palrgb[x].rgbGreen = palrgb[x].rgbRed = x;
		palrgb[x].rgbReserved = 0;
	}

	// 출력 이미지의 픽셀 당 비트수 (8bit)
	bmpInfoHeader.biBitCount = 8;

	// 압축하지 않음
	bmpInfoHeader.biCompression = 0;
	
	// 8bit grayscale 이미지의 팔레트 갯수 = 2^(8) 
	bmpInfoHeader.biClrUsed = 256;
	bmpInfoHeader.SizeImage = bmpInfoHeader.biWidth * bmpInfoHeader.biHeight;

	// 이미지를 구성하는 BMP 파일의 전체 사이즈
	bmpHeader.bfSize = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) +
		sizeof(RGBQUAD) * 256 + bmpInfoHeader.SizeImage;
	
	// BMP 파일에서 픽셀 데이터 이전까지의 offset 사이즈
	bmpHeader.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER)
		+ sizeof(RGBQUAD) * 256;

	/* BMP 파일헤더, 정보헤더, 팔레트, 픽셀 데이터 순으로 출력 이미지를 채워줌 */
	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(palrgb, sizeof(RGBQUAD), 256, fp);
	fwrite(outimg, sizeof(ubyte), bmpInfoHeader.biWidth * bmpInfoHeader.biHeight, fp);

	// 이미지를 위해 할당 했던 공간 해제
	free(inimg);
	free(outimg);

	return 0;
}



