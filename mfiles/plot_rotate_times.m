load([dir,'rotate_times_track']);
figure;
%hold on;
plot(time, [rotate_times_track(1), diff(rotate_times_track)]);