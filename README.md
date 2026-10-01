# Guided Filter em C e Python

Projeto acadêmico com uma implementação do *Guided Image Filter* para imagens em tons de cinza. O repositório reúne um exemplo C com imagem embutida, um executável de teste para arquivos PGM e scripts/notebooks Python usados como referência visual e para comparar resultados.

O filtro segue a formulação de He, Sun e Tang (ECCV 2010). Nesta versão C, a imagem é usada como guia e também como entrada (*self-guided filter*). A implementação em `filter.c` calcula estatísticas locais diretamente, sem a otimização de tempo constante da formulação original.

## Estrutura do repositório

| Caminho | Função |
|---|---|
| `main.c` | Exemplo C que copia a imagem embutida para a RAM, aplica o filtro e mantém o resultado em uma matriz. |
| `filter.c` | Implementação de `guidedFilterSelf`, com entrada e saída em pixels de 8 bits. |
| `common.h` | Limites estáticos de imagem: 45 linhas por 45 colunas. |
| `image_flash.c`, `image_flash.h` | Dados da imagem de demonstração e declaração do vetor que representa a memória Flash. |
| `mainTest.c` | Aplicativo de console para ler uma imagem PGM, filtrá-la e gravar outra PGM. Usa APIs do Windows. |
| `inout.c` | Leitura e gravação de PGM nos formatos P2 (ASCII) e P5 (binário). |
| `conversao.py` | Redimensiona as 12 imagens PGM para 45×45 e cria cópias ASCII com sufixo `_ascii`. |
| `visualizador.py` | Aplica o Guided Filter do OpenCV às imagens e exibe comparações com detecção de bordas Canny. |
| `comparador.py` | Compara pixel a pixel as imagens filtradas por C e Python e mostra as diferenças. |
| `comparador.ipynb`, `teste.ipynb`, `visualizador.ipynb` | Notebooks de experimentação, comparação e visualização. |
| `images/` | Imagens de entrada, em versões PGM binárias e versões ASCII redimensionadas. |
| `images_pgm_c_filtradas/` | Resultados C usados pelo comparador. |
| `images_pgm_py_filtradas/` | Resultados da referência Python. |
| `requirements.txt` | Dependências Python para os scripts e notebooks. |

## Requisitos

- Compilador C. Para compilar `mainTest.c`, use Windows com MSVC ou MinGW, pois esse arquivo usa `windows.h`.
- Python e `pip` para os fluxos de visualização e comparação.
- As imagens processadas pelo código C devem caber nos limites definidos em `common.h` (atualmente 45×45).

## Compilação e execução em C

### Exemplo com imagem embutida

`main.c` processa a matriz `image_flash` com raio 2 e `eps` igual a `0.01`. O resultado permanece no vetor `output`; este exemplo não lê argumentos, não lê PGM e não grava arquivo.

Compile incluindo `image_flash.c`, que fornece os dados referenciados por `main.c`:

```powershell
gcc -std=c11 -O2 -Wall -Wextra main.c filter.c image_flash.c -o guided_filter.exe
```

O projeto Visual Studio `testing.vcxproj` lista `main.c`, `filter.c` e `inout.c`, mas não lista `image_flash.c`. Para usá-lo com o exemplo de imagem embutida, inclua `image_flash.c` no projeto.

### Teste com arquivos PGM no Windows

`mainTest.c` recebe o caminho da imagem de entrada e o nome do arquivo de saída. Ele usa `radius = 2` e `eps = 0.04`, exige imagem de 45×45 e salva a saída ao lado do executável. Passe apenas o nome do arquivo de saída, não um caminho completo.

Compile:

```powershell
gcc -std=c11 -O2 -Wall mainTest.c filter.c inout.c -o guided_filter_pgm.exe
```

Execute, por exemplo, a partir da raiz do repositório:

```powershell
.\guided_filter_pgm.exe images\imagem01_ascii.pgm resultado01.pgm
```

O leitor PGM também aceita P5. A entrada precisa ser em escala de cinza e respeitar os limites de tamanho; para o executável de teste, as dimensões devem ser exatamente 45×45.

## Fluxo Python de referência

Crie um ambiente e instale as dependências:

```powershell
python -m venv .venv
.\.venv\Scripts\Activate.ps1
python -m pip install -r requirements.txt
```

Na raiz do projeto, o fluxo principal é:

```powershell
python conversao.py
python visualizador.py
python comparador.py
```

`conversao.py` gera as versões de 45×45 em `images/`. `visualizador.py` aplica `cv2.ximgproc.guidedFilter` com raio 2 e `eps = 0.04` e grava os resultados em `images_pgm_py_filtradas/`. O comparador espera encontrar os resultados C correspondentes em `images_pgm_c_filtradas/`.

As pastas de saída precisam existir antes de executar os scripts. Os notebooks permitem explorar parâmetros e visualizar as imagens e diferenças; abra-os no Jupyter/VS Code com o ambiente Python configurado.

## Parâmetros e limites

- `radius`: raio da janela local. Valores maiores ampliam a vizinhança e aumentam o custo de processamento.
- `eps`: regularização. No código C, `eps` é interpretado na escala normalizada e convertido para pixels de 8 bits por `eps × 255²`.
- As janelas são truncadas nas bordas da imagem.
- O filtro C recebe uma única imagem (`src`) para guia e entrada; não implementa o caso de guia e imagem filtrada distintas.
- A capacidade máxima é definida por `MAX_ROWS` e `MAX_COLS` em `common.h`. Aumentar esses valores exige considerar o espaço estático disponível no alvo.

## Referência

Kaiming He, Jian Sun e Xiaoou Tang, “Guided Image Filtering”, ECCV 2010.
