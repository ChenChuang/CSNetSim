load([dir,'alive_sum.mat']);
alive_sum = alive_sum(1:sampling:end);
%figure;
hold on;
plot(time, alive_sum, style);
axis([0, RECORD_PERIOD*(MAX_PERIOD_COUNT-1), 0, NODE_NUM]);
%title('time - num of dead node');