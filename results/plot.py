import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from mpl_toolkits.mplot3d import Axes3D

rn = 10;
nn = 4000;

def summary(*algs):
    for alg in algs:
        times = np.fromfile(alg + '/time.dat')
        alives = np.fromfile(alg + '/alive_sum.dat')
        outputs = np.fromfile(alg + '/output_track.dat')
        times = [times[i] for i,a in enumerate(alives) if 0 < a < nn-1]
        print alg, ':', 'FND =', times[0], 'LND =', times[-1], 'OUTPUT =', outputs[-1]

def plot_time_alive(*algs):
    fig, ax = plt.subplots()
    for alg in algs:
        times = np.fromfile(alg + '/time.dat')
        alives = np.fromfile(alg + '/alive_sum.dat')
        ax.plot(times, alives)
    plt.show()

def plot_time_energy(*algs):
    fig, ax = plt.subplots()
    for alg in algs:
        times = np.fromfile(alg + '/time.dat')
        energys = np.fromfile(alg + '/energy_sum.dat')
        ax.plot(times, energys)
    plt.show()

def plot_time_output(*algs):
    fig, ax = plt.subplots()
    for alg in algs:
        times = np.fromfile(alg + '/time.dat')
        outputs = np.fromfile(alg + '/output_track.dat')
        ax.plot(times, outputs)
    plt.show()

def plot_output_alive(*algs):
    fig, ax = plt.subplots()
    for alg in algs:
        outputs = np.fromfile(alg + '/output_track.dat')
        alives = np.fromfile(alg + '/alive_sum.dat')
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
    es = np.fromfile(alg + '/energy_snapshot.dat')[nn*snap : nn*(snap+1)]
    hops[hops < 0] = 0

    chxs = [xs[i] for i,c in enumerate(chs) if c == i and es[i] > 0]
    chys = [ys[i] for i,c in enumerate(chs) if c == i and es[i] > 0]
    mnxs = [xs[i] for i,c in enumerate(chs) if c != i and c >= 0 and es[i] > 0]
    mnys = [ys[i] for i,c in enumerate(chs) if c != i and c >= 0 and es[i] > 0]

    fig, ax = plt.subplots()

    ax.scatter(xs[0], ys[0], s = 40, color="green")
    ax.scatter(chxs, chys, s = 15, alpha = 0.5, color="red")
    ax.scatter(mnxs, mnys, s = 10, alpha = 0.5, color="blue")

    for i in xrange(1,nn):
        if hops[i] < 0 or es[i] <= 0:
            continue
        if i != chs[i]:
            ax.plot([xs[i],xs[hops[i]]], [ys[i],ys[hops[i]]], alpha = 0.3, color="black")
        else:
            ax.plot([xs[i],xs[hops[i]]], [ys[i],ys[hops[i]]], alpha = 0.8, color="black")
    plt.show()

def plot_xy_es(alg, snap):
    xs = np.fromfile(alg + '/nodes_x.dat')
    ys = np.fromfile(alg + '/nodes_y.dat')
    es = np.fromfile(alg + '/energy_snapshot.dat')[nn*snap : nn*(snap+1)]
    
    fig, ax = plt.subplots()
    
    dxs = [xs[i] for i,c in enumerate(es) if es[i] <= 0 and i > 0]
    dys = [ys[i] for i,c in enumerate(es) if es[i] <= 0 and i > 0]
    ax.scatter(dxs, dys, s = 10, alpha = 0.5, color="black")
    
    dxs = [xs[i] for i,c in enumerate(es) if 0 < es[i] <= 500]
    dys = [ys[i] for i,c in enumerate(es) if 0 < es[i] <= 500]
    ax.scatter(dxs, dys, s = 10, alpha = 0.5, color="red")

    dxs = [xs[i] for i,c in enumerate(es) if 500 < es[i] <= 1000]
    dys = [ys[i] for i,c in enumerate(es) if 500 < es[i] <= 1000]
    ax.scatter(dxs, dys, s = 10, alpha = 0.5, color="yellow")

    dxs = [xs[i] for i,c in enumerate(es) if 1000 < es[i] <= 1500]
    dys = [ys[i] for i,c in enumerate(es) if 1000 < es[i] <= 1500]
    ax.scatter(dxs, dys, s = 10, alpha = 0.5, color="green")

    dxs = [xs[i] for i,c in enumerate(es) if 1500 < es[i] <= 2000]
    dys = [ys[i] for i,c in enumerate(es) if 1500 < es[i] <= 2000]
    ax.scatter(dxs, dys, s = 10, alpha = 0.5, color="blue")

    plt.show()

def plot_xy_es_rainbow(alg, snap):
    xs = np.fromfile(alg + '/nodes_x.dat')
    ys = np.fromfile(alg + '/nodes_y.dat')
    es = np.fromfile(alg + '/energy_snapshot.dat')[nn*snap : nn*(snap+1)]
    es[es<0] = 0

    fig, ax = plt.subplots()
    cs = cm.rainbow(es/2000)

    for i,x in enumerate(xs):
        ax.scatter(x, ys[i], s=10, alpha=0.5, color=cs[i])

    plt.show()

