load('nodes_x.mat')
load('nodes_y.mat')
load 'hop_snapshot';
load 'ch_snapshot';
load 'energy_snapshot';
figure;

snap = 9;

axis([0, AREA_SIZE_X, 0, AREA_SIZE_Y]);
scatter(nodes_x, nodes_y, '+');
hold on;
scatter(SINK_X, SINK_Y, '*');

hops = hop_snapshot(:,snap);
chs = ch_snapshot(:,snap);

for i = 2:NODE_NUM
    if (energy_snapshot(hops(i)+1, snap) > 0 || hops(i) == SINK_ADDR) && energy_snapshot(i, snap) > 0
        if chs(i) == i-1
            plot([nodes_x(i), nodes_x(hops(i)+1)], [nodes_y(i), nodes_y(hops(i)+1)], 'r','linewidth',2);
        else
            plot([nodes_x(i), nodes_x(hops(i)+1)], [nodes_y(i), nodes_y(hops(i)+1)]);
        end
    end
end
axis([0, AREA_SIZE_X, 0, AREA_SIZE_Y]);