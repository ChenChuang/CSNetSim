%dir = 'heed/';
dir = 'lcr/';
load([dir,'nodes_x.mat']);
load([dir,'nodes_y.mat']);
load([dir,'hop_snapshot.mat']);
load([dir,'ch_snapshot.mat']);
load([dir,'energy_snapshot.mat']);
figure;

snap = 102;

axis([0, AREA_SIZE_X, 0, AREA_SIZE_Y]);
scatter(nodes_x, nodes_y, '+');
hold on;
scatter(SINK_X, SINK_Y, '*');

chs = ch_snapshot(:,snap);
for i = 2:NODE_NUM
    if chs(i) < 0
        continue;
    end
    if energy_snapshot(chs(i)+1, snap) > 0 && energy_snapshot(i, snap) > 0
        plot([nodes_x(i), nodes_x(chs(i)+1)], [nodes_y(i), nodes_y(chs(i)+1)]);
    end
    if energy_snapshot(chs(i)+1, snap) > 0 && chs(i) == i-1
        circle(nodes_x(chs(i)+1), nodes_y(chs(i)+1), 1);
        circle(nodes_x(chs(i)+1), nodes_y(chs(i)+1), 1.3);
        %circle(nodes_x(heed_ch(i)+1), nodes_y(heed_ch(i)+1), CLUSTER_RADIUS);
    end
end
axis([0, AREA_SIZE_X, 0, AREA_SIZE_Y]);