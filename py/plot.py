import os
import numpy as np
from scipy import interpolate
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from mpl_toolkits.mplot3d import Axes3D

rn = 10
nn = 4000

pdir = '../results_tmp/'
pdirs = ['../results_5000_300_300/',\
         '../results_4000_300_300/',\
         '../results_3000_300_300/',\
         '../results_2083_250_250/',\
         '../results_1333_200_200/']
pdirs = pdirs[::-1]
#pdir = pdirs[0]

colors = {'lcr':'red','ifucm':'green','ecpf':'orange','heed':'blue'}
labels = {'lcr':'ACR','ifucm':'IFUC-M','ecpf':'ECPF','heed':'HEED(AMRP)'}

def plot_es_all(snap):
    axs = [None, None, None, None]
    fig, ((axs[0], axs[1]), (axs[2], axs[3])) = plt.subplots(2, 2, sharex='col', sharey='row')
    for i,alg in enumerate(['lcr','ifucm','ecpf','heed']):
        es = np.fromfile(pdir + alg + '/energy_snapshot.dat')[nn*snap : nn*(snap+1)][::5]
        es[es<0] = 0
        for j,e in enumerate(es):
            axs[i].scatter(j*5, e, s=3, alpha=0.5, color=colors[alg])
        axs[i].set_xlabel(labels[alg] + ': node ID')
        axs[i].set_ylabel('residual energy (mJ)')
        axs[i].set_xticks([0,1000,2000,3000,4000])
        axs[i].set_xlim(0,4000)
        axs[i].set_ylim(0,2000)
        axs[i].ticklabel_format(axis='x', style='sci', scilimits=(-2,2))
        axs[i].ticklabel_format(axis='y', style='sci', scilimits=(-2,2))
        axs[i].grid(True)
    # plt.show()
    plt.savefig('ess.pdf')

def plot_xyt_rotate(s):
    xyts = np.fromfile(pdir + 'lcr/rotate_backup.dat')
    xs = xyts[1::7][::s]
    ys = xyts[2::7][::s]
    ts = xyts[6::7][::s]
    ds = [1-((x/350)**2 + (y/350)**2)**0.5 for x,y in zip(xs,ys)]
    cs = cm.rainbow(ds)

    fig = plt.figure()
    ax = fig.gca(projection='3d')
    ax.scatter(xs, ys, ts, color=cs, cmap=cm.hsv)
    ax.set_xlim3d(0, 300)
    ax.set_ylim3d(0, 300)
    ax.set_zlim3d(0, 16000)
    ax.set_xlabel('area length (m)')
    ax.set_ylabel('area width (m)')
    ax.set_zlabel('time (min)')
    ax.set_xticks([0,100,200,300])
    ax.set_yticks([0,100,200,300])
    ax.set_zticks([0,4000,8000,12000,16000])
    ax.ticklabel_format(axis='z', style='sci', scilimits=(-2,2))

    ax.view_init(elev=13., azim=-70)
    ax.grid(True)
    # plt.show()
    plt.savefig('rotates.pdf')

def plot_xyt_track_all(s):
    chs = np.fromfile(pdir + 'lcr/ch_snapshot.dat')[nn : nn*2]
    chas = [c for i,c in enumerate(chs) if c == i][::s]
    fig, ax =  plot_xyt_track(chas, 0, 30000)
    
    ax.set_xlim3d(0, 300)
    ax.set_ylim3d(0, 300)
    ax.set_zlim3d(0, 16000)
    ax.set_xlabel('area length (m)')
    ax.set_ylabel('area width (m)')
    ax.set_zlabel('time (min)')
    ax.set_xticks([0,100,200,300])
    ax.set_yticks([0,100,200,300])
    ax.set_zticks([0,4000,8000,12000,16000])
    ax.ticklabel_format(axis='z', style='sci', scilimits=(-2,2))
    ax.grid(True)
    # plt.show()
    plt.savefig('chtrace.pdf')

