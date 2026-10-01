import numpy as np
from PIL import Image

# Abre a imagem P5
for i in range(1, 13):
    img = Image.open(f'images/imagem{i:02d}.pgm')
    img_resized = img.resize((45, 45), Image.Resampling.LANCZOS)

    # 3. Converter para uma matriz NumPy do tipo uint8
    matriz_uint8 = np.array(img_resized, dtype=np.uint8)
    # Verificação do resultado
    print("Novo formato:", matriz_uint8.shape)  # Saída esperada: (45, 45)
    print("Tipo de dado:", matriz_uint8.dtype)  # Saída esperada: uint8

    width, height = img_resized.size
    pixels = list(img_resized.getdata())

    # Salva como P2 (ASCII)

    with open(f'images/imagem{i:02d}_ascii.pgm', 'w') as f:
        f.write('P2\n')
        f.write(f'{width} {height}\n')
        f.write('255\n')  # Valor máximo do pixel (ajuste se necessário)
        
        # Escreve os pixels separando por quebras de linha ou espaços
        for j, p in enumerate(pixels):
            f.write(f'{p} ')
            if (j + 1) % width == 0:
                f.write('\n')
