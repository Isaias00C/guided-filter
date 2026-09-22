# Guided Image Filter (C)

Implementação em C do algoritmo *Guided Image Filter* (He, Sun, Tang — ECCV 2010), com E(1) tempo de execução, operando sobre imagens em escala de cinza no formato PGM.

## Estrutura do projeto

| Arquivo | Descrição |
|---|---|
| [main.c](main.c) | Ponto de entrada: lê os argumentos, carrega as imagens PGM e chama o filtro |
| [filter.c](filter.c) | Implementação do `guidedFilter` |
| [box_filter.c](box_filter.c) | Box filter (soma de área local) usado internamente pelo guided filter |
| [inout.c](inout.c) | Leitura/escrita de arquivos PGM (`ReadPGM` / `WritePGM`) |
| [images/](images/) | Imagens de exemplo (ex.: `cat.pgm`) |

## Pré-requisitos

- `gcc` (ou outro compilador C compatível)
- `libm` (biblioteca matemática padrão do sistema, já presente no Linux)

## Compilando

Não há Makefile no projeto; compile diretamente com `gcc`:

```bash
gcc -O2 -Wall main.c filter.c box_filter.c inout.c -lm -o guided_filter
```

Isso gera o executável `guided_filter` na raiz do projeto.

## Como testar

### 1. Teste rápido com a imagem de exemplo

```bash
./guided_filter images/cat.pgm images/cat.pgm 4 0.01 output.pgm
```

Parâmetros (nessa ordem):

1. `guidance_image_filename` — imagem-guia (PGM)
2. `input_filtering_image_filename` — imagem a ser filtrada (PGM)
3. `radius` — raio da janela local (inteiro, ex.: `4`)
4. `eps` — parâmetro de regularização (float, ex.: `0.01`)
5. `output_filename` — nome do arquivo PGM de saída

Se a execução funcionar, você verá no terminal mensagens como `Reading PGM....`, `begin calculting filtered_output....` e `Writing PGM....`, e o arquivo `output.pgm` será criado no diretório atual.

### 2. Verificando a saída

Confira se o arquivo de saída foi gerado e tem tamanho compatível com a imagem de entrada:

```bash
ls -la output.pgm
file output.pgm
```

Para visualizar o resultado, abra `output.pgm` em qualquer visualizador de imagens que suporte PGM (ex. GIMP) ou converta para PNG:

```bash
convert output.pgm output.png   # requer ImageMagick
```

### 3. Testando casos de erro (validação de argumentos)

O programa deve rejeitar chamadas com número incorreto de argumentos:

```bash
./guided_filter
# Esperado: mensagem de "Usage: ..." e saída sem gerar arquivo
```

E deve reportar erro ao tentar ler um arquivo inexistente:

```bash
./guided_filter arquivo_que_nao_existe.pgm images/cat.pgm 4 0.01 output.pgm
# Esperado: "reading error..."
```

### 4. Testando com diferentes parâmetros

Varie `radius` e `eps` para observar o efeito de suavização/preservação de bordas:

```bash
./guided_filter images/cat.pgm images/cat.pgm 2 0.1 output_r2.pgm
./guided_filter images/cat.pgm images/cat.pgm 8 0.0001 output_r8.pgm
```

Valores de `radius` maiores tendem a suavizar mais; valores de `eps` menores preservam mais as bordas guiadas pela imagem-guia.

### 5. Usando guidance e input diferentes

O filtro aceita imagens de guia e de entrada distintas (ex.: usar uma imagem nítida como guia para filtrar uma imagem ruidosa), desde que ambas tenham as mesmas dimensões:

```bash
./guided_filter images/guia.pgm images/entrada.pgm 4 0.01 saida.pgm
```

## Observações

- As imagens de entrada devem estar em formato PGM (P5 binário), em escala de cinza.
- O tamanho de cada patch local é `N = (2r+1)^2`, exceto nos pixels de borda.
- Não há suíte de testes automatizada neste projeto; a validação é feita executando o binário sobre imagens de exemplo e inspecionando a saída, conforme descrito acima.
