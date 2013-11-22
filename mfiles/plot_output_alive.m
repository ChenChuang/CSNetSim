load('output_track.mat');
load('alive_sum.mat');

dead = NODE_NUM - alive_sum;

%figure;
plot(output_track, dead, 'r');