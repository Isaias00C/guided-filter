/*
 * mainTest.c
 *
 * Programa de teste do self-guided filter (filtro guiado em que a imagem
 * de guia é a própria imagem de entrada, o que resulta em um filtro de
 * suavização que preserva bordas).
 *
 * Fluxo geral:
 *   1. Lê uma imagem PGM (45x45) passada pela linha de comando.
 *   2. Aplica o guidedFilterSelf com os parâmetros radius e eps.
 *   3. Grava a imagem filtrada em PGM (P2) na mesma pasta do executável.
 *
 * Uso:
 *   programa.exe <imagem_entrada.pgm> <imagem_saida.pgm>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>   /* GetModuleFileNameA e MAX_PATH (específicos do Windows) */

#include "common.h"    /* MAX_ROWS, MAX_COLS e MAX_PIXELS */

/* Funções já existentes no seu projeto (implementadas em inout.c) */

/* Lê um PGM (P5 ou P2) de fp para o buffer image e devolve altura/largura */
void ReadPGM(FILE *fp, unsigned char *image, int *rows, int *cols);

/* Grava o buffer output como PGM ASCII (P2) em fp */
void WritePGM(int rows, int cols,
              unsigned char *input,
              unsigned char *output,
              FILE *fp);

/* Novo self-guided filter
 *   src    - imagem de entrada (também usada como guia)
 *   dest   - imagem filtrada (saída)
 *   radius - raio da janela do filtro (janela de lado 2*radius+1)
 *   eps    - fator de regularização: valores maiores suavizam mais,
 *            valores menores preservam mais as bordas
 *   rows, cols - dimensões da imagem
 * Retorna 0 em caso de sucesso e valor diferente de zero em caso de erro.
 */
int guidedFilterSelf(const uint8_t *src,
                     uint8_t *dest,
                     int radius,
                     double eps,
                     int rows,
                     int cols);

int main(int argc, char *argv[])
{
    FILE *fp;
    int rows, cols;

    /* Parâmetros do filtro (fixos neste teste) */
    int radius = 2;        /* janela de 5x5 pixels */
    double eps = 0.04f;    /* regularização; imagem é normalizada internamente pelo filtro */

    /* Buffers estáticos (sem malloc), dimensionados pelos limites de common.h.
       Ficam em memória estática para não estourar a pilha. */
    static uint8_t input[MAX_PIXELS];
    static uint8_t output[MAX_PIXELS];

    char exePath[MAX_PATH];     /* caminho completo do executável (depois, só a pasta) */
    char outputPath[MAX_PATH];  /* caminho final do arquivo de saída */

    /* Uso:
       programa.exe "C:\caminho\imagem.pgm" saida.pgm
       São esperados exatamente 2 argumentos (entrada e saída), além do nome do programa.
    */
    if (argc != 3) {
        printf("Uso: %s <imagem_entrada.pgm> <imagem_saida.pgm>\n", argv[0]);
        return 1;
    }

    /* Abre a imagem de entrada em modo binário ("rb"), necessário para PGM P5 */
    fp = fopen(argv[1], "rb");

    if (fp == NULL) {
        printf("Erro ao abrir: %s\n", argv[1]);
        return 1;
    }

    /* Lê o PGM para o buffer input e obtém as dimensões.
       Obs.: ReadPGM já fecha o arquivo internamente, então o fclose
       abaixo é redundante (fechamento duplo do mesmo FILE*). */
    ReadPGM(fp, input, &rows, &cols);
    fclose(fp);

    /* Verifica o tamanho: este teste só aceita imagens 45x45
       (compatível com MAX_ROWS/MAX_COLS definidos em common.h) */
    if (rows != 45 || cols != 45) {
        printf("Erro: imagem deve ser 45x45.\n");
        return 1;
    }

    /* Mostra os parâmetros utilizados na execução */
    printf("Imagem: %dx%d\n", rows, cols);
    printf("Radius: %d\n", radius);
    printf("Eps: %.4f\n", eps);

    /* Aplica o self-guided filter: input é filtrada e o resultado vai para output.
       Retorno diferente de zero indica falha (parâmetros inválidos, por exemplo). */
    if (guidedFilterSelf(
            input,
            output,
            radius,
            eps,
            rows,
            cols) != 0) {

        printf("Erro no Guided Filter.\n");
        return 1;
    }

    /* Descobre a pasta do .exe: obtém o caminho completo do executável
       (independe do diretório de trabalho atual) */
    GetModuleFileNameA(NULL, exePath, MAX_PATH);

    {
        /* Corta o caminho logo após a última barra invertida, removendo
           o nome do executável e mantendo apenas a pasta (com a barra final) */
        char *lastSlash = strrchr(exePath, '\\');

        if (lastSlash != NULL)
            *(lastSlash + 1) = '\0';
        else
            exePath[0] = '\0';   /* sem barra: usa caminho relativo (pasta atual) */
    }

    /* Resultado será salvo ao lado do .exe: pasta + nome informado em argv[2] */
    snprintf(outputPath, MAX_PATH, "%s%s", exePath, argv[2]);

    /* Abre o arquivo de saída para escrita (o PGM P2 é texto, mas "wb"
       evita a conversão de quebras de linha no Windows) */
    fp = fopen(outputPath, "wb");

    if (fp == NULL) {
        printf("Erro ao criar arquivo de saida.\n");
        return 1;
    }

    /* Grava a imagem filtrada (output) em formato PGM P2.
       O parâmetro input é recebido pela função mas não é utilizado por ela. */
    WritePGM(rows, cols, input, output, fp);
    fclose(fp);

    printf("Resultado salvo em:\n%s\n", outputPath);

    return 0;
}
