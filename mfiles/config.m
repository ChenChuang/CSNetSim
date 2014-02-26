% This is the configuration script for homogeneous sensor network
% run this to load parameters into workspace

% energy model:
% E_transmitter = n * (E_ELEC + E_AMP * d^beta)  
% where, 
% E_AMP = E_AMP_FREESPACE, beta = 2 if d < D_THRESHOLD
% E_AMP = E_AMP_MULTIPATH, beta = 4 elsewhere
% E_receiver = n * (E_ELEC)

DISTRIBUTE_MASK = '';   % image file path of mask
CELL_D = 10;

AREA_SIZE_X = 100;    % metre
AREA_SIZE_Y = 100;
NODE_NUM = 500;
SINK_X = 50;  % metre
SINK_Y = 50;
SINK_ADDR = 0;

CLUSTER_RADIUS = 15;   % metre
MAX_RADIUS = 33;    % metre
SENSE_DATA_PERIOD = 1;   % second

DATA_PACKET_SIZE = 100;    % byte
CTRL_PACKET_SIZE = 25;   % byte
DATA_CTRL_PACKET_SIZE = 100;  % byte

E_INIT = 2000;  % mJ
E_ELEC = 400E-6;    % mJ/byte
E_FUSION = 40E-6;   % mJ/byte/source
E_AMP_FREESPACE = 80E-9;    % mJ/byte/m^2
E_AMP_MULTIPATH = 0.0104E-9;   % mJ/byte/m^4
D_THRESHOLD = 75;   % metre

MAX_SIM_TIME = 20000;    %second
RECORD_PERIOD = 1;
MAX_PERIOD_COUNT = floor(MAX_SIM_TIME / RECORD_PERIOD) + 1;

params = [...
    AREA_SIZE_X,...
    AREA_SIZE_Y,...
    NODE_NUM,...
    SINK_X,...
    SINK_Y,...
    SINK_ADDR...
    ...
    CLUSTER_RADIUS,...
    MAX_RADIUS,...
    SENSE_DATA_PERIOD,...
    ...
    DATA_PACKET_SIZE,...
    CTRL_PACKET_SIZE,...
    DATA_CTRL_PACKET_SIZE,...
    ...
    E_INIT,...
    E_ELEC,...
    E_FUSION,...
    E_AMP_FREESPACE,...
    E_AMP_MULTIPATH,...
    D_THRESHOLD,...
    ...
    MAX_SIM_TIME,...
    RECORD_PERIOD...
];

fprintf('sended param num = %d\n', length(params));

