load 'energy_snapshot';
load 'node_snapshot';
figure;

energy_snapshot_period = MAX_PERIOD_COUNT;
%energy_snapshot_period = 5000;
node_snapshot_period = energy_snapshot_period;

scatter(1:NODE_NUM, energy_snapshot(energy_snapshot_period,:), '*');
axis([1, NODE_NUM, 0, E_INIT]);

figure;
hold on;
for i = 1 : NODE_NUM
    if energy_snapshot(energy_snapshot_period, i) <= 0
        scatter(nodes_x(i), nodes_y(i), 'k*');
    elseif energy_snapshot(energy_snapshot_period, i)/E_INIT < 0.25
        scatter(nodes_x(i), nodes_y(i), 'r*');
    elseif energy_snapshot(energy_snapshot_period, i)/E_INIT < 0.5
        scatter(nodes_x(i), nodes_y(i), 'y*');
    elseif energy_snapshot(energy_snapshot_period, i)/E_INIT < 0.75
        scatter(nodes_x(i), nodes_y(i), 'g*');
    else
        scatter(nodes_x(i), nodes_y(i), 'b*');
    end
end

title(['energy snapshot at ',num2str(energy_snapshot_period)]);

hold on;
%load('heed_ch.mat');
heed_ch = node_snapshot(node_snapshot_period,:);
for i = 1:NODE_NUM
    if energy_snapshot(energy_snapshot_period, heed_ch(i)+1) > 0 && energy_snapshot(energy_snapshot_period, i) > 0
        plot([nodes_x(i), nodes_x(heed_ch(i)+1)], [nodes_y(i), nodes_y(heed_ch(i)+1)]);
    end
    if energy_snapshot(energy_snapshot_period, heed_ch(i)+1) > 0 && heed_ch(i) == i-1
        circle(nodes_x(heed_ch(i)+1), nodes_y(heed_ch(i)+1), 1);
        circle(nodes_x(heed_ch(i)+1), nodes_y(heed_ch(i)+1), 1.3);
        %circle(nodes_x(heed_ch(i)+1), nodes_y(heed_ch(i)+1), CLUSTER_RADIUS);
    end
end
axis([0, AREA_SIZE_X, 0, AREA_SIZE_Y]);