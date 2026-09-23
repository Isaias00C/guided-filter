#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include "common.h"

void boxfilter(float *, int, int, int, float *);
void unitfilter(int, int, float *);
void matDiff(float* one, float* other, int rows, int cols, float* result);
void repmat(float* image, int rows, int cols, int repRows, int repCols, float* result);
void printMat(float* mat, int rows, int cols);
void mean(float* value, float* total,int rows, int cols, float* result);
void imagemultiplication(float* one, float* other, int rows, int cols, float* result);
void calculatevariance(float* mean1, float* mean2,float* mean3,int rows, int cols, float* result);
void calculatea(float* value1, float* value2, float eps, int rows, int cols, float* result);
void calculateoutput(float* meancoeffa, float* image, float* meancoeffb, int rows, int cols, float* result);
void printMat2(unsigned char* mat, int rows, int cols);

void guidedFilter(float * guidance, float * src,float * dest,int radius,float eps, int rows, int cols)
{
  static float mean_I[MAX_PIXELS], mean_p[MAX_PIXELS], mean_Ip[MAX_PIXELS], cov_Ip[MAX_PIXELS], mean_II[MAX_PIXELS], var_I[MAX_PIXELS];
  static float distance[MAX_PIXELS]; // distance of local patch
  static float guidanceimdistance[MAX_PIXELS]; // distance of guidance image
  static float filterimdistance[MAX_PIXELS]; // distance of guidance image
  static float Ipimdistance[MAX_PIXELS]; // distance of I*p image
  static float IIimdistance[MAX_PIXELS]; // distance of I*I
  static float coeffadistance[MAX_PIXELS]; // distance of coeffa
  static float coeffbdistance[MAX_PIXELS]; // distance of coeffb
  static float coeffa[MAX_PIXELS], coeffb[MAX_PIXELS], mean_coeffa[MAX_PIXELS], mean_coeffb[MAX_PIXELS];
  static float unf[MAX_PIXELS];
  static float improduct[MAX_PIXELS];
  static float imsquareproduct[MAX_PIXELS];

  if ((long)rows*cols > MAX_PIXELS) {
    printf("guidedFilter: image %dx%d exceeds MAX_PIXELS (%d), raise MAX_ROWS/MAX_COLS in common.h\n", rows, cols, MAX_PIXELS);
    exit(0);
  }

  unitfilter(rows, cols, unf);
  //printf("\Printing unit filter\n");
  //printMat(unf,rows,cols);

  /*N = boxfilter(ones(hei, wid), r);*/
  boxfilter(unf, rows, cols, radius, distance);
  //printf("\Printing patch distance\n");
  //printMat(distance,rows,cols);

  /* Mean of guidance image: mean_I = boxfilter(I, r) ./ N;*/
  boxfilter(guidance, rows, cols, radius, guidanceimdistance);
  //printf("\Printing guidanceimdistance\n");
  //printMat(guidanceimdistance,rows,cols);

  mean(guidanceimdistance,distance,rows,cols,mean_I);
  //printf("\nPrinting mean\n");
  //printMat(mean_I,rows,cols);

  /* Mean of filtering input image: mean_p = boxfilter(p, r) ./ N;*/
  boxfilter(src, rows, cols, radius, filterimdistance);
  //printf("\Printing filterimdistance\n");
  //printMat(filterimdistance,rows,cols);

  mean(filterimdistance,distance,rows,cols,mean_p);
  //printf("\nPrinting mean p\n");
  //printMat(mean_p,rows,cols);

  /* Mean of I*p: mean_Ip = boxfilter(I.*p, r) ./ N;*/
  imagemultiplication(guidance,guidance,rows,cols,improduct);
  //printf("\nPrinting Ip\n");
  //printMat(improduct,rows,cols);

  boxfilter(improduct, rows, cols, radius, Ipimdistance);
  //printf("\nPrinting Ipimdistance\n");
  //printMat(Ipimdistance,rows,cols);

  mean(Ipimdistance,distance,rows,cols,mean_Ip);
  //printf("\nPrinting mean\n");
  //printMat(mean_Ip,rows,cols);

  //cov_Ip = mean_Ip - mean_I .* mean_p;% this is the covariance of (I, p) in each local
  calculatevariance(mean_Ip,mean_I,mean_p,rows,cols,cov_Ip);
  //printf("\nPrinting cov_Ip\n");
  //printMat(cov_Ip,rows,cols);

  /* mean_II = boxfilter(I.*I, r) ./ N;*/
  imagemultiplication(guidance,guidance,rows,cols,imsquareproduct);
  //printf("\nPrinting I2\n");
  //printMat(imsquareproduct,rows,cols);

  boxfilter(imsquareproduct, rows, cols, radius, IIimdistance);

  //printf("\nPrinting IIimdistance\n");
  //printMat(IIimdistance,rows,cols);

  mean(IIimdistance,distance,rows,cols,mean_II);
  //printf("\nPrinting mean\n");
  //printMat(mean_II,rows,cols);

  /*var_I = mean_II - mean_I .* mean_I;*/
  calculatevariance(mean_II,mean_I,mean_I,rows,cols,var_I);
  //printf("\nPrinting var_I\n");
  //printMat(var_I,rows,cols);

  /*a = cov_Ip ./ (var_I + eps); % Eqn. (5) in the paper;*/
  calculatea(cov_Ip,var_I,eps,rows,cols,coeffa);

  /*b = mean_p - a .* mean_I; % Eqn. (6) in the paper;*/
  calculatevariance(mean_p,coeffa,mean_I,rows,cols,coeffb);

  /*mean_a = boxfilter(a, r) ./ N;*/
  boxfilter(coeffa, rows, cols, radius, coeffadistance);

  mean(coeffadistance,distance,rows,cols,mean_coeffa);


  /*mean_b = boxfilter(b, r) ./ N;*/
  boxfilter(coeffb, rows, cols, radius, coeffbdistance);

  mean(coeffbdistance,distance,rows,cols,mean_coeffb);

  /*q = mean_a .* I + mean_b; % Eqn. (8) in the paper;*/
  calculateoutput(mean_coeffa,guidance,mean_coeffb,rows,cols,dest);
}



