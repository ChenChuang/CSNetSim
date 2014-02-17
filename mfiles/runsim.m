% This is the main script

clc;
clear all;

% run config.m
config;

create_nodes;

% run csim
crunsim(params, nodes_x, nodes_y);

% run analysis.m
analysis;