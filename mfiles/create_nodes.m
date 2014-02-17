%rand
nodes_x = rand(1, NODE_NUM) * AREA_SIZE_X;
nodes_y = rand(1, NODE_NUM) * AREA_SIZE_Y;
save('nodes_x.mat', 'nodes_x');
save('nodes_y.mat', 'nodes_y');
