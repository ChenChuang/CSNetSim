dir = 'EDLCR/';
style = 'ko';
plot_snapshot;

grid on;
xlabel('Node ID');
ylabel('Residual Energy [mJ]');
%title('EDLCR');
box on;

dir = 'EDCR/';
style = 'k+';
plot_snapshot;

grid on;
xlabel('Node ID');
ylabel('Residual Energy [mJ]');
%title('EDCR');
box on;

dir = 'HEED/';
style = 'k*';
plot_snapshot;

grid on;
xlabel('Node ID');
ylabel('Residual Energy [mJ]');
%title('HEED');
box on;