
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common.h"


void ReadPGM(FILE* fp, unsigned char *image, int *rows, int *cols)
{
  int c;
  int i,j;
  int xdim, ydim;
  int val;
  int maxraw;
  char buf[1024];


  while ((c=fgetc(fp)) == '#')
      fgets(buf, 1024, fp);
   ungetc(c, fp);
   if (fscanf(fp, "P%d\n", &c) != 1) {
     printf ("read error ....");
     exit(0);
   }
   if (c != 5 && c != 2) {
     printf ("read error ....");
     exit(0);
   }

   if (c==5) {
     while ((c=fgetc(fp)) == '#')
       fgets(buf, 1024, fp);
     ungetc(c, fp);
     if (fscanf(fp, "%d%d%d",&xdim, &ydim, &maxraw) != 3) {
       printf("failed to read width/height/max\n");
       exit(0);
     }
     printf("Width=%d, Height=%d \nMaximum=%d\n",xdim,ydim,maxraw);

     if ((long)xdim*ydim > MAX_PIXELS) {
       printf("image %dx%d exceeds MAX_PIXELS (%d), raise MAX_ROWS/MAX_COLS in common.h\n", xdim, ydim, MAX_PIXELS);
       exit(0);
     }

     getc(fp);

     for (j=0; j<ydim; j++) {
       fread(image + j*xdim, 1, xdim, fp);
     }

   }

   else if (c==2) {
     while ((c=fgetc(fp)) == '#')
       fgets(buf, 1024, fp);
     ungetc(c, fp);
     if (fscanf(fp, "%d%d%d", &xdim, &ydim, &maxraw) != 3) {
       printf("failed to read width/height/max\n");
       exit(0);
     }
     printf("Width=%d, Height=%d \nMaximum=%d,\n",xdim,ydim,maxraw);

     if ((long)xdim*ydim > MAX_PIXELS) {
       printf("image %dx%d exceeds MAX_PIXELS (%d), raise MAX_ROWS/MAX_COLS in common.h\n", xdim, ydim, MAX_PIXELS);
       exit(0);
     }

     getc(fp);

     for (j=0; j<ydim; j++)
       for (i=0; i<xdim; i++) {
	 fscanf(fp, "%d", &val);
	 image[j*xdim+i] = val;
       }

   }


   *cols = xdim;
   *rows = ydim;

   fclose(fp);

}


void WritePGM(int rows, int cols, unsigned char *image,
	       unsigned char *result, FILE* fp)
{
  int i, j;




  fprintf(fp, "P5\n%d %d\n%d\n", cols, rows, 255);

  for (j=0; j<rows; j++)
    for (i=0; i<cols; i++)
      fputc(result[j*cols+i], fp);
  	 // printf("%u",result);
  fclose(fp);

}
