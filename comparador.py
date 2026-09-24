import numpy as np
from PIL import Image
import matplotlib.pyplot as plt
import cv2

output_actual = cv2.imread("images_pgm_c_filtradas/resultado01.pgm", cv2.IMREAD_GRAYSCALE)
output_expected = cv2.imread("images_pgm_py_filtradas/resultado01.pgm", cv2.IMREAD_GRAYSCALE)

if output_expected.shape != output_actual.shape:
    print("Diferentes: dimensões distintas")
elif output_expected.dtype != output_actual.dtype:
    print("Diferentes: tipos de dado distintos (ex.: 8 vs 16 bits)")
elif np.array_equal(output_expected, output_actual):
    print("As imagens são idênticas pixel a pixel")
else:
    mask = output_expected != output_actual
    n_diff = mask.sum()
    print(f"Diferentes: {n_diff} pixels ({100 * n_diff / mask.size:.4f}% do total)")

    # diferença absoluta em tipo maior, para evitar overflow do uint8
    diff = np.abs(output_expected.astype(np.int32) - output_actual.astype(np.int32))
    print("Maior diferença:", diff.max())

    coords = np.argwhere(mask)
    print("Primeiros pixels diferentes (linha, coluna):")
    print(coords[:10])


fig, axs = plt.subplots(1, 3, figsize=(12, 4))
axs[0].imshow(output_expected, cmap="gray"); axs[0].set_title("saida esperada")
axs[1].imshow(output_actual, cmap="gray"); axs[1].set_title("saida atual do algoritmo em C")
axs[2].imshow(mask, cmap="gray"); axs[2].set_title("Pixels diferentes")
for ax in axs:
    ax.axis("off")
plt.show()