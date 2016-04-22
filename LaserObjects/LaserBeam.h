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
#include <ctime>

/// Root library
#include <TH3.h>
#include <TVector3.h>
#include <TF1.h>

#include "boost/date_time/posix_time/posix_time_types.hpp"

#ifndef LASERBEAM_H
#define LASERBEAM_H

namespace lasercal
{
  /**
   * @brief Provides a base class for laser analysis purpose
   * 
   * 
   */
    
    struct Time{
        unsigned long sec;
        unsigned long usec;
    };
  class LaserBeam
  {
    protected:
      ///< Laser start position (last mirror before the TPC)
      TVector3 fLaserPosition;        ///< Laser start position (last mirror before the TPC)
      TVector3 fDirection;           ///< Direction of the Laser beam
      TVector3 fEntryPoint;          ///< First Point in TPC
      TVector3 fExitPoint;           ///< Last Point in TPC
      
      /// Errors
      TVector3 fLaserPositionError;
      TVector3 fDirectionError;
      TVector3 fEntryPointError;
      TVector3 fExitPointError;
      
      Time fTime;                       ///< Trigger time recorded by laser server
      unsigned int fLaserID;            ///< Laser event id (not daq)
      unsigned int fAssosiateEventID;   ///< ID of the assosiate event id ()
      float fAperturePosition;          ///< Aperture position
      float fPower;                     ///< Attenuator setting (not measured pulse energy)

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
     * @param LaserPosition start position of the laser 
     * @param azimuthal angle (x-axis along straight shot trough TPC)
     * @param polar angle (z-axis pointing upward)
     * @see SetPosition
     * @see SetDirection
     
     * This constructor loads the start position and two angles given by the mirror angles
     * and calculates the direction vector
     */
      LaserBeam(TVector3& LaserPosition, float Phi, float Theta); 
      
     
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
      void SetDirection(float Phi, float Theta);
     
    /**
     * @brief Sets laser trigger time
     * @param trigger time sec (epoch time)
     * @param trigger time fraction in usec
     */
      void SetTime(float sec, float usec);
      
      /**
       * @brief Return laser trigger time as Time struct
       */
      inline Time GetTime() {return fTime;}
      
     /**
     * @brief Sets the attenuator value of the laser beam
     * @param Set value of the attenuator position in %
     */    
      void SetPower(float AttenuatorPercentage);
      
      /*
       * @brief Get power setting of laser (aperture)
       */
      inline float GetPower() { return fPower; }

      inline void SetLaserID(float id) {fLaserID = (unsigned int) id;}
      
      inline int GetLaserID() {return fLaserID;}      
      
      inline void SetAssID(float id) {fAssosiateEventID = (unsigned int) id;}
      
      inline unsigned int GetAssID(float id) { return fAssosiateEventID; }
      
      void SetErrors();//Error values
      
      TVector3 GetLaserPosition();
      TVector3 GetLaserDirection();
      
      TVector3 GetEntryPoint();
      TVector3 GetExitPoint();
      //Anydatatype GetErrors();
      
  };
}

#endif