void unitfilter(int rows, int cols, float *unf)
{
  int k,l;
  float value = 1.0;

  for(k=0; k<rows; k++)
	{
		for(l=0; l<cols; l++)
			{
				unf[k*cols+l] = value;
			}
	}
}


void boxfilter(float *image,  int M, int N, int radius, float *distance)
{
  int m, n, i, j;
  static float cumdistance[MAX_PIXELS];
  static float cumheightvector[MAX_COLS]; //1-D, After repmat will become 2-D
  static float cumheightvectorcol[MAX_ROWS];
  static float cumheightvector2d[MAX_PIXELS];
  static float cumheightvector2dcol[MAX_PIXELS];
  static float cumdistancevector[MAX_PIXELS];
  static float cumdistancevectorcol[MAX_PIXELS];
  static float cumheightdiffdistance[MAX_PIXELS]; //cumheightvector2d - cumheightvector
  static float cumheightdiffdistancecol[MAX_PIXELS];

 // Cumulative SUM over Y axis
   for(n = 0; n <N; n++)
      {
		  for(m = 0; m < M; m++)
         	{
				if(m==0)
				{
					cumdistance[m*N+ n]= (float) image[m*N + n];
				}
				else
				{
					cumdistance[m*N+ n] = (float) image[m*N + n] + (float) cumdistance[(m-1)*N+n];
				}
		  }
   }


   // Difference over Y axis

   for(m = 0; m <=radius; m++)
      {
		  for(n = 0; n < N; n++)
			{
				distance[m*N+ n] = (float) cumdistance[(m+radius)*N+ n];
		  }
   }

   for(m = radius+1; m <= M-(radius+1); m++)
      {
		  for(n = 0; n < N; n++)
			{
				//imDst(r+2:hei-r, :) = imCum(2*r+2:hei, :) - imCum(1:hei-2*r-1, :);
				distance[m*N+ n] = (float) cumdistance[(m+radius)*N + n]-(float) cumdistance[(m-radius-1)*N+ n];
		  }
   }

 //imDst(hei-r+1:hei, :) = repmat(imCum(hei, :), [r, 1]) - imCum(hei-2*r:hei-r-1, :);
   for(n=0; n<N; n++) {
	   cumheightvector[n] = cumdistance[(M-1)*N + n];
   }

   for (i=0,m=M-1-2*radius; m<=M-1-radius-1; m++,i++) {
	   for (j=0,n=0; n<N; n++,j++) {
			cumdistancevector[i*N+j] = cumdistance[m*N+n];
	   }
   }

   repmat(cumheightvector,1,N,radius,1,cumheightvector2d);
   matDiff(cumheightvector2d, cumdistancevector, radius, N, cumheightdiffdistance);
   //printf("\nPrinting Cumheightdiffdistance\n");
   //printMat(cumheightdiffdistance,M,N);
  for(i=0,m = M-radius; m < M; m++,i++)
      {
		  for(j=0,n = 0; n < N; n++,j++)
			{
				distance[m*N+n] = cumheightdiffdistance[i*N+j];
		  }
   }

  //Cumulative SUM over X axis
	for(m = 0; m <M; m++)
      {
		  for(n = 0; n < N; n++)
         	{
				if(n==0)
				{
					cumdistance[m*N+ n]= (float) distance[m*N + n];
				}
				else
				{
					cumdistance[m*N+ n] = (float) distance[m*N+ n] + cumdistance[m*N+(n-1)];
				}
		  }
   }

// Difference over X axis
//imDst(:, 1:r+1) = imCum(:, 1+r:2*r+1)
 for(m = 0; m <M; m++)
      {
		  for(n = 0; n <= radius; n++)
         	{
					distance[m*N+ n] = (float) cumdistance[m*N+(n+radius)];
		  }
	}

 //imDst(:, r+2:wid-r) = imCum(:, 2*r+2:wid) - imCum(:, 1:wid-2*r-1);

 for(m = 0; m <M; m++)
      {
		  for(n = radius+1; n <= N-radius-1; n++)
         	{
					distance[m*N+ n] = (float) cumdistance[m*N+(n+radius)]-(float) cumdistance[m*N+(n-radius-1)];
		  }
   }

//imDst(:, wid-r+1:wid) = repmat(imCum(:, wid), [1, r]) - imCum(:, wid-2*r:wid-r-1);

  for(m=0; m<M; m++) {
	  	   cumheightvectorcol[m] = cumdistance[m*N + N-1];
  }
  //printf("\n Printing cumheightvectorcol \n");
  //printMat(cumheightvectorcol,M,1);

   for (i=0,m=0; m<M; m++,i++) {
	   for (j=0,n=N-1-2*radius; n<=N-1-radius-1; n++,j++) {
					cumdistancevectorcol[i*radius+j] = (float) cumdistance[m*N+n];
		  }
   }

   repmat(cumheightvectorcol,M,1,1,radius,cumheightvector2dcol); //M*radius
   matDiff(cumheightvector2dcol, cumdistancevectorcol, M, radius, cumheightdiffdistancecol);

   for(i=0,m = 0; m <M; m++,i++)
      {
		  for(j=0,n = N-radius; n <N; n++,j++)
         	{
				distance[m*N+n] = cumheightdiffdistancecol[i*radius+j];
		  }
   }
}

