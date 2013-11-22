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

avg_covered = zeros(1,length(time));
var_covered = zeros(1,length(time));
for t = 1:length(time)
    s = 0;
    n = 0;
    for i = 1:NODE_NUM
        if covered_snapshot(t, i) >= 0
            s = s + covered_snapshot(t, i);
            n = n + 1;
        end
    end
    if(n > 0)
        avg_covered(t) = s / n;
    end
    s = 0;
    n = 0;
    for i = 1:NODE_NUM
        if covered_snapshot(t, i) >= 0
            s = s + (covered_snapshot(t, i) - avg_covered(t))^2;
            n = n + 1;
        end
    end
    if(n > 0)
        var_covered(t) = (s / n)^(1/2);
    end
end

% figure;
% hold on;
% plot(time, avg_covered, 'k-square');
% hold on;
% plot(time, var_covered, 'k-^');
% axis([0, RECORD_PERIOD*(MAX_PERIOD_COUNT-1), -1, 6]);

figure;
errorbar(time, avg_covered, var_covered, style);
axis([0, RECORD_PERIOD*(MAX_PERIOD_COUNT-1), -1, 8]);

