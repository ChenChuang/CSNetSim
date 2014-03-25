import os
import numpy as np
from scipy import interpolate
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.markers as mks
from mpl_toolkits.mplot3d import Axes3D
import pylab as pla

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

colors = {'lcr':'red','ifucm':'green','ecpf':'orange','heed':'blue','lcrlamb':'purple'}
labels = {'lcr':'ACR','ifucm':'IFUC-M','ecpf':'ECPF','heed':'HEED(AMRP)','lcrlamb':'ACR-lambda'}

def plot_hot(alg, snapa, snapb):
    nxs = np.fromfile(pdir + alg + '/nodes_x.dat')
    nys = np.fromfile(pdir + alg + '/nodes_y.dat')
    esa = np.fromfile(pdir + alg + '/energy_snapshot.dat')[nn*snapa : nn*(snapa+1)]
    esb = np.fromfile(pdir + alg + '/energy_snapshot.dat')[nn*snapb : nn*(snapb+1)]
    hots = [esa[i]-esb[i]+1 for i in xrange(0,len(esa))]

    # fig, ax = plt.subplots()
    # ax.hist(hots, bins=50)
    # plt.yscale('log', nonposy='clip')
    # plt.show()
    # return
    
    mhot = max(hots)
    cs = cm.rainbow(hots/mhot)
    aps = 0.3+0.7*np.array(hots)/mhot
    ss = 3 + 157*np.array(hots)/mhot

    # fig, ax = plt.subplots()
    # ax.scatter(xs, ys, s=ss, alpha=0.5, color=cs)
    
    xs = pla.arange(0,300,2)
    ys = pla.arange(0,300,2)
    xss, yss = pla.meshgrid(xs, ys)
    zss = xss

    for ix,x in enumerate(xs):
        print x
        for iy,y in enumerate(ys):
            tmpzs = []
            maxi = -1
            for i in xrange(1,len(nxs)):
                if (nxs[i]-x)**2 + (nys[i]-y)**2 < 100:
                    if len(tmpzs) == 0 or hots[i] > max(tmpzs):
                        maxi = i
                    tmpzs.append(hots[i])
                    
            try:
                zss[iy][ix] = np.median(tmpzs) + (max(tmpzs) - np.median(tmpzs)) * (1-((nxs[maxi]-x)**2 + (nys[maxi]-y)**2)**0.5/10.0)
            except:
                zss[iy][ix] = 0
    
    pla.figure(1)
    pla.imshow(zss, interpolation='nearest')

    pla.show()
    
    return zss

def hist_rotate(*algs):
    if len(algs) == 0:
        algs = ['lcr','ifucm','ecpf','heed']
    rs = []
    fig, ax = plt.subplots()
    locs = range(1,len(algs)+1)
    hs = locs[:]
    gs = locs[:]
    for i,alg in enumerate(algs):
        o = np.fromfile(pdir + alg + '/rotate_overhead_track.dat')[-1]
        if alg == 'lcr':
            ts = np.fromfile(pdir + alg + '/newch_track.dat')[3::5]
        else:
            ts = np.fromfile(pdir + alg + '/newch_track.dat')[3::4]
        print alg, len(ts)
        hs[i] = len(ts)
        gs[i] = o
    rects = ax.bar(locs,hs,align='center',alpha=0.77,width=0.57)
    for rect in rects:
        ax.text(rect.get_x()+rect.get_width()/2, rect.get_height()+5777, '%d'%int(rect.get_height()),ha='center',va='bottom')
    ax.set_xticks(locs)
    ax.set_xticklabels([labels[x] for x in algs])
    ax.ticklabel_format(axis='y', style='sci', scilimits=(-2,2))
    ax.set_xlabel('algorithm')
    ax.set_ylabel('total rotation times')
    ax.grid(True)

    # plt.show()
    plt.tight_layout()
    plt.savefig('rtimes.pdf', transparent=True, bbox_inches="tight", pad_inches=0)

def hist_overhead(*algs):
    if len(algs) == 0:
        algs = ['lcr','ifucm','ecpf','heed']
    rs = []
    fig, ax = plt.subplots()
    locs = range(1,len(algs)+1)
    gs = locs[:]
    for i,alg in enumerate(algs):
        o = np.fromfile(pdir + alg + '/rotate_overhead_track.dat')[-1]
        gs[i] = o
    rects = ax.bar(locs,gs,align='center',alpha=0.77,width=0.57)
    for rect in rects:
        ax.text(rect.get_x()+rect.get_width()/2, rect.get_height()+5777, '%d'%int(rect.get_height()),ha='center',va='bottom')
    ax.set_xticks(locs)
    ax.set_xticklabels([labels[x] for x in algs])
    ax.ticklabel_format(axis='y', style='sci', scilimits=(-2,2))
    ax.set_xlabel('algorithm')
    ax.set_ylabel('total rotation overheads (mJ)')
    ax.set_ylim(0,1.8e6)
    ax.grid(True)

    # plt.show()
    plt.tight_layout()
    plt.savefig('rohd.pdf', transparent=True, bbox_inches="tight", pad_inches=0)

