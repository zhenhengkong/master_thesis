b = 8.;  //speciemn width 
l = 20.; //specimen length 

meshb = 0.5; // mesh size of width 
meshl = 4; // mesh size of length 

a = 2.;  //size of the rectangular hole 

Point(1) = {-l,-b,0,meshl};
Point(2) = { l,-b,0,meshl};
Point(3) = { l, b,0,meshl};
Point(4) = {-l, b,0,meshl};

Point(5) = {-a, a, 0, meshb};
Point(6) = { a, a, 0, meshb};
Point(7) = { a,-a, 0, meshb};
Point(8) = {-a,-a, 0, meshb};


Line (1) = {1,2};
Line (2) = {2,3};
Line (3) = {3,4};
Line (4) = {4,1};

Line(5) = {5,6};
Line(6) = {6,7};
Line(7) = {7,8};
Line(8) = {8,5};


Line Loop(1) = {1,2,3,4};
Line Loop(2) = {5,6,7,8};
Plane Surface(1) = {1,2};

Physical Surface(1) = {1};
