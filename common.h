#ifndef COMMON_H
#define COMMON_H

/* Dimensões máximas de imagem suportadas sem alocação dinâmica.
   Todos os buffers temporários têm seu tamanho definido a partir destes
   limites em tempo de compilação, em vez de usar malloc/free em tempo de
   execução. Aumente-os se precisar processar imagens maiores, mas note que
   só o filter.c mantém quase 30 buffers de MAX_PIXELS floats vivos, então o
   uso de RAM cresce rápido (MAX_PIXELS * 4 bytes * ~30 buffers).

   MAX_ROWS e MAX_COLS limitam cada dimensão de forma independente (por
   exemplo, o boxfilter() do filter.c dimensiona cumheightvector[MAX_COLS] e
   cumheightvectorcol[MAX_ROWS] pela largura/altura reais, e não apenas pelo
   total de pixels), portanto ambos devem cobrir a maior largura E a maior
   altura entre as imagens, incluindo as orientações retrato e paisagem
   (por exemplo, amostras de 480x640 e 640x480). */

/* Altura máxima da imagem, em pixels (linhas) */
#define MAX_ROWS   45
/* Largura máxima da imagem, em pixels (colunas) */
#define MAX_COLS   45
/* Total máximo de pixels; define o tamanho dos buffers estáticos */
#define MAX_PIXELS (MAX_ROWS * MAX_COLS)

#endif
