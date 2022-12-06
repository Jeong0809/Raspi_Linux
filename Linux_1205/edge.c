/* bitoffset을 +=256 하는 이유는 인덱스드 컬러를 사용하지 않기 때문에 RGBQUAD 만큼 밀어줘야 시작 위치가 제대로 찾아진다 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmpHeader3.h"

#define BYTE   unsigned char
#define BASE    16

#define widthbytes(bits)   (((bits)+31)/32*4)

int main(int argc, char** argv)
{
   FILE *fp;
   RGBQUAD palrgb[256];
    BITMAPFILEHEADER bmpFileHeader;
    BITMAPINFOHEADER bmpInfoHeader;
    unsigned int imagesize;

   float r, g, b, gray1, gray2;
 
   int i, j, size, index, value;
   unsigned long histogram[256];
   unsigned char *inimg;
   unsigned char *outimg;
   char input[128], output[128];
 
   strcpy(input, argv[1]);
   strcpy(output, argv[2]);
 
   if((fp = fopen(input, "rb")) == NULL) {
      fprintf(stderr, "Error : Failed to open file...\n");
      exit(EXIT_FAILURE);
   }

   fread(&bmpFileHeader,sizeof(bmpFileHeader), 1, fp);
   fread(&bmpInfoHeader,sizeof(bmpInfoHeader), 1, fp);
   size = widthbytes(bmpInfoHeader.biBitCount * bmpInfoHeader.biWidth);

   if(!imagesize) imagesize = bmpInfoHeader.biHeight * size;
   inimg = (BYTE*)malloc(sizeof(BYTE)*imagesize);
   outimg = (BYTE*)malloc(sizeof(BYTE)*imagesize);
   fread(inimg, sizeof(BYTE), imagesize, fp);
   fclose(fp);
 
    printf("Image width : %d, height : %d(%d)\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight, bmpInfoHeader.biWidth*bmpInfoHeader.biHeight);
   
   for(i = 0; i < 256; i++) histogram[i] = 0; // memset
 
   for(i = 0; i < bmpInfoHeader.biHeight; i++) {
      index = (bmpInfoHeader.biHeight-i-1) * size; 
      for(j = 0 ; j < bmpInfoHeader.biWidth; j++) { 
         r = (float)inimg[index+3*j+2];
         g = (float)inimg[index+3*j+1];
         b = (float)inimg[index+3*j+0];
         gray1 = (r*0.3F)+(g*0.59F)+(b*0.11F); // grayscale

		 r = (float)inimg[index+3*j+2-3];
         g = (float)inimg[index+3*j+1-3];
         b = (float)inimg[index+3*j+0-3];
         gray2 = (r*0.3F)+(g*0.59F)+(b*0.11F); // grayscale
		
		 value = (int)(gray2 - gray1);
		 outimg[index+3*j] = (value > BASE)?255:0;
		 outimg[index+3*j+1] = (value > BASE)?255:0;
		 outimg[index+3*j+2] = (value > BASE)?255:0;

      };
   };

    /*for(i = 0; i < 256; i++) {
      printf("%-3d  ", i);
      if(!(int)(histogram[i]/bmpInfoHeader.biHeight) && (histogram[i]%bmpInfoHeader.biHeight))
         printf("*");
      else for(j = 0; j < (int)(histogram[i]/bmpInfoHeader.biHeight); j++)
         printf("*");
      printf("  %d\n", histogram[i]);
    };*/
   
   bmpFileHeader.bf0ffBits += 256*sizeof(RGBQUAD); 

   if((fp = fopen(output, "wb")) == NULL) {
      fprintf(stderr, "Error : Failed to open file...\n");
      exit(EXIT_FAILURE);
   }
 
   fwrite(&bmpFileHeader, sizeof(bmpFileHeader), 1, fp);
   fwrite(&bmpInfoHeader, sizeof(bmpInfoHeader), 1, fp);
   fwrite(palrgb, sizeof(unsigned int), 256, fp);
   fwrite(outimg, sizeof(unsigned char), imagesize, fp);

   free(inimg);
   free(outimg);

   fclose(fp);

        return 0;
}
