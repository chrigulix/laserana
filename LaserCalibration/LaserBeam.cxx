#include "LaserBeam.h"

namespace lasercal
{
  Beam::Beam(){}
  
  Beam::Beam(TVector3& LaserPosition, TVector3& LaserDirection) : fLaserPosition(LaserPosition),  fDirection(LaserDirection)
  {
//     SetEntryPoint();
//     SetExitPoint();
  }
}