load('output_track.mat');
load('energy_sum.mat');

plot(output_track, energy_sum);
axis([0, max(output_track), 0, E_INIT*NODE_NUM]);
title('output - sum of energy');