def hist_overhead_per(*algs):
    if len(algs) == 0:
        algs = ['lcr','ifucm','ecpf','heed']
    rs = []
    fig, ax = plt.subplots()
    locs = range(1,len(algs)+1)
    hs = locs[:]
    gs = locs[:]
    for i,alg in enumerate(algs):
        o = np.fromfile(pdir + alg + '/rotate_overhead_track.dat')[-1]
        if alg == 'lcr':
            ts = np.fromfile(pdir + alg + '/newch_track.dat')[3::5]
        else:
            ts = np.fromfile(pdir + alg + '/newch_track.dat')[3::4]
        hs[i] = len(ts)
        gs[i] = o / hs[i]
    rects = ax.bar(locs,gs,align='center',alpha=0.77,width=0.57)
    for rect in rects:
        ax.text(rect.get_x()+rect.get_width()/2, rect.get_height()+0.05, '%f'%rect.get_height(),ha='center',va='bottom')
    ax.set_xticks(locs)
    ax.set_xticklabels([labels[x] for x in algs])
    ax.ticklabel_format(axis='y', style='sci', scilimits=(-2,2))
    ax.set_xlabel('algorithm')
    ax.set_ylabel('overheads per rotation (mJ)')
    ax.set_ylim(0,8)
    ax.grid(True)

    # plt.show()
    plt.tight_layout()
    plt.savefig('rohdper.pdf', transparent=True, bbox_inches="tight", pad_inches=0)


def hist_rotate_lcr():
    ss = np.fromfile(pdir + 'lcr' + '/newch_track.dat')
    ts = ss[3::5]
    tys = ss[4::5]
    ds = {}
    # print len(tys)
    ds['migrate'] = len([x for x in tys if x == 0])
    ds['chdie'] = len([x for x in tys if x == 1])
    ds['chold'] = len([x for x in tys if x == 2])
    ds['notent'] = len([x for x in tys if x == 3])
    ds['mn_nonewch'] = len([x for x in tys if x == 4])
    ds['ch_nonewch'] = len([x for x in tys if x == 5])
    ds['newch_nomn'] = len([x for x in tys if x == 6])

    # for k,v in ds.iteritems():
        # print k,v
    
    dss = {}
    dss['Migration'] = ds['migrate'] + ds['notent'] + ds['chold']
    dss['Fission'] = ds['mn_nonewch']
    dss['Withering'] = ds['chdie']
    hs = [dss['Migration'], dss['Fission'],dss['Withering']]
    labs = ['Migration', 'Fission', 'Withering']
    locs = range(1,len(hs)+1)

    fig, ax = plt.subplots()
    rects = ax.bar(locs, hs, align='center', alpha=0.77,width=0.57)
    for rect in rects:
        ax.text(rect.get_x()+rect.get_width()/2, rect.get_height()+177, '%d'%int(rect.get_height()),ha='center',va='bottom')
    ax.set_xticks(locs)
    ax.set_xticklabels(labs)
    ax.set_xlabel('cause of rotation in ' + labels['lcr'])
    ax.set_ylabel('times')
    ax.set_ylim(0,18000)
    ax.ticklabel_format(axis='y', style='sci', scilimits=(-2,2))
    ax.grid(True)
    
    # plt.show()
    plt.tight_layout()
    plt.savefig('rcauses.pdf', transparent=True, bbox_inches="tight", pad_inches=0)



def plot_time_rotate_lcr():
    ss = np.fromfile(pdir + 'lcr' + '/newch_track.dat')
    ts = ss[3::5]
    tys = ss[4::5]
    g = lambda x : (x == 0 or x == 3 or x == 2) and 1 or 0
    ms = np.cumsum([g(x) for x in tys])
    g = lambda x : (x == 4) and 1 or 0
    fs = np.cumsum([g(x) for x in tys])
    g = lambda x : (x == 1) and 1 or 0
    ws = np.cumsum([g(x) for x in tys])

    fig, ax = plt.subplots()
    ax.plot(ts, ms)
    ax.plot(ts, fs)
    ax.plot(ts, ws)
    ax.set_xlabel('cause of rotation in ' + labels['lcr'])
    ax.set_ylabel('times')
    ax.set_ylim(0,18000)
    ax.ticklabel_format(axis='y', style='sci', scilimits=(-2,2))
    ax.grid(True)
    
    plt.show()


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
    plt.tight_layout()
    # plt.show()
    plt.savefig('ess.pdf', transparent=True, bbox_inches="tight", pad_inches=0)

