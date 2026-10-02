#include <stdint.h>
#include "common.h"

static int clampInt(int value, int min, int max)
{
    if (value < min)
        return min;

    if (value > max)
        return max;

    return value;
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
    int iy, ix;

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
             * O Guided Filter calcula a média dos
             * coeficientes das janelas que contêm
             * o pixel atual (y,x).
             */
            int wy_start =
                clampInt(y - radius, 0, rows - 1);

            int wy_end =
                clampInt(y + radius, 0, rows - 1);

            int wx_start =
                clampInt(x - radius, 0, cols - 1);

            int wx_end =
                clampInt(x + radius, 0, cols - 1);


            /*
             * Percorre as janelas que contêm o pixel.
             */
            for (wy = wy_start;
                 wy <= wy_end;
                 wy++) {

                for (wx = wx_start;
                     wx <= wx_end;
                     wx++) {

                    uint32_t sum = 0;
                    uint32_t sum_sq = 0;
                    uint32_t count = 0;


                    /*
                     * Limites da janela centrada em
                     * (wy,wx).
                     *
                     * Nas bordas, a janela é truncada,
                     * da mesma maneira que o boxfilter()
                     * original.
                     */
                    int y0 =
                        clampInt(wy - radius, 0, rows - 1);

                    int y1 =
                        clampInt(wy + radius, 0, rows - 1);

                    int x0 =
                        clampInt(wx - radius, 0, cols - 1);

                    int x1 =
                        clampInt(wx + radius, 0, cols - 1);


                    /*
                     * Calcula:
                     *
                     * sum(I)
                     * sum(I²)
                     *
                     * para a janela.
                     */
                    for (iy = y0;
                         iy <= y1;
                         iy++) {

                        for (ix = x0;
                             ix <= x1;
                             ix++) {

                            uint8_t pixel =
                                src[iy * cols + ix];

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
                     * var(I) =
                     * E(I²) - E(I)²
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
             * Conversão para uint8_t com arredondamento.
             *
             * result já está saturado em [0,255], então
             * result + 0.5 equivale a round(result).
             */
            dest[y * cols + x] =
                (uint8_t)(result + 0.5);
        }
    }

    return 0;
}