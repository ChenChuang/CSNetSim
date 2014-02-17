load('energy_sum.mat');
figure;
plot(time, energy_sum);
axis([0, length(energy_sum), 0, E_INIT*NODE_NUM]);
title('time - sum of energy');