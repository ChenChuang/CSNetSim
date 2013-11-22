plot_nodes;
hold on;
load('heed_ch.mat');
for i = 1:NODE_NUM
   plot([nodes_x(i), nodes_x(heed_ch(i)+1)], [nodes_y(i), nodes_y(heed_ch(i)+1)]);
   %scatter(nodes_x(heed_ch(i)+1), nodes_y(heed_ch(i)+1), '*r');
   %circle(nodes_x(heed_ch(i)+1), nodes_y(heed_ch(i)+1), CLUSTER_RADIUS);
   circle(nodes_x(heed_ch(i)+1), nodes_y(heed_ch(i)+1), 1);
   circle(nodes_x(heed_ch(i)+1), nodes_y(heed_ch(i)+1), 1.3);
end
title('HEED');
axis([0, AREA_SIZE_X, 0, AREA_SIZE_Y]);