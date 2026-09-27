# import numpy as np
# from PIL import Image

# # 1. Abrir a imagem original
# img = Image.open("images/imagem12.pgm")

# # 2. Redimensionar para (276, 276)
# # Usamos o filtro Resampling.LANCZOS para manter a alta qualidade
# img_resized = img.resize((276, 276), Image.Resampling.LANCZOS)

# # 3. Converter para uma matriz NumPy do tipo uint8
# matriz_uint8 = np.array(img_resized, dtype=np.uint8)

# # Verificação do resultado
# print("Novo formato:", matriz_uint8.shape)  # Saída esperada: (276, 276)
# print("Tipo de dado:", matriz_uint8.dtype)  # Saída esperada: uint8

# img.save("imagem12.pgm")