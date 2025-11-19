import struct
import numpy as np
import matplotlib.pyplot as plt
import argparse
import sys
import os


def read_and_visualize(filepath):
    """
    Читает бинарный файл изображения заданного формата, выводит информацию
    и визуализирует его.

    Формат файла:
    - 8 байт: N1 (uint64, количество строк)
    - 8 байт: N2 (uint64, количество столбцов)
    - 8 байт: resolution (double)
    - N1*N2 байт: данные пикселей (uint8)
    """
    # Проверяем, существует ли файл
    if not os.path.exists(filepath):
        print(f"Ошибка: Файл не найден по пути '{filepath}'")
        sys.exit(1)

    try:
        with open(filepath, 'rb') as f:
            # --- Шаг 1: Чтение заголовка ---
            # Читаем первые 24 байта, содержащие N1, N2 и разрешение
            header_data = f.read(24)
            if len(header_data) < 24:
                print(f"Ошибка: Файл '{filepath}' слишком мал и не содержит полного заголовка.")
                sys.exit(1)

            # Распаковываем бинарные данные.
            # '<' - порядок байтов little-endian (самый распространенный)
            # 'Q' - 8-байтное беззнаковое целое (unsigned long long)
            # 'd' - 8-байтный float (double)
            # Формат "<QQd" означает: uint64, uint64, double
            N1, N2, resolution = struct.unpack('<QQd', header_data)

            print("--- Информация об изображении ---")
            print(f"Размеры (строки x столбцы): {N1} x {N2}")
            print(f"Разрешение: {resolution}")
            print("---------------------------------")

            if N1 == 0 or N2 == 0:
                print("Ошибка: Размеры изображения не могут быть нулевыми.")
                sys.exit(1)

            # --- Шаг 2: Чтение данных пикселей ---
            # Читаем оставшиеся данные (N1 * N2 байт) в массив NumPy
            # np.fromfile очень эффективен для этой задачи
            image_data = np.fromfile(f, dtype=np.uint8, count=N1 * N2)

            # Проверяем, удалось ли прочитать все пиксели
            if image_data.size < N1 * N2:
                print("Ошибка: Неожиданный конец файла. Удалось прочитать меньше пикселей, чем заявлено в заголовке.")
                sys.exit(1)

            # Преобразуем 1D-массив в 2D-матрицу размером N1 x N2
            image_matrix = image_data.reshape((N1, N2))

            # --- Шаг 3: Визуализация ---
            plt.style.use('default')
            fig, ax = plt.subplots(figsize=(10, 8))

            # imshow - основная функция для отображения 2D-массивов
            im = ax.imshow(image_matrix, cmap='gray_r', vmin=0, vmax=255)

            # Настраиваем отображение
            ax.set_title(f'Изображение из файла: {os.path.basename(filepath)}\nРазмеры: {N1}x{N2}', fontsize=14)
            ax.set_xlabel(f'Столбцы (N2 = {N2})')
            ax.set_ylabel(f'Строки (N1 = {N1})')

            # Добавляем colorbar, который показывает соответствие цвета значению пикселя
            cbar = fig.colorbar(im, ax=ax)
            cbar.set_label('Значение пикселя (0=пора [белый], 255=порода [черный])')

            plt.tight_layout()  # Автоматически подбирает отступы
            plt.show()

    except IOError as e:
        print(f"Ошибка чтения файла: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"Произошла непредвиденная ошибка: {e}")
        sys.exit(1)


# --- Главный блок для запуска ---
if __name__ == '__main__':
    # ================================================================= #
    # УКАЖИТЕ ПУТЬ К ВАШЕМУ ФАЙЛУ ЗДЕСЬ                             #
    # Например: "C:/Users/User/Documents/image.bin" или просто "image.bin" #
    # если файл лежит рядом со скриптом.                               #

    FILE_TO_VISUALIZE_IN_PYCHARM = "D:\VS_Projects\Magister_projects\C_Project\images2\picture_2d.raw"  # <-- ИЗМЕНИТЕ ЭТУ СТРОКУ

    # ================================================================= #

    # Вызываем основную функцию с указанным путем
    read_and_visualize(FILE_TO_VISUALIZE_IN_PYCHARM)