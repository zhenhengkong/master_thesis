log =
{
  pattern = "*.info";
  file    = "-$(CASE_NAME).log";
};

control = 
{
  // pause = 0.;
  fgMode = true;
  runWhile = "i < 250";
};

userInput =
{
  modules = [ "inputSolid", "inputInterf", "ngroups" ];

  inputSolid =     // reads mesh file for solid elements
  {
    type = "Input";
    file = "solid.mesh";
  };

  inputInterf =    // reads mesh file for interface elements
  {
    type = "Input";
    file = "interface.mesh";
  };


  ngroups =        // creates node groups (for boundary conditions)
  {
    type = "GroupInput";
    nodeGroups = [ "left", "right", "lb" ];

    left.xtype = "min";
    right.xtype = "max";
    lb.xtype = "min";
    lb.ytype = "min";
  };
};

model =
{
  type        = "Matrix";
  matrix.type = "Sparse"; 

  model       =
  {
    type   = "Multi";
    models = [ "bulk" , "inclusion" , "interface" , "diri" , "lodi" ];

    bulk =        // model for stiffness matrix from bulk solid phase 
    {
      type      = "Solid";
      elements  = "solid0";

      shape =
      {
        type      = "Triangle3";
        intScheme = "Gauss1"   ;
      };

      material =
      {
//        type    = "Damage";   // refers to which material class is used

        type    = "NeoHooke";
        dim     = 2;          // spatial dimension
        state   = "PLANE_STRAIN";


        mu     = 8000.;
        lambda = 5000.; 

       kappa0 = 0.1;
       kappau = 0.11; 


//        young   = 32000.;     // young's modulus
//        poisson = 0.3;        // poisson's ratio

//        softening = "linear"; // type of softening relation
//        equistrain = "Mazars";// type of state variable

//        kappaI = 0.01;        // damage initiation criterion (~strength)
//        gf = 10.;             // fracture energy
      };
    };

    inclusion =   // model for stiffness matrix from inclusion
    {
      type = "Solid";
      elements = "solid1";

      shape =
      {
        type      = "Triangle3";
        intScheme = "Gauss1"   ;
      };

      material = 
      {
        type = "Hooke";      // linear elastic material
        dim = 2;             // spatial dimension
        state = "PLANE_STRAIN";
        young = 1000.;      // young's modulus
        poisson = 0.3;       // poisson's ratio
      };
    };


    interface =   // model for stiffness matrix from interface
    {
      type = "Interface";
      elements  = "interface";

      shape =
      {
        type      = "BLine2";
        intScheme = "NewtonCotes2"   ;
      };

      // Cohesive zone properties

      coheMat =
      {
        type   = "Turon";   // bilinear mixed-mode cohesive alw
        dim    = 2;         // spatial dimension
        dummy  = 1.e6;      // initial (dummy) stiffness
 
        f2t = 20.;          // tensile strength
        f6  = 20.;          // shear strength
        gI  = 3.;           // mode I fracture energy
        gII = 3.;           // mode II fracture energy
        eta = 1.;           // mode interaction parameter
      };
    };

//    arclen =  // model for boundary conditions and adaptive stepping
//    {
//      type = "DispArclen";

//      swtIter = 4;          // number of iterations for switching to arcl
//      swtEnergy = 1.;       // dissipation for switching to arclength
      
//      dispIncr = 0.01;      // (initial) displacement increment size
//      minDispIncr = 0.00001;// minimum displacement increment

//      maxDisp = 2.;         // displacement value for termination
//      minIncr =  .0001;     // minimum time step size (dissipation control)
//      maxIncr = 10.;        // maximum time step size (dissipation control)

//      constraints =         // boundary conditions
//      {
//        nodeGroups = [ "left", "lb", "right" ];
//        dofs = [ "dx", "dy", "dx" ];
//        loaded = 2;
//      };
//    };

    diri =
    {
      type = "Dirichlet";

      dispIncr = 0.02;

      maxDisp = 4.;

      // nodeGroups = [ "right", "left", "left" ];
      // dofs = [ "dy", "dy", "dx" ];

        nodeGroups = [ "left", "lb", "right" ];
        dofs = [ "dx", "dy", "dx" ];
        loaded = 2;
    };


    lodi =  // model for recording load displacement data
    {
      type = "LoadDisp";
      group = "right";
    };
  };
};
 
userModules = 
{
  modules = [ "solver", "graph", "view"  ];

//  solver =
//  {
//    type = "FlexArclen";

//    nonLin = 
//    {
//      precision = 1.0e-6;
//      solver.type = "SkylineLU";
//      solver.useThreads = true;
//      maxIter = 10;
//    };

//    arcLen = nonLin;
//    arcLen . allowDespair = true;
//  };


  solver = 
  {
    type = "Nonlin";

    precision = 1.e-6;
    solver.type = "SkylineLU";
    solver.useThreads = true;
    maxIter = 30;
  };


  graph =  // for load-displacement visualization
  {
    type = "Graph";
    
    dataSets = "loadDisp";
    loadDisp =
    {
      xData = "model.model.lodi.disp[0]";
      yData = "abs(model.model.lodi.load[0])";
    };
    graph.keyPos  = [ 0.900000, 0.200000 ];
    sampleWhen = "accepted";
  };


  view = // for stress visualization in deformed mesh
  {
    type = "FemView";

    updateWhen = "accepted";
    snapFile = "$(CASE_NAME)%2i.png";
    configFile  = "$(CASE_NAME).view";

    // Define some data sets.

    dataSets = [ "disp" , "stress" ];

    disp =
    {
      type   = "Vector";
      vector = "state";
    };

    stress =
    {
      type  = "Table";
      table = "nodes/stress";
    };

    mesh =
    {
      deformation = "2 * disp";  // scaled deformations

      plugins = "colors";

      colors =
      {
        type = "MeshColorView";
        data = "stress[xx]";    // stress component that is shown
        palette   = "custom";
      };
    };

  };
};

sample =  // for writing load-displacement data
{
    file = "lodi.dat";

    dataSets = [ "i"," model.model.lodi.disp[0]",
                 " abs(model.model.lodi.load[0])"
               ];
    sampleWhen = "accepted";
};

xoutput =   // for writing stress and history
{
  table = "stress | kappa";
};
