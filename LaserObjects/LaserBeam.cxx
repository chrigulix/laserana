#include "LaserBeam.h"

namespace lasercal
{

LaserBeam::LaserBeam()
{
}

LaserBeam::LaserBeam(TVector3& LaserPosition, TVector3& LaserDirection) : fLaserPosition(LaserPosition), fDirection(LaserDirection)
{
    //     SetEntryPoint();
    //     SetExitPoint();
}

LaserBeam::LaserBeam(TVector3& LaserPosition, TVector2& LaserAngles)
{

}
}