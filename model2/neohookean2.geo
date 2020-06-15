b = 8.;  //speciemn width 
l = 20.; //specimen length 

meshb = 1.0; // mesh size of width 
meshl = 1.5; // mesh size of length 

xc = 0;   // x coordiante of the plaque center
yc = -2;   // y coordiante of the plaque center 
rx = 6;   // vertical plaque radius
ry = 4;   // horizontal plaque radius


Point(1) = {-l,-b,0,meshl};
Point(2) = { l,-b,0,meshl};
Point(3) = { l, b,0,meshl};
Point(4) = {-l, b,0,meshl};

Point(5) = {-rx+xc, yc, 0, meshb};
Point(6) = { xc, ry+yc, 0, meshb};
Point(7) = { rx+xc, yc, 0, meshb};
Point(8) = { xc,-ry+yc, 0, meshb};
Point(9) = { xc, yc, 0, meshb};

Line (1) = {1,2};
Line (2) = {2,3};
Line (3) = {3,4};
Line (4) = {4,1};

Ellipse(5) = {5,9,5,6};
Ellipse(6) = {6,9,5,7};
Ellipse(7) = {7,9,5,8};
Ellipse(8) = {8,9,5,5};


Line Loop(1) = {1,2,3,4};
Line Loop(2) = {5,6,7,8};
Plane Surface(1) = {1,2};
Plane Surface(2) = {2};

Physical Surface(1) = {1};
Physical Surface(2) = {2};
