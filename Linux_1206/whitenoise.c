#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>
#include <string.h>

#define BYTE unsigned char

#define widthbytes(bits) (((bits)+31)/32*4)

typedef struct tagRGBQUAD{
	BYTE rgbBlue; 
	BYTE rgbGreen; 
	BYTE rgbRed; 
	BYTE rgbReserved;
} RGBQUAD;

inline unsigned char clip(int value, int min, int max);
unsigned char clip(int value, int min, int max)
{
    return(value > max? max : value < min? min : value);
}

int main(int argc, char** argv) {
	FILE* fp; 
	RGBQUAD palrgb[256];
	
	/* BMP FILE INFO */
	unsigned short int type; 
	unsigned int file_size; 
	unsigned short int reserved1; 
	unsigned short int reserved2; 
	unsigned int offset;
	
	/* BMP IMAGE INFO */
	unsigned int header_size; 
	int width, height; 
	unsigned short int planes; 
	unsigned short int bits; 
	unsigned int compression; 
	unsigned int imagesize; 
	int hresolution, vresolution; 
	unsigned int ncolors, importantcolors; 

	char input[128], output[128];
	
	int i, j, size; 
	float srcX, srcY;
	int index; 
	float r,g,b,gray;
	int graysize; 
	int index2;
	
	unsigned char *grayimg, *inimg, *outimg;
	
	/* usage a.out in.bmp out.bmp */
	strcpy(input, argv[1]); 
	strcpy(output, argv[2]);
	
	
	if((fp=fopen(input, "rb")) == NULL) { 
		fprintf(stderr, "Error : Failed to open file...₩n"); 
		return -1;
	}
	
	fread(&type, sizeof(unsigned short int), 1, fp); 
	fread(&file_size, sizeof(unsigned int), 1, fp); 
	fread(&reserved1, sizeof(unsigned short int), 1, fp); 
	fread(&reserved2, sizeof(unsigned short int), 1, fp); 
	fread(&offset, sizeof(unsigned int), 1, fp);
	
	fread(&header_size, sizeof(unsigned int), 1, fp); 
	fread(&width, sizeof(int), 1, fp);
	fread(&height, sizeof(int), 1, fp);	
	fread(&planes, sizeof(unsigned short int), 1, fp); 
	fread(&bits, sizeof(unsigned short int), 1, fp); 
	fread(&compression, sizeof(unsigned int), 1, fp); 
	fread(&imagesize, sizeof(unsigned int), 1, fp); 
	fread(&hresolution, sizeof(int), 1, fp); 
	fread(&vresolution, sizeof(int), 1, fp); 
	fread(&ncolors, sizeof(unsigned int), 1, fp); 
	fread(&importantcolors, sizeof(unsigned int), 1, fp);
	
	size=widthbytes(bits * width); 
	graysize = widthbytes(8 * width);
	
	if(!imagesize) 
		imagesize=height*size;
	
	inimg=(BYTE*)malloc(sizeof(BYTE)*imagesize); 
	outimg=(BYTE*)malloc(sizeof(BYTE)*imagesize); 
	fread(inimg, sizeof(BYTE), imagesize, fp); 
	
	fclose(fp);

	int count = 0;
	scanf("%d", &count);

	srand((unsigned int)time(NULL));
	
	for(i=0; i<height*3; i+=3) { 
		for(j=0; j<width*3; j+=3) {
	
			int b = inimg[j+(i*width+0)]; 
			int g = inimg[j+(i*width+1)]; 
			int r = inimg[j+(i*width+2)];

			/*if(j == rand()%(width*3+1) || i == rand()%(height*3+1)){
				b += 255; //0~255까지의 숫자만 출력 (255-0+1)+0
				g += 255;
				r += 255; 
			}*/
						
			outimg[j+width*i+0]= clip(b, 0, 255);
			outimg[j+width*i+1]= clip(g, 0, 255);
			outimg[j+width*i+2]= clip(r, 0, 255);
		};
	 };	   
	
	//화이트 노이즈 생성 부분
	for(int x = 0; x < count; x++){
		int	j = rand()%(width*3+1);
		int i = rand()%(height*3+1);
						
		outimg[j+width*i+0]= 255; //clip(b, 0, 255);
		outimg[j+width*i+1]= 255; //clip(g, 0, 255);
		outimg[j+width*i+2]= 255; //clip(r, 0, 255);
	}


	size=widthbytes(bits*width); 
	imagesize=height*size; 
	offset+=256*sizeof(RGBQUAD);
	
	if((fp=fopen(output, "wb"))==NULL) { 
		fprintf(stderr, "Error : Failed to open file...₩n"); 
		return -1;
	}
	
	fwrite(&type, sizeof(unsigned short int), 1, fp); 
	fwrite(&file_size, sizeof(unsigned int), 1, fp); 
	fwrite(&reserved1, sizeof(unsigned short int), 1, fp); 
	fwrite(&reserved2, sizeof(unsigned short int), 1, fp); 
	fwrite(&offset, sizeof(unsigned int), 1, fp); 
	
	fwrite(&header_size, sizeof(unsigned int), 1, fp); 
	fwrite(&width, sizeof(int), 1, fp);
	fwrite(&height, sizeof(int), 1, fp);
	fwrite(&planes, sizeof(unsigned short int), 1, fp); 
	fwrite(&bits, sizeof(unsigned short int), 1, fp);		
	fwrite(&compression, sizeof(unsigned int), 1, fp); 
	fwrite(&imagesize, sizeof(unsigned int), 1, fp); 
	fwrite(&hresolution, sizeof(int), 1, fp); 
	fwrite(&vresolution, sizeof(int), 1, fp); 
	fwrite(&ncolors, sizeof(unsigned int), 1, fp); 
	fwrite(&importantcolors, sizeof(unsigned int), 1, fp);
	fwrite(palrgb, sizeof(unsigned int), 256, fp); 
	
	fwrite(outimg, sizeof(unsigned char), imagesize, fp);
	
	free(inimg); 
	free(outimg);
	
	fclose(fp); 
	
	return 0;
}
