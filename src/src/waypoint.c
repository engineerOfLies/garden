#include <stdlib.h>
#include <stdio.h>
#include "map.h"
#include "path.h"

int main(int argc, char *argv[])
{
  //Init_Graphics();
  LoadMap("maps/testmap3.tcm");
  MapWayPoints();
  SaveMap("maps/testmap3.tcm");

        
  return 0;
}



/*the necessary blank line at the end of the file:*/