def plot_inds(*algs):
    fig, ax = plt.subplots()
    for j,alg in enumerate(algs):
        dirs = [x[0] for x in os.walk('../results/'+alg)][1:]
        pas = [int(x.split('/')[-1]) for x in dirs]
        dirs = [x[1] for x in sorted(zip(pas, dirs))]
        fnds = []
        lnds = []
        ohds = []
        for d in dirs:
            try:
                inds = np.fromfile(d + '/inds_4.dat')
            except:
                continue
            
            fnd = inds[::5]
            lnd = inds[2::5]
            ohd = inds[3::5]
            fnds.append(np.average(fnd))
            lnds.append(np.median(lnd))
            ohds.append(np.average(ohd))
        
        for i,(fnd,lnd,ohd) in enumerate(zip(fnds, lnds, ohds)):
            if i == 1:
                ax.plot([fnds[i-1],fnd], [lnds[i-1],lnd], '-', color=colors[alg], linewidth=1.5, zorder=4-j, label=labels[alg])
            elif i > 0:
                ax.plot([fnds[i-1],fnd], [lnds[i-1],lnd], '-', color=colors[alg], linewidth=1.5, zorder=4-j)
        
        for i,(fnd,lnd,ohd) in enumerate(zip(fnds, lnds, ohds)):
            #ax.scatter(fnd, lnd, s = 10**(output / 1e9) * 0.5, alpha = 0.2, color=c)
            #ax.scatter(fnd, lnd, s = (30 * (output - 2.0e9) / 1e9)**2, alpha = 0.2, color=c)
            ss = ohd/3.0e3
            ax.scatter(fnd, lnd, s = ss, alpha = 0.37, color=colors[alg], zorder=4-j)
            ax.scatter(fnd, lnd, s = ss/2, alpha = 0.42, color=colors[alg], zorder=4-j)
            ax.scatter(fnd, lnd, s = ss/4, alpha = 0.47, color=colors[alg], zorder=4-j)
            ax.scatter(fnd, lnd, s = ss/8, alpha = 0.57, color=colors[alg], zorder=4-j)
            ax.scatter(fnd, lnd, s = ss/16, alpha = 0.77, color=colors[alg], zorder=4-j)
            ax.scatter(fnd, lnd, s = 7, alpha = 1, color=colors[alg], zorder=4-j)
            
    ax.set_xlabel('FND (min)')
    ax.set_ylabel('LND (min)')
    ax.ticklabel_format(axis='y', style='sci', scilimits=(-2,2))
    ax.ticklabel_format(axis='x', style='sci', scilimits=(-2,2))
    ths, tls = ax.get_legend_handles_labels()
    ax.legend([ths[0],ths[1],ths[3],ths[2]], [tls[0],tls[1],tls[3],tls[2]], loc=4)
    ax.grid(True)
    # plt.show()
    plt.savefig('inds.pdf')

def plot_ind_avg(col, *algs):
    n = {'fnd':0, 'hnd':1, 'lnd':2, 'ohd':3}[col]
    fig, ax = plt.subplots()
    for alg in algs:
        inds = []
        if alg in ('ifucm','heed'):
            fname = '/inds_4.dat'
        else:
            fname = '/inds_2.dat'
        for pdir in pdirs:
            ind = np.fromfile(pdir + alg + fname)[n::5]
            inds.append(sum(ind)/len(ind))
            # inds.append(np.median(ind))
        ax.plot(range(0,len(pdirs)), inds, alpha=0.77, color=colors[alg], lw=2, label=labels[alg])
        ax.scatter(range(0,len(pdirs)), inds, s=45, alpha=1.00, color=colors[alg])
    # plt.show()
    ax.set_xticks(range(-1,len(pdirs)+1))
    ax.set_xticklabels(['','3.3','3.3','3.3','4.4','5.5',''])
    ax.set_xlabel('density (nodes / 100$\mathrm{\mathsf{m^2}}$)')
    ax2 = ax.twiny()
    ax2.set_xticks(range(-1,len(pdirs)+1))
    ax2.set_xticklabels(['','200','250','300','300','300',''])
    ax2.set_xlabel('area length (m)')
    ax.grid(True)
    return fig, ax

