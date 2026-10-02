#include <stdint.h>
#include "common.h"

static int reflectIndex(int index, int length)
{
    if (length <= 1)
        return 0;

    while (index < 0 || index >= length) {
        if (index < 0)
            index = -index - 1;
        else
            index = 2 * length - index - 1;
    }

    return index;
}


int guidedFilterSelf(const uint8_t *src,
                     uint8_t *dest,
                     int radius,
                     double eps,
                     int rows,
                     int cols)
{
    int y, x;
    int wy, wx;
    int dy, dx;

    /*
     * Verificação das dimensões.
     */
    if (rows <= 0 || cols <= 0)
        return -1;

    if (rows > MAX_ROWS || cols > MAX_COLS)
        return -2;

    if (radius < 0)
        return -3;

    if (radius >= rows || radius >= cols)
        return -4;


    /*
     * O código original trabalhava com a imagem
     * normalizada em [0,1].
     *
     * Como agora trabalhamos diretamente em
     * pixels [0,255], transformamos epsilon.
     */
    double eps8 = eps * 255.0f * 255.0f;


    /*
     * Processa cada pixel da imagem de saída.
     */
    for (y = 0; y < rows; y++) {

        for (x = 0; x < cols; x++) {

            double sum_a = 0.0f;
            double sum_b = 0.0f;
            uint32_t window_count = 0;


            /*
             * Cada janela tem tamanho fixo
             * (2 * radius + 1) x (2 * radius + 1).
             * Centros fora da imagem são refletidos,
             * incluindo a repetição dos pixels de borda
             * (equivalente a BORDER_REFLECT do OpenCV).
             */
            for (wy = y - radius;
                 wy <= y + radius;
                 wy++) {

                int center_y = reflectIndex(wy, rows);

                for (wx = x - radius;
                     wx <= x + radius;
                     wx++) {

                    int center_x = reflectIndex(wx, cols);
                    uint32_t sum = 0;
                    uint32_t sum_sq = 0;
                    uint32_t count = 0;


                    /*
                     * Calcula sum(I) e sum(I²) na janela
                     * centrada em (center_y, center_x).
                     * Cada coordenada fora da imagem é
                     * mapeada por reflexão.
                     */
                    for (dy = -radius;
                         dy <= radius;
                         dy++) {

                        int iy = reflectIndex(center_y + dy, rows);

                        for (dx = -radius;
                             dx <= radius;
                             dx++) {

                            int ix = reflectIndex(center_x + dx, cols);
                            uint8_t pixel = src[iy * cols + ix];

                            sum += pixel;

                            sum_sq +=
                                (uint32_t)pixel *
                                (uint32_t)pixel;

                            count++;
                        }
                    }


                    /*
                     * Média local:
                     *
                     * mu = sum(I) / N
                     */
                    double mean =
                        (double)sum / (double)count;


                    /*
                     * Variância local:
                     *
                     * var(I) = E(I²) - E(I)²
                     */
                    double variance =
                        ((double)sum_sq / (double)count)
                        - (mean * mean);


                    /*
                     * Pequenos erros numéricos podem
                     * produzir uma variância negativa
                     * muito próxima de zero.
                     */
                    if (variance < 0.0f)
                        variance = 0.0f;


                    /*
                     * Self-Guided Filter:
                     *
                     * a = var / (var + eps)
                     */
                    double a =
                        variance /
                        (variance + eps8);


                    /*
                     * Como I = p:
                     *
                     * b = mean * (1 - a)
                     */
                    double b =
                        mean * (1.0f - a);


                    /*
                     * Acumula os coeficientes.
                     */
                    sum_a += a;
                    sum_b += b;

                    window_count++;
                }
            }


            /*
             * Média dos coeficientes das janelas.
             */
            double mean_a =
                sum_a / (double)window_count;

            double mean_b =
                sum_b / (double)window_count;


            /*
             * Equação final:
             *
             * q = mean_a * I + mean_b
             */
            double result =
                mean_a *
                (double)src[y * cols + x]
                + mean_b;


            /*
             * Saturação para [0,255].
             */
            if (result < 0.0f)
                result = 0.0f;

            if (result > 255.0f)
                result = 255.0f;


            /*
             * Conversão para uint8_t.
             *
             * Mantém a lógica de truncamento do
             * seu main original.
             */
            dest[y * cols + x] =
                (uint8_t)result;
        }
    }

    return 0;
}