x1 = 0.;
x2 = 250.;
x3 = 290.;
x4 = 310.;
x5 = 350.;
x6 = 600.;
y1 = 0.;
y2 = 150.;
y3 = 152.;

fi = 2.;
co = 7.;

Point(1 ) = {x1, y1, 0., co};
Point(2 ) = {x2, y1, 0., co};
Point(3 ) = {x5, y1, 0., co};
Point(4 ) = {x6, y1, 0., co};
Point(5 ) = {x6, y2, 0., co};
Point(6 ) = {x5, y2, 0., co};
Point(7 ) = {x4, y2, 0., fi};
Point(8 ) = {x4, y3, 0., fi};
Point(9 ) = {x3, y3, 0., fi};
Point(10) = {x3, y2, 0., fi};
Point(11) = {x2, y2, 0., co};
Point(12) = {x1, y2, 0., co};

Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 5};
Line(5) = {5, 6};
Line(6) = {6, 7};
Line(7) = {7, 8};
Line(8) = {8, 9};
Line(9) = {9, 10};
Line(10) = {10, 11};
Line(11) = {11, 12};
Line(12) = {12, 1};

Line Loop(15) = {12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
Plane Surface(16) = {15};
Physical Surface(17) = {16};
