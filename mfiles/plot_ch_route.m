plot_nodes;
hold on;
load('ch_route.mat');
load('heed_ch.mat');
load 'energy_snapshot';
for i = 1:NODE_NUM
    if energy_snapshot(MAX_PERIOD_COUNT, i) > 0
        scatter(nodes_x(heed_ch(i)+1), nodes_y(heed_ch(i)+1), '*r');
    end
   %circle(nodes_x(heed_ch(i)+1), nodes_y(heed_ch(i)+1), CLUSTER_RADIUS);
end
for i = 1:NODE_NUM
    if energy_snapshot(MAX_PERIOD_COUNT, i) > 0
        if ch_route(i) == SINK_ADDR
            plot([nodes_x(i), SINK_X], [nodes_y(i), SINK_Y]); 
        elseif ch_route(i) >= 0
            plot([nodes_x(i), nodes_x(ch_route(i)+1)], [nodes_y(i), nodes_y(ch_route(i)+1)]);
            %circle(nodes_x(ch_route(i)+1), nodes_y(ch_route(i)+1), MAX_RADIUS);
        end
    end
end
title('CH route');