def plot_xyt_rotate(s):
    xyts = np.fromfile(pdir + 'lcr/rotate.dat')
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
    plt.show()

    # plt.tight_layout()
    # plt.savefig('rotates.pdf', transparent=True, bbox_inches="tight", pad_inches=0)

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

    plt.tight_layout()
    plt.savefig('chtrace.pdf', transparent=True, bbox_inches="tight", pad_inches=0.5)

def plot_inds(*algs):
    if len(algs) == 0:
        algs = ['lcr','ifucm','heed','ecpf']
    fnames = {'lcr':'/inds_7.dat','ifucm':'/inds_4.dat','ecpf':'/inds_4.dat','heed':'/inds_4.dat','lcrlamb':'/inds_8.dat'}
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
                inds = np.fromfile(d + fnames[alg])
            except:
                continue
            
            fnd = inds[::5]
            lnd = inds[2::5]
            ohd = inds[3::5]
            fnds.append(np.average(fnd))
            lnds.append(np.median(lnd))
            ohds.append(np.average(ohd))

        if alg == 'lcr':
            pass
             # print dirs, fnds, lnds, ohds
        
        for i,(fnd,lnd,ohd) in enumerate(zip(fnds, lnds, ohds)):
            if i == 1:
                ax.plot([fnds[i-1],fnd], [lnds[i-1],lnd], '-', alpha=0.77, color=colors[alg], linewidth=1.5, zorder=4-j, label=labels[alg])
            elif i > 0:
                ax.plot([fnds[i-1],fnd], [lnds[i-1],lnd], '-', alpha=0.77, color=colors[alg], linewidth=1.5, zorder=4-j)
        
        for i,(fnd,lnd,ohd) in enumerate(zip(fnds, lnds, ohds)):
            ax.scatter(fnd, lnd, s = 77, alpha=0.77, color=colors[alg], zorder=4-j)
            # ss = ohd/3.0e3
            # ax.scatter(fnd, lnd, s = ss, alpha = 0.37, color=colors[alg], zorder=4-j)
            # ax.scatter(fnd, lnd, s = ss/2, alpha = 0.42, color=colors[alg], zorder=4-j)
            # ax.scatter(fnd, lnd, s = ss/4, alpha = 0.47, color=colors[alg], zorder=4-j)
            # ax.scatter(fnd, lnd, s = ss/8, alpha = 0.57, color=colors[alg], zorder=4-j)
            # ax.scatter(fnd, lnd, s = ss/16, alpha = 0.77, color=colors[alg], zorder=4-j)
            # ax.scatter(fnd, lnd, s = 7, alpha = 1, color=colors[alg], zorder=4-j)
            
    ax.set_xlabel('FND (min)')
    ax.set_ylabel('LND (min)')
    ax.set_xlim(200,1300)
    ax.set_ylim(0.9e4, 1.75e4)
    ax.ticklabel_format(axis='y', style='sci', scilimits=(-2,2))
    ax.ticklabel_format(axis='x', style='sci', scilimits=(-2,2))
    ths, tls = ax.get_legend_handles_labels()
    ax.legend([ths[0],ths[1],ths[3],ths[2]], [tls[0],tls[1],tls[3],tls[2]], loc=3)
    ax.grid(True)
    plt.show()
    # plt.tight_layout()
    # plt.savefig('inds.pdf', transparent=True, bbox_inches="tight", pad_inches=0)

def plot_lamb_ind(col):
    n = {'fnd':0, 'hnd':1, 'lnd':2, 'ohd':3}[col]
    fname = '/inds_9.dat'
    lambs = [4,6,8,10,12,14,16]
    # lambs = [10,20,30,40,50]
    # lambs = [10,100,300,500,1000,1500,2000,6000,10000]
    # locs = range(0,len(lambs))
    locs = lambs[:]
    cs = ['blue','red','green','orange','purple']
    ls = [r'200m$\times$200m, 1333 nodes',
          r'250m$\times$250m, 2083 nodes', 
          r'300m$\times$300m, 3000 nodes',
          r'300m$\times$300m, 4000 nodes',
          r'300m$\times$300m, 5000 nodes',]
    fig, ax = plt.subplots()
    # for i,pdir in enumerate(sorted(pdirs)):
    for i,pdir in enumerate(sorted([pdirs[3],])):
        inds = []
        for lamb in lambs:
            ind = np.fromfile(pdir + 'lcrlamb/' + str(lamb) + fname)[n::5]
            inds.append(int(np.average(sorted(ind)[-10:])))
            # inds.append(np.average(ind))
        ax.plot(locs, inds, alpha=0.77, color=cs[i], lw=2, label=ls[i])
        ax.scatter(locs, inds, s=45, color=cs[i], alpha=1.00)
    ax.grid(True)
    ax.ticklabel_format(axis='y', style='sci', scilimits=(-2,2))
    ax.set_xlabel(r'$\lambda$')
    # ax.legend()
    return fig, ax

