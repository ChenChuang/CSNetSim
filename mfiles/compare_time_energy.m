figure;

sampling = 400;
time = 0:RECORD_PERIOD:RECORD_PERIOD*(MAX_PERIOD_COUNT-1);
time = time(1:sampling:end);

dir = 'EDLCR/';
style = 'k-o';
plot_energy_sum;

dir = 'EDCR/';
style = 'k-+';
plot_energy_sum;

dir = 'HEED/';
style = 'k-*';
plot_energy_sum;

grid on;
xlabel('Time [minute]');
ylabel('Residual Energy of Network [mJ]');
legend('EDLCR','EDCR','HEED');
box on;