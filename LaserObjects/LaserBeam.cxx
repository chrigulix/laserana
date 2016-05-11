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

LaserBeam::LaserBeam(TVector3& LaserPosition, float Phi, float Theta)
{
    TVector3 LaserDir;          // 3-Vector in laser coordinate system
    TVector3 LaserDirection;    // 3-Vector in uboone coordinate system 
    
    // Transform from measured angles to uboone coordinate system.
    LaserDir.SetMagThetaPhi(1., Theta, Phi);
    LaserDirection.SetX(LaserDir.Y());
    LaserDirection.SetY(LaserDir.Z());
    LaserDirection.SetZ(LaserDir.X());
    
    SetPosition(LaserPosition);
    SetDirection(LaserDirection);
    
    SetEntryPoint();
    
}

void LaserBeam::SetPosition(TVector3& LaserPosition){
    fLaserPosition = LaserPosition;
}

void LaserBeam::SetDirection(TVector3& LaserDirection){
    fDirection = LaserDirection;
}
void LaserBeam::SetPower(float AttenuatorPercentage) {
    fPower = AttenuatorPercentage;
}

void LaserBeam::SetTime(float sec, float usec) {
    fTime.sec = (unsigned long) sec;
    fTime.usec = (unsigned long) usec;
}

void LaserBeam::SetEntryPoint(){
    // TODO: Fix this, seems to be a problem with Qwwises GetIntersections
    //geo::GeometryCore const* Geometry = &*(art::ServiceHandle<geo::Geometry>());
    //const TVector3 Laser1(0,0,0);
    //const TVector3 Laser2(0,0,0);

    //auto aa = Geometry->TPC().GetIntersections(Laser1, Laser2);
    //std::cout << aa << std::endl;
    //aa.front().Print();
    //aa.back().Print();
    
}

void LaserBeam::Print() const {
    std::cout   << "Laser System        " << fLaserID << "\n"
                << "Laser Event ID      " << fLaserEventID << "\n"
                << "Associate Event ID  " << fAssosiateEventID << "\n"
                << "Attenuator Position " << fPower * 100 << " %\n"
                << "Aperture Position   " << fAperturePosition << "\n"
                << "Mirror Position     ";// << std::endl; 
    fLaserPosition.Print();
    std::cout    << "Mirror Direction    ";// << std::endl;
    fDirection.Print();
    std::cout << "\n" << std::endl;
}               
}