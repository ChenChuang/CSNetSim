sampling = 400;
time = 0:RECORD_PERIOD:RECORD_PERIOD*(MAX_PERIOD_COUNT-1);
time = time(1:sampling:end);

dir = 'EDLCR/';
plot_covered_3d;

dir = 'EDCR/';
plot_covered_3d;

dir = 'HEED/';
plot_covered_3d;