def plot_lamb_fnd():
    fig, ax = plot_lamb_ind('fnd')
    ax.set_ylim(1000,1800)
    ax.set_ylabel('FND (min)')
    # plt.show()
    plt.tight_layout()
    plt.savefig('lambfnd.pdf', transparent=True, bbox_inches="tight", pad_inches=0)

def plot_lamb_hnd():
    fig, ax = plot_lamb_ind('hnd')
    ax.set_ylim(10000,10800)
    ax.set_ylabel('HND (min)')
    # plt.show()
    plt.tight_layout()
    plt.savefig('lambhnd.pdf', transparent=True, bbox_inches="tight", pad_inches=0)

def plot_lamb_lnd():
    fig, ax = plot_lamb_ind('lnd')
    ax.set_ylim(16100,16900)
    ax.set_ylabel('LND (min)')
    # plt.show()
    plt.tight_layout()
    plt.savefig('lamblnd.pdf', transparent=True, bbox_inches="tight", pad_inches=0)

def plot_lamb_ohd():
    fig, ax = plot_lamb_ind('ohd')
    ax.set_ylim(2.38e4, 2.58e4)
    ax.set_ylabel('overhead (mJ)')
    # plt.show()
    plt.tight_layout()
    plt.savefig('lambohd.pdf', transparent=True, bbox_inches="tight", pad_inches=0)


def plot_ind_avg(col, *algs):
    n = {'fnd':0, 'hnd':1, 'lnd':2, 'ohd':3}[col]
    # fnames = {'lcr':'/inds_8.dat','ifucm':'/inds_4.dat','ecpf':'/inds_2.dat','heed':'/inds_4.dat'}
    fnames = {'lcr':'/inds_8.dat','ifucm':'/inds_4.dat','ecpf':'/inds_10.dat','heed':'/inds_4.dat'}
    fig, ax = plt.subplots()
    for alg in algs:
        inds = []
        for pdir in pdirs:
            ind = np.fromfile(pdir + alg + fnames[alg])[n::5]
            inds.append(np.average(sorted(ind)[-10:]))
            # print alg, pdir, inds[-1]
            # inds.append(np.average(ind))
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
    ax.set_ylim(0,3000)
    ax.set_ylabel('FND (min)')
    ax.ticklabel_format(axis='y', style='sci', scilimits=(-2,2))
    ax.legend()
    # plt.show()
    plt.tight_layout()
    plt.savefig('fnd.pdf', transparent=True, bbox_inches="tight", pad_inches=0)

def plot_hnd_avg():
    fig, ax = plot_ind_avg('hnd','lcr','ifucm','ecpf','heed')
    ax.set_ylim(7000,16000)
    ax.set_ylabel('HND (min)')
    ax.ticklabel_format(axis='y', style='sci', scilimits=(-2,2))
    ax.legend()
    # plt.show()
    plt.tight_layout()
    plt.savefig('hnd.pdf', transparent=True, bbox_inches="tight", pad_inches=0)

def plot_lnd_avg():
    fig, ax = plot_ind_avg('lnd','lcr','ifucm','ecpf','heed')
    ax.set_ylim(7000,24000)
    ax.set_ylabel('LND (min)')
    ax.ticklabel_format(axis='y', style='sci', scilimits=(-2,2))
    ax.legend()
    # plt.show()
    plt.tight_layout()
    plt.savefig('lnd.pdf', transparent=True, bbox_inches="tight", pad_inches=0)

def plot_ohd_avg():
    fig, ax = plot_ind_avg('ohd','lcr','ifucm','ecpf','heed')
    ax.set_ylabel('overhead (mJ)')
    ax.ticklabel_format(axis='y', style='sci', scilimits=(-2,2))
    # ax.set_ylim(0,4e6)
    ax.legend(loc=2)
    # plt.show()
    plt.tight_layout()
    plt.savefig('ohd.pdf', transparent=True, bbox_inches="tight", pad_inches=0)

