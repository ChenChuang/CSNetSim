figure;
sampling = 400;
time = 0:RECORD_PERIOD:RECORD_PERIOD*(MAX_PERIOD_COUNT-1);
time = time(1:sampling:end);

dir = 'EDLCR/';
style = 'k-o';
plot_ch_count;

dir = 'EDCR/';
style = 'k-+';
plot_ch_count;

dir = 'HEED/';
style = 'k-*';
plot_ch_count;

grid on;
xlabel('Time [minute]');
ylabel('Number of CHs');
legend('EDLCR','EDCR','HEED');
box on;