control = 
{
  runWhile = "i<=1";
  fgMode = true;
};

log =
{
  pattern = "*.info";
  file    = "-$(CASE_NAME).log";
};


userInput =
{
  modules = [ "input", "ngroups" ];

  input = 
  {
    type = "Input";
    file = "simple3.mesh";
  };

  ngroups = 
  {
    type = "GroupInput";

    nodeGroups = [ "left", "right", "leftbottom", "bottom", "top", "front", "back" ];
    left.xtype = "min";
    right.xtype = "max";
    leftbottom.xtype = "min";
    leftbottom.ytype = "min";
    bottom.ytype = "min";
    top.ytype = "max";
    front.ztype = "min";
    back.ztype = "max";
  };
};


model =
{
  type        = "Matrix";
  matrix.type = "Sparse";

  model       =
  {
    type   = "Multi";
    models = [ "bulk", 
           "diri", 
           // "diri2", 
           "lodi" ];

    bulk =
    {
      type     = "NLSolid";
      elements = "all";

      largeDisp = true;

      material =
      {
        type   = "Gasser";
        dim    = 3;
        
        mu     = 8000.;
        lambda = 5000.;

        beta    = 0.20;
        k1_a    = 562.;
        k2_a    = 0.7112;
        state  = "PLANE_STRAIN";   
        c_a   = 0.;
        c_10  = 0.;
        kappa = 0.1;    



      };
      
      shape =
      {
        type       = "Hex8";
        intScheme  = "Gauss2*Gauss2*Gauss2";
      };      
    };

    diri =
    {
      type = "Dirichlet";

      dispIncr = .01;

      maxDisp = 2.;

      // nodeGroups = [ "right", "left", "left" ];
      // dofs = [ "dy", "dy", "dx" ];
      nodeGroups = [ "right", "left", "bottom", "front", "back" ];
      dofs = [ "dx", "dx", "dy", "dz", "dz" ];
      loaded = 0;
    };

    diri2 = 
    {
      type = "Dirichlet";

      dispIncr = 0.;
      initDisp = 0.1;

      nodeGroups = [ "right" ];
      dofs = [ "dx" ];
      loaded = 0;
    };

    lodi =
    {
      type = "LoadDisp";
      group = "right";
    };    
  };
};
 
sample =
{
    file = "lodi.dat";

    sampleWhen = "accepted";

    dataSets = ["i", "model.model.lodi.disp[0]",
                "abs( model.model.lodi.load[0] )" ];
};

userModules = 
{
  modules = [ "solver", "graph", "view" ];

  solver = 
  {
    type = "Nonlin";

    precision = 1.e-6;
    solver.type = "SkylineLU";
    solver.useThreads = true;
    maxIter = 20;
  };

  graph =
  {
    type = "Graph";

    dataSets = "loadDisp";

    sampleWhen = "accepted";
    loadDisp =
    {
      key = "Load-displacement curve";

      xData = "model.model.lodi.disp[0]";
      yData = "model.model.lodi.load[0]";
    };
  };
  
  view =
  { 
    type = "FemView";
    
    updateWhen = true;
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
      // Use the data set "disp" to deform the mesh.
      
      deformation = "disp";
      
      // Define extra plugins for showing more data sets.
      
      plugins = "colors";
      
      colors =
      {
        type = "MeshColorView";
        data = "stress[xx]";
        palette   = "custom";
      };
    };
  
  };
};


xoutput =  
{
  write = true;
  table = "stress | eqstrain | eqveps | loading";
};