//								M		  1			1			 radius=2
 void repmat(float* image, int rows, int cols, int repRows, int repCols, float* result)
  {
	int totRows = rows * repRows;//M
	int totCols = cols * repCols;//2
	int r,c, ro, co;
	float val;
	//Loop thru rows and cols of given image
	for (r=0; r<rows; r++) {
		for (c=0; c<cols; c++) {
			val = image[r*cols+c];
			//Loop thru rows and cols of the new image and replicate the value
			for (ro=r; ro<totRows; ro=ro+rows){
				for (co=c; co<totCols; co=co+cols){
					result[ro*totCols+co] = val;
				}
			}
		}
	}
 }

void imagemultiplication(float* one, float* other, int rows, int cols, float* result) {
	int i,j;
	for (j=0; j<rows; j++) {
		for (i=0; i<cols; i++){
			result[j*cols+i] = one[j*cols+i]*other[j*cols+i];
		}
	}
}

void matDiff(float* one, float* other, int rows, int cols, float* result) {
	int i,j;
	for (j=0; j<rows; j++) {
		for (i=0; i<cols; i++){
			result[j*cols+i] = one[j*cols+i] - other[j*cols+i];
		}
	}
}

void mean(float* value, float* total,int rows, int cols, float* result) {
	int i,j;
	for (j=0; j<rows; j++) {
		for (i=0; i<cols; i++){
			result[j*cols+i] = value[j*cols+i] /total[j*cols+i];
		}
	}
}

void calculatevariance(float* mean1, float* mean2,float* mean3,int rows, int cols, float* result) {
	int i,j;
	for (j=0; j<rows; j++) {
		for (i=0; i<cols; i++){
			result[j*cols+i] = mean1[j*cols+i] - mean2[j*cols+i]*mean3[j*cols+i];
		}
	}
}

void calculatea(float* value1, float* value2, float eps, int rows, int cols, float* result){
	int i,j;
	for (j=0; j<rows; j++) {
		for (i=0; i<cols; i++){
			result[j*cols+i] = value1[j*cols+i]/(value2[j*cols+i]+eps);
		}
	}
}

void calculateoutput(float* meancoeffa, float* image, float* meancoeffb, int rows, int cols, float* result){
	int i,j;
	for (j=0; j<rows; j++) {
		for (i=0; i<cols; i++){
			result[j*cols+i] = meancoeffa[j*cols+i]*(float)image[j*cols+i]+meancoeffb[j*cols+i];
		}
	}
}


void printMat2(unsigned char* mat, int rows, int cols) {
	int i,j;
	for (j=0; j<rows; j++) {
		for (i=0; i<cols; i++){
			printf("%5d  ", (int) mat[j*cols+i]);
		}
		printf("\n");
	}
}
