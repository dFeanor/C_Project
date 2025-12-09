import struct
import numpy as np
import matplotlib.pyplot as plt
import os

DIR = "picture_results"

def load(path):
    if not os.path.exists(path): return None
    with open(path, 'rb') as f:
        r, c = struct.unpack('ii', f.read(8))
        d = np.frombuffer(f.read(), dtype=np.float64)
    return d.reshape((r, c))

def show_comp():
    num = load(os.path.join(DIR, "case1_num_mag.raw"))
    ana = load(os.path.join(DIR, "case1_ana_mag.raw"))
    if num is None or ana is None: return

    diff = np.abs(num - ana)
    fig, ax = plt.subplots(3, 1, figsize=(6, 10))
    fig.suptitle("Task 1: Poiseuille Comparison")
    
    ax[0].imshow(ana, cmap='plasma', aspect='auto'); ax[0].set_title("Analytical")
    plt.colorbar(ax[0].images[0], ax=ax[0])
    
    ax[1].imshow(num, cmap='plasma', aspect='auto'); ax[1].set_title("Numerical")
    plt.colorbar(ax[1].images[0], ax=ax[1])
    
    im = ax[2].imshow(diff, cmap='Reds', aspect='auto'); ax[2].set_title("Difference")
    plt.colorbar(im, ax=ax[2])
    plt.show()

def show_case(pre, tit):
    mag = load(os.path.join(DIR, f"{pre}_mag.raw"))
    prs = load(os.path.join(DIR, f"{pre}_pressure.raw"))
    if mag is None: return

    fig, ax = plt.subplots(2, 1, figsize=(6, 8))
    fig.suptitle(tit)
    
    im0 = ax[0].imshow(mag, cmap='inferno'); ax[0].set_title("Velocity Magnitude |V|")
    plt.colorbar(im0, ax=ax[0])
    
    im1 = ax[1].imshow(prs, cmap='viridis'); ax[1].set_title("Pressure Field")
    plt.colorbar(im1, ax=ax[1])
    plt.show()

def main():
    if not os.path.exists(DIR): return
    show_comp()
    show_case("case2_pore", "Task 2: Pore")
    show_case("case3_sin", "Task 3: Sinusoid")
    show_case("case4_2d", "Task 4: 2D Raw")
    show_case("case5_3d", "Task 5: 3D Slice")

if __name__ == "__main__":
    main()