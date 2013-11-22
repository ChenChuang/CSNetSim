function circle( x, y, r )
%CIRCLE Summary of this function goes here
%   Detailed explanation goes here
a = 0 : pi/50 : 2*pi;
xx = r * cos(a) + x; 
yy = r * sin(a) + y; 
plot(xx, yy, 'r');
end

