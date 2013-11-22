figure;

sampling = 400;
time = 0:RECORD_PERIOD:RECORD_PERIOD*(MAX_PERIOD_COUNT-1);
time = time(1:sampling:end);

dir = 'EDLCR/';
style = 'k-o';
plot_output;

dir = 'EDCR/';
style = 'k-+';
plot_output;

dir = 'HEED/';
style = 'k-*';
plot_output;

grid on;
xlabel('simulation time/min');
ylabel('total network output/bytes');
legend('EDLCR','EDCR','HEED');
box on;