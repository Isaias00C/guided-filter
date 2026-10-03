
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common.h"


/*
 * ReadPGM: lê uma imagem PGM (formato P5 binário ou P2 ASCII) de um arquivo.
 *
 *   fp    - arquivo já aberto para leitura (é fechado ao final da função)
 *   image - buffer de saída (rows*cols bytes), alocado pelo chamador
 *   rows  - recebe a altura da imagem
 *   cols  - recebe a largura da imagem
 *
 * Encerra o programa se o cabeçalho for inválido ou se a imagem
 * exceder os limites definidos em common.h.
 */
void ReadPGM(FILE* fp, unsigned char *image, int *rows, int *cols)
{
  int c;
  int i,j;
  int xdim, ydim;
  int val;
  int maxraw;
  char buf[1024];


  /* Ignora linhas de comentário (iniciadas por '#') antes do número mágico */
  while ((c=fgetc(fp)) == '#')
      fgets(buf, 1024, fp);
   ungetc(c, fp);
   /* Lê o número mágico: P5 (binário) ou P2 (ASCII) */
   if (fscanf(fp, "P%d\n", &c) != 1) {
     printf ("read error ....");
     exit(0);
   }
   if (c != 5 && c != 2) {
     printf ("read error ....");
     exit(0);
   }

   if (c==5) {
     /* PGM binário: ignora comentários e lê largura, altura e valor máximo */
     while ((c=fgetc(fp)) == '#')
       fgets(buf, 1024, fp);
     ungetc(c, fp);
     if (fscanf(fp, "%d%d%d",&xdim, &ydim, &maxraw) != 3) {
       printf("failed to read width/height/max\n");
       exit(0);
     }
     printf("Width=%d, Height=%d \nMaximum=%d\n",xdim,ydim,maxraw);

     /* Verifica se a imagem cabe nos buffers estáticos */
     if (ydim > MAX_ROWS || xdim > MAX_COLS || (long)xdim*ydim > MAX_PIXELS) {
       printf("image %dx%d exceeds MAX_ROWS/MAX_COLS/MAX_PIXELS (%d/%d/%d), raise them in common.h\n", xdim, ydim, MAX_ROWS, MAX_COLS, MAX_PIXELS);
       exit(0);
     }

     /* Consome o caractere de espaço que separa o cabeçalho dos dados */
     getc(fp);

     /* Lê os pixels linha a linha (1 byte por pixel) */
     for (j=0; j<ydim; j++) {
       fread(image + j*xdim, 1, xdim, fp);
     }

   }

   else if (c==2) {
     /* PGM ASCII: mesmo cabeçalho, mas os pixels são números em texto */
     while ((c=fgetc(fp)) == '#')
       fgets(buf, 1024, fp);
     ungetc(c, fp);
     if (fscanf(fp, "%d%d%d", &xdim, &ydim, &maxraw) != 3) {
       printf("failed to read width/height/max\n");
       exit(0);
     }
     printf("Width=%d, Height=%d \nMaximum=%d,\n",xdim,ydim,maxraw);

     /* Verifica se a imagem cabe nos buffers estáticos */
     if (ydim > MAX_ROWS || xdim > MAX_COLS || (long)xdim*ydim > MAX_PIXELS) {
       printf("image %dx%d exceeds MAX_ROWS/MAX_COLS/MAX_PIXELS (%d/%d/%d), raise them in common.h\n", xdim, ydim, MAX_ROWS, MAX_COLS, MAX_PIXELS);
       exit(0);
     }

     /* Consome o caractere que separa o cabeçalho dos dados */
     getc(fp);

     /* Lê cada pixel como inteiro e armazena em ordem linha-a-linha */
     for (j=0; j<ydim; j++)
       for (i=0; i<xdim; i++) {
	 fscanf(fp, "%d", &val);
	 image[j*xdim+i] = val;
       }

   }


   /* Devolve as dimensões ao chamador */
   *cols = xdim;
   *rows = ydim;

   fclose(fp);

}


/*
 * WritePGM: grava a imagem em `result` no formato PGM ASCII (P2).
 *
 *   rows, cols - dimensões da imagem
 *   image      - imagem original (não utilizada nesta função)
 *   result     - pixels a serem gravados (rows*cols bytes)
 *   fp         - arquivo já aberto para escrita
 */
void WritePGM(int rows, int cols, unsigned char *image,
	       unsigned char *result, FILE* fp)
{
  int i, j;




  /* Cabeçalho: número mágico, largura/altura e valor máximo (255) */
  fprintf(fp, "P2\n%d %d\n%d\n", cols, rows, 255);

  /* Escreve os pixels como texto, uma linha da imagem por linha do arquivo */
  for (j=0; j<rows; j++) {
    for (i=0; i<cols; i++) {
      fprintf(fp, "%u ", (unsigned int)result[j*cols+i]);
    }
    fprintf(fp, "\n");
  }

}
