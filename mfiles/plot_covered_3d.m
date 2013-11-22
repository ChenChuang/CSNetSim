load([dir,'energy_snapshot.mat']);
energy_snapshot = energy_snapshot(1:sampling:end,:);
load([dir,'node_snapshot.mat']);
node_snapshot = node_snapshot(1:sampling:end,:);
load([dir,'nodes_x']);
load([dir,'nodes_y']);

radius2 = CLUSTER_RADIUS^2;
covered_snapshot = zeros(size(node_snapshot));
for t = 1:length(time)
    ns = node_snapshot(t,:);
    t
    for i = 1:NODE_NUM
        if energy_snapshot(t, i) <= 0
            covered_snapshot(t, i) = -1;
        end
        for j = i+1:NODE_NUM
            if energy_snapshot(t, i) > 0 && energy_snapshot(t, j) > 0
                if node_snapshot(t, i) == i-1 && (nodes_x(i) - nodes_x(j))^2 + (nodes_y(i) - nodes_y(j))^2 < radius2
                    covered_snapshot(t, j) = covered_snapshot(t, j) + 1;
                    if node_snapshot(t, j) == j-1
                        covered_snapshot(t, i) = covered_snapshot(t, i) + 1;
                    end
                else if node_snapshot(t, j) == j-1 && (nodes_x(i) - nodes_x(j))^2 + (nodes_y(i) - nodes_y(j))^2 < radius2
                        covered_snapshot(t, i) = covered_snapshot(t, i) + 1;
                        if node_snapshot(t, i) == i-1
                            covered_snapshot(t, j) = covered_snapshot(t, j) + 1;
                        end
                    end
                end
            end
        end
    end
end

%hist = zeros(length(time), max(max(covered_snapshot))+1);
hist = zeros(length(time), 12);
for t = 1:length(time)
    for i = 1:NODE_NUM
        if covered_snapshot(t, i) >= 0
            if(covered_snapshot(t,i) < 12)
                hist(t, covered_snapshot(t,i)+1) = hist(t, covered_snapshot(t,i)+1) + 1;
            else
                hist(t, 12) = hist(t, 12) + 1;
            end
        end
    end
end

hist(hist>200)=0;

figure;
bar3(hist(2:end-12,:))
set(gca,'xticklabel',{'0','','','3','','','6','','','9','','','12'});
set(gca,'yticklabel',time(1:10:end));
ylim([0,38]);
xlim([0,13]);
zlim([0,200]);
view(148,10);
xlabel('F_{cd,l}');
ylabel('Simulation Time [minutes]');
zlabel('Number of Nodes');