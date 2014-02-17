load 'node_snapshot';
load 'energy_snapshot';
%ch_count = zeros(1, MAX_PERIOD_COUNT);
index = ones(MAX_PERIOD_COUNT, 1) * (0 : NODE_NUM - 1);
ch_count = sum((node_snapshot == index) .* (energy_snapshot > 0), 2);
figure;
plot(time, ch_count);
title('time - num of CH');