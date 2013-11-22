load([dir,'rotate_overhead_track']);
%figure;
hold on;
plot(time(3:end), rotate_overhead_track(3:end) - rotate_overhead_track(2));
%plot(time, rotate_overhead_track);