def plot_eq_cd():
    # fig, ax = plt.subplots(figsize=(8,3.7))
    fig, ax = plt.subplots()
    x = np.linspace(1.0, 7.0, 1000)
    xx = np.linspace(1.0, 7.0, 7)
    y = np.exp(-x)
    yy = np.exp(-xx)
    ax.plot(x, y)
    ax.scatter(xx, yy)
    ax.vlines(xx, [0], yy, linestyles='dashed') 
    ax.set_ylim(0,0.4)
    ax.set_xlabel(r'$F_{cd,k} = |\mathrm{\mathsf{SNC}}_{k}\cap \mathrm{\mathsf{SCH}}|$')
    ax.set_ylabel(r'$\exp(-F_{cd,k})$')
    ax.grid(True)
    plt.tight_layout()
    plt.savefig('eqcd.pdf', transparent=True, bbox_inches="tight", pad_inches=0)
    # plt.show()

def plot_eq_quan():
    fig, ax = plt.subplots()
    x = np.linspace(0, 1.1, 12)
    x[-1] = x[-2]
    y = np.linspace(-1, 10, 12)
    y[0] = 0
    labs = [str(int(yy)) for yy in y]
    labs[-2] = r'$\lambda-1$'
    labs[-1] = r'$\lambda$'
    ax.step(x,y)

    nx = [0.22,0.23,0.234, 0.27, 0.31, 0.54, 0.67]
    cnx = [0.91, 0.93, 0.934, 0.95, 0.98, 0.99]
    ny = [np.floor(nxx*10) for nxx in nx]
    cny = [np.floor(nxx*10) for nxx in cnx]
    ax.scatter(nx,ny,color='blue',alpha=1,label='non-candidates')
    ax.scatter(cnx,cny,color='red',alpha=1,label='candidates')
    ax.vlines(nx, [0], ny, linestyles='dashed')
    ax.vlines(cnx, [0], cny, linestyles='dashed')

    ax.set_xlabel('factor')
    ax.set_ylabel('quantified factor')
    ax.set_yticks(y)
    ax.set_yticklabels(labs)
    ax.set_xlim(0.0,1.0)
    ax.set_ylim(0.0,10)
    ax.grid(True)
    ax.legend(loc=2)
    plt.tight_layout()
    plt.savefig('eqquan.pdf', transparent=True, bbox_inches="tight", pad_inches=0)
    # plt.show()

################################################################################################################

def plot_es(alg, snap):
    s = 2
    es = np.fromfile(pdir + alg + '/energy_snapshot.dat')[nn*snap : nn*(snap+1)][::2]
    # es[es<0] = 0
    fig, ax = plt.subplots()
    ls = range(0,len(es)*s,s)
    ax.scatter(ls, es, s=10, alpha=0.77, color=colors[alg])
    ax.set_xlabel(labels[alg] + ': node ID')
    ax.set_ylabel('residual energy (mJ)')
    ax.set_xticks([0,1000,2000,3000,4000])
    ax.set_xlim(0,4000)
    ax.set_ylim(0,2000)
    ax.ticklabel_format(axis='x', style='sci', scilimits=(-2,2))
    ax.ticklabel_format(axis='y', style='sci', scilimits=(-2,2))
    ax.grid(True)
    
    # plt.show()
    
    plt.tight_layout()
    plt.savefig('es_' + alg + '.pdf', transparent=True, bbox_inches="tight", pad_inches=0)

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
    # ax.view_init(elev=36., azim=-114)
    ax.view_init(elev=13., azim=-70)
    
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
    plt.show()

def write_time_chratio(alg):
    chss = np.fromfile(pdir + alg + '/ch_snapshot.dat')
    ess = np.fromfile(pdir + alg + '/energy_snapshot.dat')
    chns = []
    ndns = []
    rs = []
    snap = 0
    s = 1
    while True:
        if nn*(snap+1) > len(chss):
            break
        chs = chss[nn*snap : nn*(snap+1)]
        es = ess[nn*snap : nn*(snap+1)]
        chn = 0
        ndn = 0
        for i in xrange(0,len(chs)):
            if chs[i] == i and es[i] > 0:
                chn = chn + 1
            if es[i] > 0:
                ndn = ndn + 1
        chns.append(chn)
        ndns.append(ndn)
        print chn,ndn
        if chn > 0:
            rs.append(chn/float(ndn))
        elif ndn > 0 and len(rs) > 0:
            rs.append(rs[-1])
        else:
            rs.append(0.0)
        snap = snap + s
    
    np.asarray(chns).tofile(pdir + alg + "/chn_track.dat")
    np.asarray(ndns).tofile(pdir + alg + "/ndn_track.dat")
    np.asarray(rs).tofile(pdir + alg + "/chr_track.dat")
   
