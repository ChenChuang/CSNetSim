load('rotate_overhead_track');
figure;
plot(time(3:end), rotate_overhead_track(3:end) - rotate_overhead_track(2));
%plot(time, rotate_overhead_track);

load('rotate_times_track');
figure;
plot(time, [rotate_times_track(1), diff(rotate_times_track)]);