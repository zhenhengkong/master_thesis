b = 8.;  //speciemn width 
l = 20.; //specimen length 

c = 2.;  //length of the imperfection
d = 4.;  //depth of the imperfetion  

meshb = 0.5; // mesh size of width 
meshl = 1.0; // mesh size of length 

Point(1) = {-l,-b,0,meshl};
Point(2) = { l,-b,0,meshl};
Point(3) = { l, b,0,meshl};
Point(4) = { 12, b,0,meshb};
Point(5) = { 12, b-d,0,meshb};
Point(6) = { 8, b-d,0,meshb};
Point(7) = { 8, b,0,meshb};
Point(8) = {-l, b,0,meshl};


Line (1) = {1,2};
Line (2) = {2,3};
Line (3) = {3,4};
Line (4) = {4,5};
Line (5) = {5,6};
Line (6) = {6,7};
Line (7) = {7,8};
Line (8) = {8,1};



Line Loop(1) = {1,2,3,4,5,6,7,8};
Plane Surface(1) = {1};

Physical Surface(1) = {1};