def plot_xyt_rotate(ta, tb):
    xyts = np.fromfile('lcr/rotate.dat')
    
    ads = xyts[0::7]
    xs = xyts[1::7]
    ys = xyts[2::7]

    nads = xyts[3::7]
    nxs = xyts[4::7]
    nys = xyts[5::7]

    ts = xyts[6::7]
    xs = [xs[i] for i,t in enumerate(ts) if ta < t < tb]
    ys = [ys[i] for i,t in enumerate(ts) if ta < t < tb]
    ts = [ts[i] for i,t in enumerate(ts) if ta < t < tb]
    ds = [1-((x/350)**2 + (y/350)**2)**0.5 for x,y in zip(xs,ys)]
    cs = cm.rainbow(ds)

    fig = plt.figure()
    ax = fig.gca(projection='3d')
    ax.scatter(xs, ys, ts, color=cs, cmap=cm.hsv)
    ax.set_xlim3d(0, 350)
    ax.set_ylim3d(0, 350)
    plt.show()

def plot_xyt_rotate(s):
    xyts = np.fromfile('lcr/rotate_backup.dat')
    xs = xyts[1::7][::s]
    ys = xyts[2::7][::s]
    ts = xyts[6::7][::s]
    ds = [1-((x/350)**2 + (y/350)**2)**0.5 for x,y in zip(xs,ys)]
    cs = cm.rainbow(ds)

    fig = plt.figure()
    ax = fig.gca(projection='3d')
    ax.scatter(xs, ys, ts, color=cs, cmap=cm.hsv)
    ax.set_xlim3d(0, 350)
    ax.set_ylim3d(0, 350)
    
    plt.show()

def plot_xyt_track_all(s):
    chs = np.fromfile('lcr/ch_snapshot.dat')[nn : nn*2]
    chas = [c for i,c in enumerate(chs) if c == i][::s]
    plot_xyt_track(chas, 0, 30000)

def plot_xyt_track(iads, ta, tb):
    xyts = np.fromfile('lcr/rotate.dat')
    
    ads = xyts[0::7]
    xs = xyts[1::7]
    ys = xyts[2::7]

    nads = xyts[3::7]
    nxs = xyts[4::7]
    nys = xyts[5::7]

    ts = xyts[6::7]

    ads = [ads[i] for i,t in enumerate(ts) if ta < t < tb]
    nads = [nads[i] for i,t in enumerate(ts) if ta < t < tb]
    xs = [xs[i] for i,t in enumerate(ts) if ta < t < tb]
    ys = [ys[i] for i,t in enumerate(ts) if ta < t < tb]
    nxs = [nxs[i] for i,t in enumerate(ts) if ta < t < tb]
    nys = [nys[i] for i,t in enumerate(ts) if ta < t < tb]
    ts = [ts[i] for i,t in enumerate(ts) if ta < t < tb]

    fig = plt.figure()
    ax = fig.gca(projection='3d')
    
    tracks = dict();
    for iad in iads:
        tracks[iad] = dict()
        tracks[iad]['ads'] = list()
        tracks[iad]['xs'] = list()
        tracks[iad]['ys'] = list()
        tracks[iad]['ts'] = list()
        tmpa = iad
        for i,a in enumerate(ads):
            if a == tmpa:
                tracks[iad]['ads'].append(ads[i])
                tracks[iad]['xs'].append(xs[i])
                tracks[iad]['ys'].append(ys[i])
                tracks[iad]['ts'].append(ts[i])
                #tracks[iad]['xs'].append(nxs[i])
                #tracks[iad]['ys'].append(nys[i])
                tmpa = nads[i]


        ds = [1-((x/350)**2 + (y/350)**2)**0.5 for x,y in zip(tracks[iad]['xs'], tracks[iad]['ys'])]
        maxt = max(tracks[iad]['ts'])
        mint = min(tracks[iad]['ts'])
        cts = [(x-mint)/(maxt-mint) for x in tracks[iad]['ts']]
        cs = cm.rainbow(cts)

        ax.scatter(tracks[iad]['xs'], tracks[iad]['ys'], tracks[iad]['ts'], color=cs, cmap=cm.hsv)
        ax.plot(tracks[iad]['xs'], tracks[iad]['ys'], tracks[iad]['ts'])
    
    ax.set_xlim3d(0, 350)
    ax.set_ylim3d(0, 350)
    plt.show()

def plot_inds(*algs):
    fig, ax = plt.subplots()
    for alg in algs:
        if alg == 'lcr':
            inds = np.loadtxt('lcr/inds.txt', skiprows=1, usecols=[3,4,5])
            c = 'red'
        if alg == 'heed':
            inds = np.loadtxt('heed/inds.txt', skiprows=1, usecols=[1,2,3])
            c = 'green'
        if alg == 'ecpf':
            inds = np.loadtxt('ecpf/inds.txt', skiprows=1, usecols=[1,2,3])
            c = 'blue'
        
        for fnd,lnd,output in inds:
            ax.scatter(fnd, lnd, s = (output / 1e9)**10/150, alpha = 0.5, color=c)
            ax.scatter(fnd, lnd, s = 10, alpha = 1, color='black')
    plt.show()