def plot_time_chratio(s, *algs):
    fig, ax = plt.subplots()
    for alg in algs:
        chrs = np.fromfile(pdir + alg + '/chr_track.dat')[::s]
        time = np.fromfile(pdir + alg + '/time.dat')[::s]
        ax.plot(time, chrs, color=colors[alg], label=labels[alg])
    
    ths, tls = ax.get_legend_handles_labels()
    ax.legend(ths[::-1], tls[::-1], loc=2)
    ax.set_xlim(0,16000)
    ax.set_ylim(0,1.0)
    ax.set_xlabel('time (min)')
    ax.set_ylabel("cluster head ratio")
    ax.ticklabel_format(axis='x', style='sci', scilimits=(-2,2))
    ax.grid(True)
    
    # plt.show()
    plt.tight_layout()
    plt.savefig('chr.pdf', transparent=True, bbox_inches="tight", pad_inches=0)
    

def write_time_fcd(alg, snaps):
    xs = np.fromfile(pdir + alg + '/nodes_x.dat')
    ys = np.fromfile(pdir + alg + '/nodes_y.dat')
    chss = np.fromfile(pdir + alg + '/ch_snapshot.dat')
    ess = np.fromfile(pdir + alg + '/energy_snapshot.dat')
    time = np.fromfile(pdir + alg + '/time.dat')
    fcdss = []
    if snaps is None:
        snaps = range(0, len(time))
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
    ax.grid(True)
    
    # plt.show()
    plt.tight_layout()
    plt.savefig('fcd.pdf', transparent=True, bbox_inches="tight", pad_inches=0)

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
    plt.show()

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

def plot_xy_des(alg, snapa, snapb):
    xs = np.fromfile(pdir + alg + '/nodes_x.dat')
    ys = np.fromfile(pdir + alg + '/nodes_y.dat')
    chs = np.fromfile(pdir + alg + '/ch_snapshot.dat')[nn*snapa : nn*(snapa+1)]
    hops = np.fromfile(pdir + alg + '/hop_snapshot.dat')[nn*snapa : nn*(snapa+1)]
    esa = np.fromfile(pdir + alg + '/energy_snapshot.dat')[nn*snapa : nn*(snapa+1)]
    esb = np.fromfile(pdir + alg + '/energy_snapshot.dat')[nn*snapb : nn*(snapb+1)]
    
    es = esa
    esa[esa<0] = 0
    esb[esb<0] = 0
    des = esa - esb

    thrd = 0
    chxs = [xs[i] for i,c in enumerate(chs) if c == i and es[i] > 0 and des[i] <= thrd]
    chys = [ys[i] for i,c in enumerate(chs) if c == i and es[i] > 0 and des[i] <= thrd]
    tchs = [i for i,c in enumerate(chs) if c == i and es[i] > 0 and des[i] > thrd]
    tchxs = [xs[i] for i,c in enumerate(chs) if c == i and es[i] > 0 and des[i] > thrd]
    tchys = [ys[i] for i,c in enumerate(chs) if c == i and es[i] > 0 and des[i] > thrd]
    mnxs = [xs[i] for i,c in enumerate(chs) if c != i and c >= 0 and es[i] > 0]
    mnys = [ys[i] for i,c in enumerate(chs) if c != i and c >= 0 and es[i] > 0]
    
    chdes = [des[i] for i,c in enumerate(chs) if c == i and es[i] > 0 and des[i] <= thrd]
    tchdes = [des[i] for i,c in enumerate(chs) if c == i and es[i] > 0 and des[i] > thrd]
    mndes = [des[i] for i,c in enumerate(chs) if c != i and es[i] > 0]

    fig, ax = plt.subplots()

    lam = 0.7
    chcs = cm.rainbow(lam + chdes/max(des)*(1-lam))
    tchcs = cm.rainbow(lam + tchdes/max(des)*(1-lam))
    mncs = cm.rainbow(lam + mndes/max(des)*(1-lam))
    
    ss = ((chdes/max(des))*40)**1.5
    tss = ((tchdes/max(des))*40)**1.5
    # mss = ((mndes/max(des))*30)**1.5
    mss = 10

    maxi = -1
    pis = []
    for i in tchs:
        if des[i] > 18:
            pis.append(i)
            if des[i] > maxi:
                maxi = i

    tpis = [i for (de,i) in sorted([(de,i) for i,de in enumerate(des)])[-3:]]
    while True:
        a = [i for i in pis if hops[i] in tpis]
        pis = [i for i in pis if i not in a]
        if len(a) > 0:
            tpis = tpis + a
        else:
            break

    for i in tpis:
        ax.plot([xs[i], xs[hops[i]]], [ys[i], ys[hops[i]]], color=cm.rainbow(lam + des[i]/des[maxi]*(1-lam)), lw=(1 + 2*des[i]/des[maxi]), alpha=0.8, zorder=1)

    # ax.scatter(mnxs, mnys, s=mss, alpha=0.1, color=mncs)
    ax.scatter(chxs, chys, s=ss, alpha=0.77, color=chcs, zorder=2)
    ax.scatter(tchxs, tchys, s=tss, alpha=0.8, color=tchcs, zorder=2)
    # ax.scatter(mnxs, mnys, s=10, alpha=0.15, color=mncs)

    ax.scatter(0, 0, s=80, alpha=1, color='green', zorder=2)

    ax.set_xlim(-10,310)
    ax.set_ylim(-10,310)
    ax.set_xlabel('area length (m)')
    ax.set_ylabel('area width (m)')
    ax.grid(True)
    # plt.show()
    plt.tight_layout()
    plt.savefig('xydes.pdf', transparent=True, bbox_inches="tight", pad_inches=0)



