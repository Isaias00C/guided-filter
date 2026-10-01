
/* Guided Image Filtering by by Kaiming He, Jian Sun, and Xiaoou Tang, in ECCV 2010 (Oral)

%   GUIDEDFILTER   O(1) time implementation of guided filter
%
%   - guidance image: I (should be a gray-scale/single channel image)
%   - filtering input image: p (should be a gray-scale/single channel image)
%   - local window radius: r
%   - regularization parameter: eps

*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common.h"
#include "image_flash.h"
#include <stdint.h>


// void ReadPGM(FILE* , unsigned char* , int* , int* );
// void WritePGM(int, int, unsigned char *, unsigned char *, FILE*);
// void printMat2(unsigned char* mat, int rows, int cols);
// void printMat(float* mat, int rows, int cols);
int guidedFilterSelf(const uint8_t *src,
                     uint8_t *dest,
                     int radius,
                     float eps,
                     int rows,
                     int cols);

uint8_t input[45][45];
uint8_t output[45][45];


int main(void)
{
    int y, x;

    /*
     * Copia a imagem armazenada na Flash
     * para a matriz de entrada na RAM.
     */
    for (y = 0; y < MAX_ROWS; y++) {
        for (x = 0; x < MAX_COLS; x++) {
            input[y][x] =
                image_flash[y * MAX_COLS + x];
        }
    }

    /*
     * Self-Guided Filter:
     *
     * A mesma imagem é usada como:
     *   I = imagem guia
     *   p = imagem de entrada
     *
     * radius = 2
     *
     * eps = 0.01 no domínio normalizado [0,1].
     */
    if (guidedFilterSelf(
            &input[0][0],
            &output[0][0],
            2,
            0.01f,
            MAX_ROWS,
            MAX_COLS) != 0) {

        return 1;
    }

    /*
     * A matriz output agora contém
     * a imagem filtrada.
     *
     * Em um embarcado real, o resultado
     * pode ser enviado por UART, SPI,
     * armazenado externamente etc.
     */

    return 0;
}