def plot_fnd_avg():
    fig, ax = plot_ind_avg('fnd','lcr','ifucm','ecpf','heed')
    ax.set_ylim(0,3500)
    ax.set_ylabel('FND (min)')
    ax.ticklabel_format(axis='y', style='sci', scilimits=(-2,2))
    ax.legend()
    # plt.show()
    plt.savefig('fnd.pdf')

def plot_hnd_avg():
    fig, ax = plot_ind_avg('hnd','lcr','ifucm','ecpf','heed')
    ax.set_ylim(7000,15000)
    ax.set_ylabel('HND (min)')
    ax.ticklabel_format(axis='y', style='sci', scilimits=(-2,2))
    ax.legend()
    # plt.show()
    plt.savefig('hnd.pdf')

def plot_lnd_avg():
    fig, ax = plot_ind_avg('lnd','lcr','ifucm','ecpf','heed')
    ax.set_ylim(7000,24000)
    ax.set_ylabel('LND (min)')
    ax.ticklabel_format(axis='y', style='sci', scilimits=(-2,2))
    ax.legend()
    # plt.show()
    plt.savefig('lnd.pdf')

def plot_ohd_avg():
    fig, ax = plot_ind_avg('ohd','lcr','ifucm','ecpf','heed')
    ax.set_ylabel('overhead (byte)')
    ax.ticklabel_format(axis='y', style='sci', scilimits=(-2,2))
    ax.legend(loc=2)
    # plt.show()
    plt.savefig('ohd.pdf')


################################################################################################################

def plot_es(alg, snap):
    es = np.fromfile(pdir + alg + '/energy_snapshot.dat')[nn*snap : nn*(snap+1)]
    es[es<0] = 0
    fig, ax = plt.subplot(4,1,1)
    for i,e in enumerate(es):
        ax.scatter(i, e, s=10, alpha=0.5, color=colors[alg])
    # plt.show()

def plot_xyt_rotate_between(ta, tb):
    xyts = np.fromfile(pdir + 'lcr/rotate_backup.dat')
    
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
    ax.scatter(xs, ys, ts, color=cs)
    ax.set_xlim3d(0, 350)
    ax.set_ylim3d(0, 350)
    # plt.show()

def plot_inds_txt(*algs):
    fig, ax = plt.subplots()
    for alg in algs:
        if alg == 'lcr':
            inds = np.loadtxt(pdir + 'lcr/inds.txt', skiprows=1, usecols=range(2,7))
        if alg == 'heed':
            inds = np.loadtxt(pdir + 'heed/inds.txt', skiprows=1, usecols=range(1,6))
        if alg == 'ecpf':
            inds = np.loadtxt(pdir + 'ecpf/inds.txt', skiprows=1, usecols=range(1,6))
        if alg == 'ifucm':
            inds = np.loadtxt(pdir + 'ifucm/inds.txt', skiprows=1, usecols=range(1,6))
        
        for i,[fnd,hnd,lnd,overhead,output] in enumerate(inds):
            #ax.scatter(fnd, lnd, s = 10**(output / 1e9) * 0.5, alpha = 0.2, color=c)
            #ax.scatter(fnd, lnd, s = (30 * (output - 2.0e9) / 1e9)**2, alpha = 0.2, color=c)
            ax.scatter(fnd, lnd, s = overhead/3.0e3, alpha = 0.47, color=colors[alg])
            ax.scatter(fnd, lnd, s = 10, alpha = 1, color='black')
            if i > 0:
                ax.plot([inds[i-1][0],fnd], [inds[i-1][2],lnd], alpha = 0.7, color=colors[alg])
    # plt.show()