def hist_des(snapa, snapb, *algs):
    fig, ax = plt.subplots()
    chdess = []
    wss = []
    for alg in algs:
        chs = np.fromfile(pdir + alg + '/ch_snapshot.dat')[nn*snapa : nn*(snapa+1)]
        esa = np.fromfile(pdir + alg + '/energy_snapshot.dat')[nn*snapa : nn*(snapa+1)]
        esb = np.fromfile(pdir + alg + '/energy_snapshot.dat')[nn*snapb : nn*(snapb+1)]
    
        esa[esa<0] = 0
        esb[esb<0] = 0
        des = esa - esb

        rchs = [c for i,c in enumerate(chs) if c != i]

        # chdes = [des[i] for i,c in enumerate(chs) if c == i and i in rchs and esa[i] > 0 and esb[i] > 0]
        chdes = [des[i] for i,c in enumerate(chs) if c == i and esa[i] > 0 and esb[i] > 0]
        chdess.append(chdes)
        ws = np.ones_like(chdes)/len(chdes)
        wss.append(ws)
    
        ax.hist(chdes, weights=ws, alpha=0.77, label=[labels[alg] for alg in algs], histtype='bar', bins=70)
    # ax.legend()
    
    # plt.yscale('log', nonposy='clip')
    ax.set_xlabel('Energy Consumptino Rate (mJ/min)')
    ax.set_yticks(np.linspace(0,0.25,5))
    ax.set_yticklabels(["%0.2f".rjust(4) % (100.0*x) for x in np.linspace(0,0.25,5)])
    ax.set_ylabel('Percentage (%)')
    ax.grid(True)
    
    # plt.show()
    plt.tight_layout()
    plt.savefig('hdes.pdf', transparent=True, bbox_inches="tight", pad_inches=0)



#########################################################################################

def plot_move_1():
    alg = 'heed'
    snap = 200
    xs = np.fromfile(pdir + alg + '/nodes_x.dat')
    ys = np.fromfile(pdir + alg + '/nodes_y.dat')
    chs = np.fromfile(pdir + alg + '/ch_snapshot.dat')[nn*snap : nn*(snap+1)]
    
    xs[2415] = xs[2415]-5
    
    # np.asarray(chs).tofile("chs.dat")
    
    chxs = [xs[i] for i,c in enumerate(chs) if c == i and c != 2925 ]
    chys = [ys[i] for i,c in enumerate(chs) if c == i and c != 2925] 

    ri = 2536

    # mnxs = [xs[i] for i,c in enumerate(chs) if c != i and c > 0]
    # mnys = [ys[i] for i,c in enumerate(chs) if c != i and c > 0]

    # np.asarray(xs).tofile("xs.dat")
    # np.asarray(ys).tofile("ys.dat")
    
    fig, ax = plt.subplots()

    # ax.scatter(xs[0], ys[0], s = 40, color="green")
    ax.scatter(chxs, chys, s = 15, alpha = 0.3, color="blue")
    ax.scatter(chxs, chys, s = 15000, alpha = 0.17, color="blue")
    # ax.scatter(mnxs, mnys, s = 10, alpha = 0.5, color="blue")

    ax.scatter(xs[ri], ys[ri], s = 15000, alpha=0.3, facecolors='none', edgecolors='red',linewidths=2)
   
    ax.set_xlim(110,210)
    ax.set_ylim(160,260)
   
    ax.set_xticklabels([])
    ax.set_yticklabels([])
    plt.show()
    # plt.tight_layout()
    # plt.savefig('move1.pdf', transparent=True, bbox_inches="tight", pad_inches=0)

