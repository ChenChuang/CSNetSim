plot_nodes;
hold on;
load('cluster_radius_G.mat');
m = size(cluster_radius_G,1);
n = size(cluster_radius_G,2);
for i = 1:m
    for j = 1:n
        if cluster_radius_G(i,j) > 0
            plot([nodes_x(i), nodes_x(j)], [nodes_y(i), nodes_y(j)]);
        end
    end
end