def plot_xyt_track(iads, ta, tb):
    xyts = np.fromfile(pdir + 'lcr/rotate_backup.dat')
    
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
    
    maxt = max([max(tracks[iad]['ts']) for iad in iads])
    mint = min([min(tracks[iad]['ts']) for iad in iads])

    for iad in iads:
        ds = [1-((x/300)**2 + (y/300)**2)**0.5 for x,y in zip(tracks[iad]['xs'], tracks[iad]['ys'])]
        cts = [1 - (x-mint)/(maxt-mint) for x in tracks[iad]['ts']]
        cs = cm.rainbow(cts)

        ax.scatter(tracks[iad]['xs'], tracks[iad]['ys'], tracks[iad]['ts'], s=10, alpha=1, color=cs, cmap=cm.spectral)
        ax.scatter(tracks[iad]['xs'], tracks[iad]['ys'], tracks[iad]['ts'], s=3, alpha=1, color='black')
        ax.plot(tracks[iad]['xs'], tracks[iad]['ys'], tracks[iad]['ts'], color='#777777')
    
    ax.set_xlim3d(0, 300)
    ax.set_ylim3d(0, 300)
    ax.set_zlim3d(0, maxt)
    ax.view_init(elev=36., azim=-114)
    
    # plt.show()
    return fig, ax

def summary(*algs):
    for alg in algs:
        times = np.fromfile(pdir + alg + '/time.dat')
        alives = np.fromfile(pdir + alg + '/alive_sum.dat')
        outputs = np.fromfile(pdir + alg + '/output_track.dat')
        overheads = np.fromfile(pdir + alg + '/rotate_overhead_track.dat')
        hnd = 0
        for i,x in enumerate(alives):
            if x < nn/2:
                hnd = times[i]
                break
        times = [times[i] for i,a in enumerate(alives) if 0 < a < nn-1]
        # print alg, ':', 'FND =', times[0], 'HND =', hnd, 'LND =', times[-1], 'OVERHEAD =', overheads[-1], 'OUTPUT =', outputs[-1]
        print alg, ':', '%0.2f %0.2f %0.2f %0.2f %0.2f' % (times[0], hnd, times[-1], overheads[-1], outputs[-1])

def plot_time_alive(*algs):
    fig, ax = plt.subplots()
    for alg in algs:
        times = np.fromfile(pdir + alg + '/time.dat')
        alives = np.fromfile(pdir + alg + '/alive_sum.dat')
        ax.plot(times, alives)
    # plt.show()

def plot_time_overhead(*algs):
    fig, ax = plt.subplots()
    for alg in algs:
        times = np.fromfile(pdir + alg + '/time.dat')
        alives = np.fromfile(pdir + alg + '/rotate_overhead_track.dat')
        ax.plot(times, alives)
    # plt.show()

def plot_time_energy(*algs):
    fig, ax = plt.subplots()
    for alg in algs:
        times = np.fromfile(pdir + alg + '/time.dat')
        energys = np.fromfile(pdir + alg + '/energy_sum.dat')
        ax.plot(times, energys)
    # plt.show()

def plot_time_output(*algs):
    fig, ax = plt.subplots()
    for alg in algs:
        times = np.fromfile(pdir + alg + '/time.dat')
        outputs = np.fromfile(pdir + alg + '/output_track.dat')
        ax.plot(times, outputs)
    # plt.show()

def plot_output_alive(*algs):
    fig, ax = plt.subplots()
    for alg in algs:
        outputs = np.fromfile(pdir + alg + '/output_track.dat')
        alives = np.fromfile(pdir + alg + '/alive_sum.dat')
        ax.plot(outputs, alives)
    # plt.show()


def plot_xy_chs(alg, snap):
    xs = np.fromfile(pdir + alg + '/nodes_x.dat')
    ys = np.fromfile(pdir + alg + '/nodes_y.dat')
    chs = np.fromfile(pdir + alg + '/ch_snapshot.dat')[nn*snap : nn*(snap+1)]
    
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
    # plt.show()

