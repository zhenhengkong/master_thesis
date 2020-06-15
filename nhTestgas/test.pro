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
    file = "simple.mesh";
  };

  ngroups = 
  {
    type = "GroupInput";

    nodeGroups = [ "left", "right", "leftbottom", "bottom", "top" ];
    left.xtype = "min";
    right.xtype = "max";
    leftbottom.xtype = "min";
    leftbottom.ytype = "min";
    bottom.ytype = "min";
    top.ytype = "max";
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

        type    = "Gasser";
        dim     = 2;       
        mu      = 6000.;
        lambda  = 5000.;
        beta    = 0.349;
        k1_a    = 0.;
        k2_a    = 1.0;
        state  = "PLANE_STRAIN";   
        kappa = 0.1;
        
      

        kappam0 = 0.5; 
        kappamu = 0.9;

        kappaf0 = 0.7;
        kappafu = 0.9;


      };
      
      shape =
      {
        type       = "Quad4";
        intScheme  = "Gauss2*Gauss2";
      };      
    };

    diri =
    {
      type = "Dirichlet";

      dispIncr = .01;

      maxDisp = 2.;

      // nodeGroups = [ "right", "left", "left" ];
      // dofs = [ "dy", "dy", "dx" ];
      nodeGroups = [ "right", "left", "bottom" ];
      dofs = [ "dx", "dx", "dy" ];
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

