/**
 * @file   LaserTrack.h
 * @brief  Provides a base class for laser analysis purpose
 * @author Christoph Rudolf von Rohr (crohr@fnal.gov)
 * @date   August 19th, 2015
 */

/// C/C++ standard library
#include <iostream>
#include <vector>
#include <string>

/// Root library
#include <TH3.h>
#include <TVector3.h>
#include <TF1.h>

// TODO: Fix class compilation if including GeometryCore.h and others!!!

/// LArSoft
#ifndef __GCCXML__
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/GeometryCore.h"
#include "larcore/Geometry/BoxBoundedGeo.h"
#endif

#include "larcore/SimpleTypesAndConstants/geo_types.h"

// Framework includes




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
      unsigned int fLaserID;            ///< Laser System identifier (1 = upstream, 2 = downstream)
      unsigned int fLaserEventID;       ///< Laser event id (not daq)
      unsigned int fAssosiateEventID;   ///< ID of the assosiate event id ()
      float fAperturePosition;          ///< Aperture position
      float fPower;                     ///< Attenuator setting (not measured pulse energy)
      
      #ifndef __GCCXML__
      void SetIntersectionPoints();
      #endif
      
      
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
      LaserBeam(const TVector3& LaserPosition, const TVector3& LaserDirection);
      
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
      LaserBeam(const TVector3& LaserPosition, const float& Phi, const float& Theta); 
      
     
     #ifndef __GCCXML__
     /**
     * @brief Sets laser Position
     * @param LaserPosition start position of the laser 
     */
      void SetPosition(const TVector3& LaserPosition, const bool& ReCalcFlag = true);
      
     /**
     * @brief Sets laser direction
     * @param LaserDirection direction of the laser beam
     */
      void SetDirection(const TVector3& LaserDirection, const bool& ReCalcFlag = true);
      
      /**
     * @brief Sets laser direction
     * @param LaserAngles reads the angles of the laser beam and calculates the direction
     */
      void SetDirection(const float& Phi, const float& Theta, const bool& ReCalcFlag  = true);
     
    /**
     * @brief Sets laser trigger time
     * @param trigger time sec (epoch time)
     * @param trigger time fraction in usec
     */
      void SetTime(const float& sec, const float& usec);
      
      /**
       * @brief Return laser trigger time as Time struct
       */
      inline Time GetTime() const {return fTime;}
      
     /**
     * @brief Sets the attenuator value of the laser beam
     * @param Set value of the attenuator position in %
     */    
      void SetPower(const float& AttenuatorPercentage);
      
      /*
       * @brief Get power setting of laser (aperture)
       */
      inline float GetPower() const { return fPower; }

      inline void SetLaserID(float id) {fLaserID = (unsigned int) id;}
      
      inline unsigned int GetLaserID() const {return fLaserID;}
      
      inline void SetLaserEventID(float id) {fLaserEventID = (unsigned int) id;}
      
      inline int GetLaserEventID() const {return fLaserEventID;}      
      
      inline void SetAssID(unsigned int id) {fAssosiateEventID = id;}
      
      inline unsigned int GetAssID() const { return fAssosiateEventID; }
      
      void SetErrors();//Error values
      
      /*
       * @brief Print all protected value to stdout
       */
      void Print() const;
      
      TVector3 GetLaserPosition() const;
      TVector3 GetLaserDirection() const;
      
      TVector3 GetEntryPoint() const;
      TVector3 GetExitPoint() const;
      
      #endif
      //Anydatatype GetErrors();
      
  };
}

#endif