def write_time_fcd(alg, snaps):
    xs = np.fromfile(pdir + alg + '/nodes_x.dat')
    ys = np.fromfile(pdir + alg + '/nodes_y.dat')
    chss = np.fromfile(pdir + alg + '/ch_snapshot.dat')
    ess = np.fromfile(pdir + alg + '/energy_snapshot.dat')
    fcdss = []
    def dist(i,j):
        return ((xs[i]-xs[j])**2 + (ys[i]-ys[j])**2)**0.5
    for snap in snaps:
        print snap,
        chs = chss[nn*snap : nn*(snap+1)]
        es = ess[nn*snap : nn*(snap+1)]
        chs = [x for i,x in enumerate(chs) if x == i and es[i] > 0]
        nds = [i for i in range(1,len(es)) if es[i] > 0]
        fcds = [len([ch for ch in chs if dist(nd, ch) < 15.0]) for nd in nds]
        if len(fcds) == 0:
            fcdss.append(0.0)
            fcdss.append(0.0)
        else:
            fcdss.append(np.average(fcds))
            fcdss.append(np.std(fcds))
    fcdss = np.asarray(fcdss)
    fcdss.tofile(pdir + alg + "/fcd_track.dat")

def plot_time_fcd(s, *algs):
    fig, ax = plt.subplots()
    alphas = {'lcr':(1,0.4), 'heed':(0.35,0.25)}
    for alg in algs:
        time = np.fromfile(pdir + alg + '/time.dat')[::s]
        fcds = np.fromfile(pdir + alg + '/fcd_track.dat')
        avgs = fcds[0::2][::s]
        stds = fcds[1::2][::s]
   
        time = [x for i,x in enumerate(time) if avgs[i] > 0]
        stds = [x for i,x in enumerate(stds) if avgs[i] > 0]
        avgs = [x for i,x in enumerate(avgs) if avgs[i] > 0]

        ups = [avgs[i]+stds[i]/2 for i in xrange(0,len(avgs))]
        bts = [avgs[i]-stds[i]/2 for i in xrange(0,len(avgs))]
    
        ax.plot(time, avgs, color=colors[alg], alpha=alphas[alg][0], label=labels[alg])
        # ax.plot(time, ups, color=colors[alg])
        # ax.plot(time, bts, color=colors[alg])
        ax.fill_between(time, ups, bts, alpha=alphas[alg][1], color=colors[alg], label=labels[alg])
   
    ths, tls = ax.get_legend_handles_labels()
    ax.legend(ths[::-1], tls[::-1])
    ax.set_xlim(0,16000)
    ax.set_ylim(0,3.5)
    ax.set_xlabel('time (min)')
    ax.set_ylabel("error bar of all nodes' fcd")
    ax.ticklabel_format(axis='x', style='sci', scilimits=(-2,2))
    
    # plt.show()
    plt.savefig('fcd.pdf')

def plot_xy_hops(alg, snap):
    xs = np.fromfile(pdir + alg + '/nodes_x.dat')
    ys = np.fromfile(pdir + alg + '/nodes_y.dat')
    chs = np.fromfile(pdir + alg + '/ch_snapshot.dat')[nn*snap : nn*(snap+1)]
    hops = np.fromfile(pdir + alg + '/hop_snapshot.dat')[nn*snap : nn*(snap+1)]
    es = np.fromfile(pdir + alg + '/energy_snapshot.dat')[nn*snap : nn*(snap+1)]
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
    # plt.show()

def plot_xy_es(alg, snap):
    xs = np.fromfile(pdir + alg + '/nodes_x.dat')
    ys = np.fromfile(pdir + alg + '/nodes_y.dat')
    es = np.fromfile(pdir + alg + '/energy_snapshot.dat')[nn*snap : nn*(snap+1)]
    es[es<0] = 0

    fig, ax = plt.subplots()
    cs = cm.rainbow(1-es/2000)

    for i,x in enumerate(xs):
        ax.scatter(x, ys[i], s=10, alpha=0.5, color=cs[i])

    # plt.show()


