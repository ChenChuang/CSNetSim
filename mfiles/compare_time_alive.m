figure;

sampling = 400;
time = 0:RECORD_PERIOD:RECORD_PERIOD*(MAX_PERIOD_COUNT-1);
time = time(1:sampling:end);

dir = 'EDLCR/';
style = 'k-*';
plot_alive_sum;

dir = 'EDCR/';
style = 'k-square';
plot_alive_sum;

dir = 'HEED/';
style = 'k-o';
plot_alive_sum;

grid on;
xlabel('Time [minute]');
ylabel('Number of Alive Nodes');
legend('EDLCR','EDCR','HEED');
axis([0,20000,0,550]);
box on;
