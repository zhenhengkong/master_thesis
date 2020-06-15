b = 8.;  //speciemn width 
l = 20.; //specimen length 

meshb = 0.5; // mesh size of width 
meshl = 4; // mesh size of length 

Point(1) = {-l,-b,0,meshl};
Point(2) = { l,-b,0,meshl};
Point(3) = { l, b,0,meshl};
Point(4) = {-l, b,0,meshl};

Line (1) = {1,2};
Line (2) = {2,3};
Line (3) = {3,4};
Line (4) = {4,1};



Line Loop(1) = {1,2,3,4};
Plane Surface(1) = {1};

Physical Surface(1) = {1};
