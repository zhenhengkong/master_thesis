log =
{
  pattern = "*.info";
  file    = "-$(CASE_NAME).log";
};

control = 
{
  runWhile = "i < 10";
  fgMode = true;
};

userInput =
{
  modules = [ "input", "ngroups" ];

  input =
  {
    type = "GmshInput";
    file = "bend3.msh";
  };

  ngroups = 
  {
    type = "GroupInput";
    nodeGroups = [ "top", "leftbot", "rightbot" ];

    top.ytype = "max";
    leftbot.xtype = "min";
    leftbot.ytype = "min";
    rightbot.xtype = "max";
    rightbot.ytype = "min";
  };
};

model =
{
  type        = "Matrix";
  matrix.type = "Sparse"; 

  model       =
  {
    type   = "Multi";
    models = [ "solid", "diri", "lodi" ];

    solid =
    {
      type      = "Solid";
      elements  = "all";

      shape =
      {
        type      = "Triangle3";
        intScheme = "Gauss1"   ;
      };

      // Elasticity parameters

      material =
      {
        type    = "Orthotropic";
        dim     = 2;
        state   = "PLANE_STRAIN";
        young1  = 44730.;
        poisson12 = 0.28;
      };
    };

    diri =
    {
      type = "Dirichlet";

      initDisp = 0.;
      dispIncr = .001;
      loaded = 0;

      nodeGroups = [ "top", "leftbot", "leftbot" , "rightbot" ];
      dofs = [ "dy", "dy", "dx", "dy" ];
    };

    lodi =
    {
      type = "LoadDisp";
      group = "top";
    };
  };
};
 
userModules = 
{
  modules = [ "solver" 
              ,"view" 
              ,"graph"
             ];

  solver =
  {
    type = "Nonlin";
    lineSearch = false;
    precision = 1.0e-6;
    solver.type = "SkylineLU";
    solver.useThreads = true;
    maxIter = 10;
  };

  graph =
  {
    type = "Graph";
    
    dataSets = "loadDisp";
    loadDisp =
    {
      xData = "model.model.lodi.disp[1]";
      yData = "abs(model.model.lodi.load[1])";
    };
    graph.keyPos  = [ 0.900000, 0.200000 ];
  };


  view =
  {
    type = "FemView";

    updateWhen = true;
    snapFile = "$(CASE_NAME)%2i.png";

    // palettes.custom.ranges = [ -500., -300., -100., 100., 300., 500. ];

    // Define some data sets.

    dataSets = [ "disp", "stress" ];

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
      // Use the data set "disp" to deform the mesh.

      deformation = "disp";

      // Define extra plugins for showing more data sets.

      plugins = "colors";

      colors =
      {
        type = "MeshColorView";
        data = "disp[dx]";
        data = "stress[xx]";
        // palette   = "custom";
        // autoScale = true;
      };
    };

  };
};

sample =
{
    // The name of the output file:

    file = "lodi.dat";

    dataSets = [ "i"," model.model.lodi.disp[1]",
                 " abs(model.model.lodi.load[1])"
               ];
    // sampleWhen = "accepted";
};

xoutput =
{
  table = "stress";
};

