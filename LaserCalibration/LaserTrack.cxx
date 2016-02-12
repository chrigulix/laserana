#include "LaserTrack.h"

namespace lasercal
{
  LaserTrack::LaserTrack(){}
  
  LaserTrack::LaserTrack(TVector3& LaserPosition, TVector3& LaserDirection) /*: fLaserPosition(LaserPosition), fDirection(LaserDirection)*/
  {
    // Fill entry and exit point using geometry
  }
  
  void LaserTrack::SetPosition(TVector3& LaserPosition)
  {
    fLaserPosition = LaserPosition;
  }
  
  void LaserTrack::SetDirection(TVector3& LaserDirection)
  {
    fDirection = LaserDirection;
  }
}