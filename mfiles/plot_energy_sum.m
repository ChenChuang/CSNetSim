load([dir,'energy_sum.mat']);
energy_sum = energy_sum(1:sampling:end);
%figure;
hold on;
plot(time, energy_sum, style);
axis([0, RECORD_PERIOD*(MAX_PERIOD_COUNT-1), 0, E_INIT*NODE_NUM]);
title('time - sum of energy');