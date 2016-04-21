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
    LaserDirection.Print();
    LaserPosition.Print();

}

LaserBeam::LaserBeam(TVector3& LaserPosition, float Phi, float Theta)
{
    TVector3 LaserDir;          // 3-Vector in laser coordinate system
    TVector3 LaserDirection;    // 3-Vector in uboone coordinate system 
    
    // Transform from measured angles to uboone coordinate system.
    LaserDir.SetMagThetaPhi(1., Theta, Phi);
    LaserDirection.SetX(LaserDir.Y());
    LaserDirection.SetY(LaserDir.Z());
    LaserDirection.SetZ(LaserDir.X());
    
    
    LaserBeam(LaserPosition, LaserDirection);
}

void LaserBeam::SetPower(float AttenuatorPercentage) {
    fPower = AttenuatorPercentage;
}

void LaserBeam::SetTime(unsigned int sec, unsigned int usec) {
    //fTime.date(boost::posix_time::seconds(sec) + boost::posix_time::microseconds(usec));
}

}