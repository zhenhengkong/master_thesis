log =
{
  pattern = "*.info";
  file    = "-$(CASE_NAME).log";
};

control =
{
  runWhile = "i<1";
  fgMode = true;
};

userInput =
{
  modules = [ "input", "ngroup", "hardening" ];

  input =
  {
    type = "Input";
    file = "bar.mesh";
  };

  ngroup = 
  {
    type = "GroupInput";

    nodeGroups = [ "right", "left", "lbot" ];
    right.xtype = "max";
    left.xtype = "min";
    lbot.xtype = "min";
    lbot.ytype = "min";
  };
};

model =
{
  type   = "Matrix";

  model =
  {
    type   = "Multi";
    models = [ "elas", "melro", "tension", "lodi" ];

    elas = 
    {
      type = "NLSolid";
      elements = "elastic";
      
      material =
      {
//        type = "Hooke";
//        dim = 2;
//        state = "PLANE_STRAIN";
//        young = 3.13e3;
//        poisson = 0.37;


        type    = "Gasser";
        dim     = 2;       
        mu      = 6000.;
        lambda  = 5000.;
        beta    = 0.349;
        k1_a    = 1000.;
        k2_a    = 0.2;
        state  = "PLANE_STRAIN";   
        kappa = 0.1;
        

        kappam0 = 1.0; 
        kappamu = 1.5;

        kappaf0 = 1.0;
        kappafu = 1.5;

      };

      thickness = 1.;

      shape =
      {
        type = "Quad4";
        intScheme = "Gauss2*Gauss2";
      };
    };
      
    melro =
    {
      type = "NLSolid";
      elements = "damage";
      
      material =
      {
//        type = "Melro";
//        dim = 2;
//        state = "PLANE_STRAIN";
//        young = 3.13e3;
//        poisson = 0.37;
//        poissonP = 0.32;
//        rmTolerance = 1.e-10;

//        sigmaT = "(-14.8590*exp(x/-0.0012)-34.1296*exp(x/-0.0057)+71.3997)";
//        sigmaC = "(-24.1071*exp(x/-0.0097)-10.4262*exp(x/-0.0016)+89.2022)";


        type    = "Gasser";
        dim     = 2;       
        mu      = 6000.;
        lambda  = 5000.;
        beta    = 0.349;
        k1_a    = 1000.;
        k2_a    = 0.2;
        state  = "PLANE_STRAIN";   
        kappa = 0.1;
        

        kappam0 = 0.3; 
        kappamu = 1.5;

        kappaf0 = 0.3;
        kappafu = 1.5;

      };

      thickness = 1.;

      shape =
      {
        type = "Quad4";
        intScheme = "Gauss2*Gauss2";
      };
    };

    tension =
    {
      type = "Dirichlet";
      nodeGroups = [ "right", "lbot", "left" ];
      dofs = [ "dx", "dy", "dx" ];
      loaded = 0;
      initDisp = 0.;
      dispIncr = 0.02;
    };

    lodi = 
    {
      type = "LoadDisp";
      group = "right";
    };
  };
};

userModules =
{
  modules = [ "solver", "graph", "view" ];

  solver =
  {
    type = "Nonlin";

    solver.type = "SkylineLU";
    maxIter = 50;
  };

  view =
  {
    type = "FemView";

    updateWhen = true;
    snapWhen = false;
    snapFile = "$(CASE_NAME)%2i.png";
    configFile = "$(CASE_NAME).view";

    dataSets = [ "state", "history" ];

    state =
    {
      type   = "Vector";
      vector = "nodes/state";
    };

    history =
    {
      type   = "Table";
      table  = "nodes/nodalHistory";
    };

    mesh =
    {
      elements = "all";
      deformation = "state";

      plugins = "colors";

      colors =
      {
        type = "MeshColorView";
        data = "state[dx]";
        palette = "rainbow";
      };
    };
  };

  graph =
  {
    type = "Graph";

    dataSets = "lodi";
    lodi =
    {
      xData = "model.model.lodi.disp[0]";
      yData = "model.model.lodi.load[0]";
    };
    updateWhen = "accepted";
  };

};


sample = 
{
  type = "Sample";

  file = "lodi.dat";
  dataSets = ["i", "model.model.lodi.disp[0]",
              "model.model.lodi.load[0]"];
  sampleWhen = "accepted";
};

xoutput = 
{
  table = "stress";
};