def plot_move_2(ax):
    alg = 'heed'
    snap = 200
    xs = np.fromfile(pdir + alg + '/nodes_x.dat')
    ys = np.fromfile(pdir + alg + '/nodes_y.dat')
    chs = np.fromfile(pdir + alg + '/ch_snapshot.dat')[nn*snap : nn*(snap+1)]

    xs[2415] = xs[2415]-5
    
    ri = 2536

    chxs = [xs[i] for i,c in enumerate(chs) if c == i and c != ri and c != 2925]
    chys = [ys[i] for i,c in enumerate(chs) if c == i and c != ri and c != 2925] 

    fig, ax = plt.subplots()

    ax.scatter(chxs, chys, s = 15, alpha = 0.3, color="blue")
    ax.scatter(chxs, chys, s = 15000, alpha = 0.17, color="blue")

    ax.scatter(xs[ri], ys[ri], s = 15, alpha = 1, color="purple")
    ax.scatter(xs[ri], ys[ri], s = 15000, alpha=0.7, color='red',facecolors='none', edgecolors='purple',linewidths=2,linestyles='dashed')

    ax.scatter(xs[ri]+2, ys[ri]-8, s = 15, alpha = 1, color='green')
    ax.scatter(xs[ri]+2, ys[ri]-8, s = 15000, alpha=0.17, color='blue')
    ax.scatter(xs[ri]+2, ys[ri]-8, s = 15000, alpha=0.9, facecolors='none', edgecolors='green',linewidths=2)

    ax.arrow(xs[ri]+0.4, ys[ri]-1.6, 0.8, -3.2, head_width=1.7, head_length=2, shape='right', fc='#222222', ec='#222222')
   
    ri = 1686
    ax.scatter(xs[ri], ys[ri], s = 15000, alpha=0.3, facecolors='none', edgecolors='red',linewidths=2)

    #ax.set_xlim(110,210)
    #ax.set_ylim(160,260)
    
    ax.set_xticklabels([])
    ax.set_yticklabels([])
    
    plt.show()
    # plt.tight_layout()
    # plt.savefig('move2.pdf', transparent=True, bbox_inches="tight", pad_inches=0)

def plot_move_3(ax):
    alg = 'heed'
    snap = 200
    xs = np.fromfile(pdir + alg + '/nodes_x.dat')
    ys = np.fromfile(pdir + alg + '/nodes_y.dat')
    chs = np.fromfile(pdir + alg + '/ch_snapshot.dat')[nn*snap : nn*(snap+1)]

    xs[2415] = xs[2415]-5
    xs[2536] = xs[2536]+2
    ys[2536] = ys[2536]-8

    ri = 1686

    chxs = [xs[i] for i,c in enumerate(chs) if c == i and c != ri and c != 2925]
    chys = [ys[i] for i,c in enumerate(chs) if c == i and c != ri and c != 2925] 

    fig, ax = plt.subplots()

    ax.scatter(chxs, chys, s = 15, alpha = 0.3, color="blue")
    ax.scatter(chxs, chys, s = 15000, alpha = 0.17, color="blue")

    ax.scatter(xs[ri], ys[ri], s = 15, alpha = 1, color="purple")
    ax.scatter(xs[ri], ys[ri], s = 15000, alpha=0.7, color='red',facecolors='none', edgecolors='purple',linewidths=2,linestyles='dashed')

    ax.scatter(xs[ri]-1, ys[ri]-6, s = 15, alpha = 1, color='green')
    ax.scatter(xs[ri]-1, ys[ri]-6, s = 15000, alpha=0.17, color='blue')
    ax.scatter(xs[ri]-1, ys[ri]-6, s = 15000, alpha=0.9, facecolors='none', edgecolors='green',linewidths=2)

    ax.arrow(xs[ri]-0.25, ys[ri]-1.5, -0.2, -1.2, head_width=1.7, head_length=2, shape='right', fc='#222222', ec='#222222')

    ax.scatter(xs[ri]+5, ys[ri]+12, s = 15, alpha = 1, color='blue')
    ax.scatter(xs[ri]+5, ys[ri]+12, s = 15000, alpha=0.17, color='blue')
    ax.scatter(xs[ri]+5, ys[ri]+12, s = 15000, alpha=0.5, facecolors='none', edgecolors='blue',linewidths=2)

    ax.arrow(xs[ri]+0.5, ys[ri]+1.2, 3.2, 7.68, head_width=1.7, head_length=2, shape='right', fc='#222222', ec='#222222')


    ax.set_xlim(110,210)
    ax.set_ylim(160,260)
    
    ax.set_xticklabels([])
    ax.set_yticklabels([])
    
    plt.show()
    # plt.tight_layout()
    # plt.savefig('move3.pdf', transparent=True, bbox_inches="tight", pad_inches=0)

def plot_move():
    axs = [None, None, None]
    fig, (axs[0], axs[1], axs[2]) = plt.subplots(1, 3, sharex='col', sharey='row')
    
    plot_move_1(axs[0])
    plot_move_2(axs[1])
    plot_move_3(axs[2])
    
    plt.show()
