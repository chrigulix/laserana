// TimeMapProducer_module.cc
// A basic "skeleton" to read in art::Event records from a file,
// access their information, and do something with them. 

// See
// <https://cdcvs.fnal.gov/redmine/projects/larsoftsvn/wiki/Using_the_Framework>
// for a description of the ART classes used here.

// Almost everything you see in the code below may have to be changed
// by you to suit your task. The example task is to make histograms
// and n-tuples related to dE/dx of particle tracks in the detector.

// As you try to understand why things are done a certain way in this
// example ("What's all this stuff about 'auto const&'?"), it will help
// to read ADDITIONAL_NOTES.txt in the same directory as this file.

// Also note that, despite our efforts, the documentation and the practices in
// this code may fall out of date. In doubt, ask!
// The last revision of this code was done on July 2015 with LArSoft v04_17_00.

#ifndef TimeMapProducer_Module
#define TimeMapProducer_Module

// Always include headers defining everything you use, and only that.
// Starting from LArSoft and ending with standard C++ is a good check
// on LArSoft headers too -- if they can't be loaded by their own, it's a bug!

// LArSoft includes
#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/raw.h"
#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/GeometryCore.h"

#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"

// Framework includes
#include "canvas/Utilities/Exception.h"
#include "art/Framework/Core/EDProducer.h"

#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/ParameterSet.h"

// uBooNE includes
#include "lardata/Utilities/AssociationUtil.h"

// ROOT includes. Note: To look up the properties of the ROOT classes,
// use the ROOT web site; e.g.,
// <http://root.cern.ch/root/html532/ClassIndex.html>
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TFile.h"

// C++ Includes
#include <map>
#include <vector>
#include <array>
#include <string>
#include <cmath>
#include <typeinfo>
#include <utility>
#include <memory>
#include <iterator>

#include <fstream>
#include <boost/tokenizer.hpp>

// Laser Module Classes
#include "LaserObjects/LaserBeam.h"


namespace TimeMapProducer
{

class TimeMapProducer : public art::EDProducer
{
public:

    explicit TimeMapProducer(fhicl::ParameterSet const& parameterSet);

    virtual void beginJob() override;

    virtual void beginRun(art::Run& run);

    virtual void endJob() override;

    virtual void reconfigure(fhicl::ParameterSet const& parameterSet) override;

    // The analysis routine, called once per event. 
    virtual void produce(art::Event& event) override;
   

private:

    bool DEBUG = false;

    // All this goes into the root tree
    TTree* fTimeAnalysis;
    unsigned int fEvent;
    unsigned int time_s;
    unsigned int time_ms;
    
    unsigned int LCS_ID = -1;
    unsigned int RunNumber = 0;

}; // class TimeMapProducer


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
// class implementation

//-----------------------------------------------------------------------
// Constructor

TimeMapProducer::TimeMapProducer(fhicl::ParameterSet const& pset)
{

    // Read in the parameters from the .fcl file.
    this->reconfigure(pset);
}


//-----------------------------------------------------------------------

void TimeMapProducer::beginJob()
{

}

void TimeMapProducer::beginRun(art::Run& run)
{
    
    art::ServiceHandle<art::TFileService> tfs;
    
    std::cout << "GENERATING TIME OUTPUT" << std::endl;
    // Initialize time info root file
    fTimeAnalysis = tfs->make<TTree>("TimeAnalysis", "TimeAnalysis");
    fTimeAnalysis->Branch("event", &fEvent);
    fTimeAnalysis->Branch("time_s", &time_s);
    fTimeAnalysis->Branch("time_ms", &time_ms);
    return;
}

void TimeMapProducer::endJob()
{
}

void TimeMapProducer::reconfigure(fhicl::ParameterSet const& parameterSet)
{

}

//-----------------------------------------------------------------------

void TimeMapProducer::produce(art::Event& event)
{
    fEvent = (unsigned int) event.id().event();
    
    time_s = (unsigned int) event.time().timeHigh();
    time_ms = (unsigned int) event.time().timeLow();
    if (DEBUG)
    {
        std::cout << "Event ID: " << fEvent << std::endl;
        std::cout << "Event Time (low): " << time_s << std::endl;
        std::cout << "Event Time (hig): " << time_ms << std::endl;
    }
    fTimeAnalysis->Fill();
    }


DEFINE_ART_MODULE(TimeMapProducer)

} // namespace TimeMapProducer

#endif // TimeMapProducer_Module
