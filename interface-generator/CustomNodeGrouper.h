#ifndef CUSTOM_NODE_GROUPER_H
#define CUSTOM_NODE_GROUPER_H

#include "typedefs.h"

struct Global; 

class CustomNodeGrouper
{
 public:

  void findTopNodesX0

    ( Global&   globdat,
      const int nn0 );

  void writeNodeGroup 
    
    ( const char* fileName = "ngroup.data" );

 private:

  IntVector inodes_;
};

#endif
