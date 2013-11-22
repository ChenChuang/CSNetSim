sampling = 400;
time = 0:RECORD_PERIOD:RECORD_PERIOD*(MAX_PERIOD_COUNT-1);
time = time(1:sampling:end);

dir = 'EDLCR/';
style = 'k-o';
plot_covered;

grid on;
xlabel('Time [minute]');
ylabel('Fcd');
%legend('average value','standard deviation value');
%title('EDLCR');

dir = 'EDCR/';
style = 'k-+';
plot_covered;

grid on;
xlabel('Time [minute]');
ylabel('Fcd');
%legend('average value','standard deviation value');
%title('EDCR');

dir = 'HEED/';
style = 'k-*';
plot_covered;

grid on;
xlabel('Time [minute]');
ylabel('Fcd');
%legend('average value','standard deviation value');
%title('HEED');
