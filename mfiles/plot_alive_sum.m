load('time.mat');
load('alive_sum.mat');
figure;
hold on;
plot(time, NODE_NUM - alive_sum);
axis([0, length(alive_sum), 0, NODE_NUM]);
title('time - num of dead node');