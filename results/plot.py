import numpy as np
import matplotlib.pyplot as plt

rn = 10;
nn = 500;

# es = np.fromfile(alg + 'energy_snapshot.dat')[nn*snap : nn*(snap+1)]

def plot_time_alive(alg):
    times = np.fromfile(alg + '/time.dat')
    alives = np.fromfile(alg + '/alive_sum.dat')
    fig, ax = plt.subplots()
    ax.plot(times, alives)
    plt.show()

def plot_time_energy(alg):
    times = np.fromfile(alg + '/time.dat')
    energys = np.fromfile(alg + '/energy_sum.dat')
    fig, ax = plt.subplots()
    ax.plot(times, energys)
    plt.show()

def plot_time_output(alg):
    times = np.fromfile(alg + '/time.dat')
    outputs = np.fromfile(alg + '/output_track.dat')
    fig, ax = plt.subplots()
    ax.plot(times, outputs)
    plt.show()

def plot_output_alive(alg):
    outputs = np.fromfile(alg + '/output_track.dat')
    alives = np.fromfile(alg + '/alive_sum.dat')
    fig, ax = plt.subplots()
    ax.plot(outputs, alives)
    plt.show()


def plot_xy_chs(alg, snap):
    xs = np.fromfile(alg + '/nodes_x.dat')
    ys = np.fromfile(alg + '/nodes_y.dat')
    chs = np.fromfile(alg + '/ch_snapshot.dat')[nn*snap : nn*(snap+1)]
    
    chxs = [xs[i] for i,c in enumerate(chs) if c == i]
    chys = [ys[i] for i,c in enumerate(chs) if c == i]
    mnxs = [xs[i] for i,c in enumerate(chs) if c != i and c > 0]
    mnys = [ys[i] for i,c in enumerate(chs) if c != i and c > 0]

    fig, ax = plt.subplots()

    ax.scatter(xs[0], ys[0], s = 40, color="green")
    ax.scatter(chxs, chys, s = 15, alpha = 0.5, color="red")
    ax.scatter(mnxs, mnys, s = 10, alpha = 0.5, color="blue")

    for i in xrange(1,nn):
        if i != chs[i]:
            ax.plot([xs[i],xs[chs[i]]], [ys[i],ys[chs[i]]], alpha = 0.3, color="black")
    plt.show()

def plot_xy_hops(alg, snap):
    xs = np.fromfile(alg + '/nodes_x.dat')
    ys = np.fromfile(alg + '/nodes_y.dat')
    chs = np.fromfile(alg + '/ch_snapshot.dat')[nn*snap : nn*(snap+1)]
    hops = np.fromfile(alg + '/hop_snapshot.dat')[nn*snap : nn*(snap+1)]
    
    chxs = [xs[i] for i,c in enumerate(chs) if c == i]
    chys = [ys[i] for i,c in enumerate(chs) if c == i]
    mnxs = [xs[i] for i,c in enumerate(chs) if c != i and c > 0]
    mnys = [ys[i] for i,c in enumerate(chs) if c != i and c > 0]

    fig, ax = plt.subplots()

    ax.scatter(xs[0], ys[0], s = 40, color="green")
    ax.scatter(chxs, chys, s = 15, alpha = 0.5, color="red")
    ax.scatter(mnxs, mnys, s = 10, alpha = 0.5, color="blue")

    for i in xrange(1,nn):
        if i != chs[i]:
            ax.plot([xs[i],xs[hops[i]]], [ys[i],ys[hops[i]]], alpha = 0.3, color="black")
        else:
            ax.plot([xs[i],xs[hops[i]]], [ys[i],ys[hops[i]]], alpha = 0.8, color="black")
    plt.show()


