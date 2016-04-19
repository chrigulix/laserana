/**
 * @file   LaserTrack.h
 * @brief  Provides a base class for laser analysis purpose
 * @author Christoph Rudolf von Rohr (crohr@fnal.gov)
 * @date   August 19th, 2015
 */

/// C/C++ standard library
#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <utility>
#include <string>
#include <iomanip>

/// Root library
#include <TH3.h>
#include <TVector3.h>
#include <TF1.h>

#ifndef LASERBEAM_H
#define LASERBEAM_H

namespace lasercal
{
  /**
   * @brief Provides a base class for laser analysis purpose
   * 
   * 
   */
  class LaserBeam
  {
    protected:
      // Laser start position (last mirror before the TPC)
      TVector3 fLaserPosition;
      // Direction of the Laser beam
      TVector3 fDirection;
      // First Point in TPC
      TVector3 fEntryPoint;
      // Last Point in TPC
      TVector3 fExitPoint;
      
      // Errors
      TVector3 fLaserPositionError;
      TVector3 fDirectionError;
      TVector3 fEntryPointError;
      TVector3 fExitPointError;
      
      float fTime;
      float fAperturePosition;

      // Power profile funtion (power along the beam)
      TF1 fPowerProfile;
      // Power of the beam (measured power of the laser e.g. by photodiod)
      float fPower;

//       void SetEntryPoint();
//       void SetExitPoint();
      
      
    public:
      
     /**
     * @brief Default constructor: sets an empty volume
     * @see SetPosition
     * @see SetDirection
     * 
     * We allow for a default construction since the derived object might need
     * some specific action before being aware of the laser properties.
     * In that case, SetPosition() and SetDirection() will set the properties.
     */
      LaserBeam();
      
     /**
     * @brief Constructor: sets laser position and laser direction
     * @param LaserPosition start position of the laser 
     * @param LaserDirection direction of the laser beam
     * @see SetPosition
     * @see SetDirection
     
     * This constructor loads the start position and direction of the laser beam
     */
      LaserBeam(TVector3& LaserPosition, TVector3& LaserDirection);
      
     /**
     * @brief Constructor: sets laser position and laser direction
     * @param LaserAngles start position of the laser 
     * @param LaserDirection direction of the laser beam
     * @see SetPosition
     * @see SetDirection
     
     * This constructor loads the start position and two angles given by the mirror angles
     * and calculates the direction vector
     */
      LaserBeam(TVector3& LaserPosition, TVector2& LaserAngles); 
      
     
     /**
     * @brief Sets laser Position
     * @param LaserPosition start position of the laser 
     */
      void SetPosition(TVector3& LaserPosition);
      
     /**
     * @brief Sets laser direction
     * @param LaserDirection direction of the laser beam
     */
      void SetDirection(TVector3& LaserDirection);
      
      /**
     * @brief Sets laser direction
     * @param LaserAngles reads the angles of the laser beam and calculates the direction
     */
      void SetDirection(std::array<float,2> LaserAngles);
      
      void SetErrors();//Error values
      
      TVector3 GetLaserPosition();
      TVector3 GetLaserDirection();
      
      TVector3 GetEntryPoint();
      TVector3 GetExitPoint();
      //Anydatatype GetErrors();
      
  };
}

#endif