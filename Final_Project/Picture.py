import struct
import numpy as np
import matplotlib.pyplot as plt
import argparse
import sys
import os


def read_and_visualize(filepath):
    if not os.path.exists(filepath):
        print(f"Ошибка: Файл не найден по пути '{filepath}'")
        sys.exit(1)

    try:
        with open(filepath, 'rb') as f:
            header_data = f.read(24)
            if len(header_data) < 24:
                print(f"Ошибка: Файл '{filepath}' слишком мал и не содержит полного заголовка.")
                sys.exit(1)

            N1, N2, resolution = struct.unpack('<QQd', header_data)

            print("--- Информация об изображении ---")
            print(f"Размеры (строки x столбцы): {N1} x {N2}")
            print(f"Разрешение: {resolution}")
            print("---------------------------------")

            if N1 == 0 or N2 == 0:
                print("Ошибка: Размеры изображения не могут быть нулевыми.")
                sys.exit(1)

            image_data = np.fromfile(f, dtype=np.uint8, count=N1 * N2)

            if image_data.size < N1 * N2:
                print("Ошибка: Неожиданный конец файла. Удалось прочитать меньше пикселей, чем заявлено в заголовке.")
                sys.exit(1)

            image_matrix = image_data.reshape((N1, N2))

            plt.style.use('default')
            fig, ax = plt.subplots(figsize=(10, 8))

            im = ax.imshow(image_matrix, cmap='gray_r', vmin=0, vmax=255)

            ax.set_title(f'Изображение из файла: {os.path.basename(filepath)}\nРазмеры: {N1}x{N2}', fontsize=14)
            ax.set_xlabel(f'Столбцы (N2 = {N2})')
            ax.set_ylabel(f'Строки (N1 = {N1})')

            cbar = fig.colorbar(im, ax=ax)
            cbar.set_label('Значение пикселя (0=пора [белый], 255=порода [черный])')

            plt.tight_layout()  
            plt.show()

    except IOError as e:
        print(f"Ошибка чтения файла: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"Произошла непредвиденная ошибка: {e}")
        sys.exit(1)


if __name__ == '__main__':
    FILE_TO_VISUALIZE_IN_PYCHARM = "./images/gen_cylinder_without_pore.raw" 

    read_and_visualize(FILE_TO_VISUALIZE_IN_PYCHARM)
    
<<<<<<< HEAD

import struct
import numpy as np
import matplotlib.pyplot as plt
import argparse
import sys
import os


def read_and_visualize(filepath):
    if not os.path.exists(filepath):
        print(f"Ошибка: Файл не найден по пути '{filepath}'")
        sys.exit(1)

    try:
        with open(filepath, 'rb') as f:
            header_data = f.read(24)
            if len(header_data) < 24:
                print(f"Ошибка: Файл '{filepath}' слишком мал и не содержит полного заголовка.")
                sys.exit(1)

            N1, N2, resolution = struct.unpack('<QQd', header_data)

            print("--- Информация об изображении ---")
            print(f"Размеры (строки x столбцы): {N1} x {N2}")
            print(f"Разрешение: {resolution}")
            print("---------------------------------")

            if N1 == 0 or N2 == 0:
                print("Ошибка: Размеры изображения не могут быть нулевыми.")
                sys.exit(1)

            image_data = np.fromfile(f, dtype=np.uint8, count=N1 * N2)

            if image_data.size < N1 * N2:
                print("Ошибка: Неожиданный конец файла. Удалось прочитать меньше пикселей, чем заявлено в заголовке.")
                sys.exit(1)

            image_matrix = image_data.reshape((N1, N2))

            plt.style.use('default')
            fig, ax = plt.subplots(figsize=(10, 8))

            im = ax.imshow(image_matrix, cmap='gray_r', vmin=0, vmax=255)

            ax.set_title(f'Изображение из файла: {os.path.basename(filepath)}\nРазмеры: {N1}x{N2}', fontsize=14)
            ax.set_xlabel(f'Столбцы (N2 = {N2})')
            ax.set_ylabel(f'Строки (N1 = {N1})')

            cbar = fig.colorbar(im, ax=ax)
            cbar.set_label('Значение пикселя (0=пора [белый], 255=порода [черный])')

            plt.tight_layout()  
            plt.show()

    except IOError as e:
        print(f"Ошибка чтения файла: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"Произошла непредвиденная ошибка: {e}")
        sys.exit(1)


if __name__ == '__main__':
    FILE_TO_VISUALIZE_IN_PYCHARM = "./images/gen_cylinder_without_pore.raw" 

    read_and_visualize(FILE_TO_VISUALIZE_IN_PYCHARM)
    
=======
>>>>>>> parent of 211f030 (AAAAAAAAA)
