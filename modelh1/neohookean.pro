log =
{
  pattern = "*.info";
  file    = "-$(CASE_NAME).log";
};

control = 
{
  runWhile = "i<=1";
  fgMode = true;   
};

userInput =
{
  modules = [ "input", "ngroups" ];
  
  input = 
  {
    type = "GmshInput";
    file = "neohookean.msh";
  };

  ngroups =
  {
    type = "GroupInput";
    nodeGroups = ["left","right" ];
    
    left.xtype = "min";
    right.xtype = "max";
  };
};

model =
{
  type        = "Matrix";
  matrix.type = "Sparse";

  model   =
  {
    type   = "Multi";
    models = [ "bulk", "diri","diri2", "lodi" ];
    
    bulk = 
    {
      type     = "NLSolid";
      elements = "all";

      largeDisp = true;
   
      material =
 
     {
        type    = "NeoHooke";   // refers to which material class is used
        dim     = 2;          // spatial dimension
        state   = "PLANE_STRAIN";

        mu     = 6000.;     // young's modulus
        lambda = 5000. ;    // poisson's ratio

       kappa0 = 0.15;
       kappau = 0.20; 

     };
     
     shape = 
     {
       type      = "Triangle3";
       intScheme = "Gauss1";
      };
    };
    
    diri = 
    {
      type = "Dirichlet";
      dispIncr = 0.01;
      maxDisp = 20.;
      nodeGroups = ["right"];
      dofs = ["dx"];
      loaded = 0; 
    };

    diri2=
    {
      type = "Dirichlet";
      dispIncr = 0.;
      initDisp = 0.;

      nodeGroups = [ "left" ];
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

    sampleWhen  = "accepted";

    dataSets = ["i", "model.model.lodi.disp[0]",
                "abs( model.model.lodi.load[0] )" ];
};

userModules =
{
  modules = ["solver", "graph", "view" ];

  solver = 
  {
    type = "AdaptiveStep";

    nonlin = 
    {
      precision = 1.e-6;
      solver.type = "SkylineLU";
      solver.useThreads = true;
      maxIter = 50;
    };
  };

  graph = 
  {
    type = "Graph";

    dataSets = "loadDisp";

    sampleWhen = "accepted";
    loadDisp=
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
































