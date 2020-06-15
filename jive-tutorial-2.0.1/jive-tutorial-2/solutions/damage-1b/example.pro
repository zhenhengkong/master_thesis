
// Parameters controlling the output messages.

log =
{
  // Print to the terminal and to a file.

  file = "-$(CASE_NAME).log";
};


// Parameters for the InputModule.

input =
{
  file = "$(CASE_NAME).dat";
};


// Parameters for the ControlModule.

control =
{
  pause    = 0.2;
  runWhile = "i < 40";
  fgMode   = true;
};


// Parameters controlling the NonlinModule.

nonlin =
{
  precision = 1.0e-4;
  solver    = "SparseLU"
  {};
};


// Parameters controlling the output.

output =
{
  file    = "$(CASE_NAME).out";

  // Save the state vector and label it "disp" in the
  // output file.

  vectors = [ "state = disp" ];

  // Save the average stresss and the damage in the elements.

  tables  = [ "elements/stress",
              "elements/damage" ];

  // Save the solution every 5 load steps.

  saveWhen = "(i % 5) < 0.1";
};


// Parameters controlling the GraphModule.

graph =
{
  dataSets = [ "load" ];

  load =
  {
    xData = "disp";
    yData = "load";
  };
};


// The definition of the model tree.

model = "Matrix"
{
  matrix =
  {
    type = "FEM";  // Use the FEMatrixBuilder.
  };

  model  = "Multi"
  {
    models = [ "elastic", "damage", "load" ];

    elastic = "Elastic"
    {
      // Use the elements in the group "elastic" (see the data file).

      elements = "elastic";
      young    = 200e9;
      area     = 0.0001;
    };

    damage = "Damage"
    {
      // Use the elements in the group "elastic" (see the data file).

      elements = "damage";
      young    = elastic.young;
      area     = elastic.area;
      alpha    = 0.001;
      beta     = 0.004;
    };

    // Define a point load at the right end of the bar.
    // The load increases each iteration.

    load = "LoadScale"
    {
      scaleFunc = "1 + i";
      model     = "Constraints"
      {
        // Use the table defined in the data file.

        conTable = "disp";
      };
    };
  };
};
