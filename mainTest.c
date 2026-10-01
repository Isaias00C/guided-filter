#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>

#include "common.h"

/* Funções já existentes no seu projeto */
void ReadPGM(FILE *fp, unsigned char *image, int *rows, int *cols);
void WritePGM(int rows, int cols,
              unsigned char *input,
              unsigned char *output,
              FILE *fp);

/* Novo self-guided filter */
int guidedFilterSelf(const uint8_t *src,
                     uint8_t *dest,
                     int radius,
                     float eps,
                     int rows,
                     int cols);

int main(int argc, char *argv[])
{
    FILE *fp;
    int rows, cols;
    int radius = 2;
    float eps = 0.04f;

    static uint8_t input[MAX_PIXELS];
    static uint8_t output[MAX_PIXELS];

    char exePath[MAX_PATH];
    char outputPath[MAX_PATH];
    /* Uso:
       programa.exe "C:\caminho\imagem.pgm"
    */
    if (argc != 3) {
        printf("Uso: %s <imagem_entrada.pgm> <imagem_saida.pgm>\n", argv[0]);
        return 1;
    }

    /* Abre imagem */
    fp = fopen(argv[1], "rb");

    if (fp == NULL) {
        printf("Erro ao abrir: %s\n", argv[1]);
        return 1;
    }

    /* Lê PGM */
    ReadPGM(fp, input, &rows, &cols);
    fclose(fp);

    /* Verifica tamanho */
    if (rows != 45 || cols != 45) {
        printf("Erro: imagem deve ser 45x45.\n");
        return 1;
    }

    printf("Imagem: %dx%d\n", rows, cols);
    printf("Radius: %d\n", radius);
    printf("Eps: %.4f\n", eps);

    /* Self-guided filter */
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

    /* Descobre pasta do .exe */
    GetModuleFileNameA(NULL, exePath, MAX_PATH);

    {
        char *lastSlash = strrchr(exePath, '\\');

        if (lastSlash != NULL)
            *(lastSlash + 1) = '\0';
        else
            exePath[0] = '\0';
    }

    /* Resultado será salvo ao lado do .exe */
    snprintf(outputPath, MAX_PATH, "%s%s", exePath, argv[2]);

    fp = fopen(outputPath, "wb");

    if (fp == NULL) {
        printf("Erro ao criar arquivo de saida.\n");
        return 1;
    }

    WritePGM(rows, cols, input, output, fp);
    fclose(fp);

    printf("Resultado salvo em:\n%s\n", outputPath);

    return 0;
}