load([dir,'output_track.mat']);
output_track = output_track(1:sampling:end);
%figure;
hold on;
plot(time, output_track, style);
%plot(output);
title('time - output');