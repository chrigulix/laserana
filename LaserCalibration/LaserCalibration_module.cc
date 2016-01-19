// LaserCalibration_module.cc
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

#ifndef LaserCalibration_Module
#define LaserCalibration_Module

// Always include headers defining everything you use, and only that.
// Starting from LArSoft and ending with standard C++ is a good check
// on LArSoft headers too -- if they can't be loaded by their own, it's a bug!

// LArSoft includes
#include "Simulation/SimChannel.h"
#include "Simulation/LArG4Parameters.h"
#include "RecoBase/Hit.h"
#include "RecoBase/Cluster.h"
#include "Geometry/Geometry.h"
#include "Geometry/GeometryCore.h"
#include "SimulationBase/MCParticle.h"
#include "SimulationBase/MCTruth.h"
#include "SimpleTypesAndConstants/geo_types.h"

// Framework includes
#include "art/Utilities/Exception.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/FindManyP.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/ParameterSet.h"

// ROOT includes. Note: To look up the properties of the ROOT classes,
// use the ROOT web site; e.g.,
// <http://root.cern.ch/root/html532/ClassIndex.html>
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TVector3.h"

// C++ Includes
#include <map>
#include <vector>
#include <string>
#include <cmath>


namespace {
  
  // This is a anonymous namespace (the one below, instead, has a name:
  // "LaserCalibration").
  // The stuff you declare in an anonymous namespace will not be visible
  // beyond this file (more technically, this "translation unit", that is
  // everything that gets included in the .cc file from now on).
  // In this way, you don't pollute the environment of other modules.
  
  // We will define this function at the end, but we at least declare it here
  // so that the module can freely use it.
  /// Utility function to get the diagonal of the detector
  float DetectorDiagonal(geo::GeometryCore const& geom);
  
} // local namespace


namespace LaserCalibration {

  //-----------------------------------------------------------------------
  //-----------------------------------------------------------------------
  // class definition

  // This is in-source documentation in Doxygen format.
  // A web page is automatically updated with documentation from the latest
  // LArSoft; that page is linked from LArSoft wiki page "Using LArSoft":
  // https://cdcvs.fnal.gov/redmine/projects/larsoft/wiki/Using_LArSoft#LArSoft-Doxygen
  // 
  // In general, comments that start with "///" or "/** */" blocks will land
  // into Doxygen documentation; code with "//!" will, too, but ROOT also
  // interprets that type of comment in some specific situations, so it's
  // better to stick to the other one to avoid confusion (and bugs).
  // 
  // Again in general, a comment starting with "///" will be sticking to the
  // next code line, while one starting with "///<" will be sticking to the
  // same code line, or the last one.
  /**
   * @brief Example analyser
   * 
   * This class extracts information from the generated and reconstructed
   * particles.
   * It produces histograms for the simulated particles in the input file:
   * - PDG ID (flavour) of all particles
   * - momentum of the primary particles selected to have a specific PDG ID
   * - length of the selected particle trajectory
   * 
   * If also produces two ROOT trees.
   * The first ROOT tree contains information on the simulated particles,
   * including in "dEdx" a binned histogram of collected charge as function
   * of track range.
   * 
   * The second ROOT tree contains information on the reconstructed hits.
   * 
   * Configuration parameters
   * =========================
   * 
   * - *SimulationLabel* (string, default: "largeant"): label of the input data
   *   product with the detector simulation information (typically an instance
   *   of LArG4 module)
   * - *HitLabel* (string, mandatory): label of the input data product with
   *   reconstructed hits
   * - *ClusterLabel* (string, mandatory): label of the input data product with
   *   reconstructed clusters
   * - *PDGcode* (integer, mandatory): particle type (PDG ID) to be selected;
   *   only primary particles of this type will be considered
   * - *BinSize* (real, mandatory): dx [cm] used for the dE/dx calculation
   * 
   */
  class LaserCalibration : public art::EDAnalyzer
  {
  public:
 
    // Standard constructor for an ART module; we don't need a special
    // destructor here.
    /// Default constructor
    explicit LaserCalibration(fhicl::ParameterSet const& parameterSet);

    // the following methods have a standard meaning and a standard signature
    // inherited from the framework (art::EDAnalyzer class).
    // The "virtual" keyword is a reminder that the function we are dealing with
    // is, in fact, virtual. You don't need to understand it now, but it's very
    // important when you write a new algorithm.
    // The "override" keyword, new in C++ 2011, is an important safety measure
    // that ensures that the method we are going to write will override a
    // matching method in the base class. For example, if we mistyped it as
    // virtual void beginJob() const;
    // (adding "const"), the compiler will be very happy with it, but art will
    // not touch it, because art needs a "void beginJob()" (non-const) and
    // it will find one in the base class (void art::EDAnalyzer::beginJob())
    // and will happily and silently use that one instead. Using:
    // virtual void beginJob() const override;
    // instead, the compiler will immediately complain with us that this method
    // is overriding nothing, hinting to some mistake (the spurious "const"
    // in this case).
    
    // This method is called once, at the start of the job. In this
    //  example, it will define the histograms and n-tuples we'll write.
    // Define the histograms and n-tuples
    virtual void beginJob() override;

    // This method is called once, at the start of each run. It's a
    // good place to read databases or files that may have
    // run-dependent information.
    virtual void beginRun(const art::Run& run) override;

    // This method reads in any parameters from the .fcl files. This
    // method is called 'reconfigure' because it might be called in the
    // middle of a job; e.g., if the user changes parameter values in an
    // interactive event display.
    virtual void reconfigure(fhicl::ParameterSet const& parameterSet) override;

    // The analysis routine, called once per event. 
    virtual void analyze (const art::Event& event) override;

  private:

    // The stuff below is the part you'll most likely have to change to
    // go from this custom example to your own task.

    // The parameters we'll read from the .fcl file.
    std::string fSimulationProducerLabel; ///< The name of the producer that tracked simulated particles through the detector
    std::string fHitProducerLabel;        ///< The name of the producer that created hits
    std::string fClusterProducerLabel;    ///< The name of the producer that created clusters
    int fSelectedPDG;                     ///< PDG code of particle we'll focus on
    double fBinSize;                      ///< For dE/dx work: the value of dx. 

    // Pointers to the histograms we'll create. 
    TH1D* fPDGCodeHist;     ///< PDG code of all particles
    TH1D* fMomentumHist;    ///< momentum [GeV] of all selected particles
    TH1D* fTrackLengthHist; ///< true length [cm] of all selected particles

    // The n-tuples we'll create.
    TTree* fSimulationNtuple;     ///< tuple with simulated data
    TTree* fReconstructionNtuple; ///< tuple with reconstructed data

    // The variables that will go into the n-tuple.
    int fEvent;     ///< number of the event being processed
    int fRun;       ///< number of the run being processed
    int fSubRun;    ///< number of the sub-run being processed
    
    /// @name Variables used in the simulation tree
    /// @{
    int fSimPDG;       ///< PDG ID of the particle begin processed
    int fSimTrackID;   ///< GEANT ID of the particle begin processed
    
    // Arrays for 4-vectors: (x,y,z,t) and (Px,Py,Pz,E).
    // Note: old-style C++ arrays are considered obsolete. However,
    // to create simple n-tuples, we still need to use them. 
    double fStartXYZT[4]; ///< (x,y,z,t) of the true start of the particle
    double fEndXYZT[4];   ///< (x,y,z,t) of the true end of the particle
    double fStartPE[4];   ///< (Px,Py,Pz,E) at the true start of the particle
    double fEndPE[4];     ///< (Px,Py,Pz,E) at the true end of the particle
    
    /// Number of dE/dx bins in a given track.
    int fSimNdEdxBins;
    
    /// The vector that will be used to accumulate dE/dx values as function of range.
    std::vector<double> fSimdEdxBins;
    /// @}
    
    /// @name Variables used in the simulation tree
    /// @{
    int fRecoPDG;       ///< PDG ID of the particle begin processed
    int fRecoTrackID;   ///< GEANT ID of the particle begin processed
    
    /// Number of dE/dx bins in a given track.
    int fRecoNdEdxBins;
    
    /// The vector that will be used to accumulate dE/dx values as function of range.
    std::vector<double> fRecodEdxBins;
    
    /// @}

    // Other variables that will be shared between different methods.
    geo::GeometryCore const* fGeometry;       ///< pointer to Geometry provider
    double                   fElectronsToGeV; ///< conversion factor
    
  }; // class LaserCalibration


  //-----------------------------------------------------------------------
  //-----------------------------------------------------------------------
  // class implementation

  //-----------------------------------------------------------------------
  // Constructor
  LaserCalibration::LaserCalibration(fhicl::ParameterSet const& parameterSet)
    : EDAnalyzer(parameterSet)
  {
    // get a pointer to the geometry service provider
    fGeometry = &*(art::ServiceHandle<geo::Geometry>());
    
    // Read in the parameters from the .fcl file.
    reconfigure(parameterSet);
  }

  
  //-----------------------------------------------------------------------
  void LaserCalibration::beginJob()
  {
    // Get the detector length, to determine the maximum bin edge of one
    // of the histograms.
    const double detectorLength = DetectorDiagonal(*fGeometry);

    // Access ART's TFileService, which will handle creating and writing
    // histograms and n-tuples for us. 
    art::ServiceHandle<art::TFileService> tfs;
  
    // The arguments to 'make<whatever>' are the same as those passed
    // to the 'whatever' constructor, provided 'whatever' is a ROOT
    // class that TFileService recognizes. 

    // Define the histograms. Putting semi-colons around the title
    // causes it to be displayed as the x-axis label if the histogram
    // is drawn (the format is "title;label on abscissae;label on ordinates").
    fPDGCodeHist     = tfs->make<TH1D>("pdgcodes",";PDG Code;",                  5000, -2500, 2500);
    fMomentumHist    = tfs->make<TH1D>("mom",     ";particle Momentum (GeV);",    100, 0.,    10.);
    fTrackLengthHist = tfs->make<TH1D>("length",  ";particle track length (cm);", 200, 0, detectorLength);

    // Define our n-tuples, which are limited forms of ROOT
    // TTrees. Start with the TTree itself.
    fSimulationNtuple     = tfs->make<TTree>("LaserCalibrationSimulation",    "LaserCalibrationSimulation");
    fReconstructionNtuple = tfs->make<TTree>("LaserCalibrationReconstruction","LaserCalibrationReconstruction");

    // Define the branches (columns) of our simulation n-tuple. When
    // we write a variable, we give the address of the variable to
    // TTree::Branch.
    fSimulationNtuple->Branch("Event",       &fEvent,          "Event/I");
    fSimulationNtuple->Branch("SubRun",      &fSubRun,         "SubRun/I");
    fSimulationNtuple->Branch("Run",         &fRun,            "Run/I");
    fSimulationNtuple->Branch("TrackID",     &fSimTrackID,     "TrackID/I");
    fSimulationNtuple->Branch("PDG",         &fSimPDG,         "PDG/I");
    // When we write arrays, we give the address of the array to
    // TTree::Branch; in C++ this is simply the array name.
    fSimulationNtuple->Branch("StartXYZT",   fStartXYZT,       "StartXYZT[4]/D");
    fSimulationNtuple->Branch("EndXYZT",     fEndXYZT,         "EndXYZT[4]/D");
    fSimulationNtuple->Branch("StartPE",     fStartPE,         "StartPE[4]/D");
    fSimulationNtuple->Branch("EndPE",       fEndPE,           "EndPE[4]/D");
    // For a variable-length array: include the number of bins.
    fSimulationNtuple->Branch("NdEdx",       &fSimNdEdxBins,   "NdEdx/I");
    // ROOT can understand fairly well vectors of numbers (and little more)
    fSimulationNtuple->Branch("dEdx",        &fSimdEdxBins);

    // A similar definition for the reconstruction n-tuple. Note that we
    // use some of the same variables in both n-tuples.
    fReconstructionNtuple->Branch("Event",   &fEvent,          "Event/I");
    fReconstructionNtuple->Branch("SubRun",  &fSubRun,         "SubRun/I");
    fReconstructionNtuple->Branch("Run",     &fRun,            "Run/I");
    fReconstructionNtuple->Branch("TrackID", &fRecoTrackID,    "TrackID/I");
    fReconstructionNtuple->Branch("PDG",     &fRecoPDG,        "PDG/I");
    fReconstructionNtuple->Branch("NdEdx",   &fRecoNdEdxBins,  "NdEdx/I");
    fReconstructionNtuple->Branch("dEdx",    &fRecodEdxBins);
  }
   
  //-----------------------------------------------------------------------
  void LaserCalibration::beginRun(const art::Run& /*run*/)
  {
    // art expects this function to have a art::Run argument;
    // C++ expects us to use all the arguments we are given,
    // or it will warn that we could have forgotten to use it
    // (otherwise, why would be ever passing it around?).
    // 
    // But we don't actually need nor use it.
    // 
    // The trick to convince C++ that we know what we are doing is to omit
    // (in this case, commenting out) the name of the parameter,
    // still leaving it around. The argument will be still passed around,
    // but we don't have any mean to use it, and the compiler will be satisfied
    // with that.
    
    // How to convert from number of electrons to GeV.  The ultimate
    // source of this conversion factor is
    // ${LARCORE_INC}/SimpleTypesAndConstants/PhysicalConstants.h.
    // But sim::LArG4Parameters might in principle ask a database for it.
    art::ServiceHandle<sim::LArG4Parameters> larParameters;
    fElectronsToGeV = 1./larParameters->GeVToElectrons();
  }

  //-----------------------------------------------------------------------
  void LaserCalibration::reconfigure(fhicl::ParameterSet const& parameterSet)
  {
    // Read parameters from the .fcl file. The names in the arguments
    // to p.get<TYPE> must match names in the .fcl file.
    fSimulationProducerLabel = parameterSet.get< std::string >("SimulationLabel");
    fHitProducerLabel        = parameterSet.get< std::string >("HitLabel");
    fClusterProducerLabel    = parameterSet.get< std::string >("ClusterLabel");
    fSelectedPDG             = parameterSet.get< int         >("PDGcode");
    fBinSize                 = parameterSet.get< double      >("BinSize");
  }

  //-----------------------------------------------------------------------
  void LaserCalibration::analyze(const art::Event& event) 
  {
    // Start by fetching some basic event information for our n-tuple.
    fEvent  = event.id().event(); 
    fRun    = event.run();
    fSubRun = event.subRun();

    // This is the standard method of reading multiple objects
    // associated with the same event; see
    // <https://cdcvs.fnal.gov/redmine/projects/larsoftsvn/wiki/Using_the_Framework>
    // for more information. Define a "handle" to point to a vector of
    // the objects.
    // Then tell the event to fill the vector with all the objects of
    // that type produced by a particular producer.
    // Here we use a art::ValidHandle because there is no way we can get away
    // with the MCParticles not being there for us: if they are not available
    // in the input file, art will throw a ProductNotFound exception and we
    // will immediately know there is a problem.
    art::ValidHandle< std::vector<simb::MCParticle> > particleHandle
      = event.getValidHandle<std::vector<simb::MCParticle>>
      (fSimulationProducerLabel);

    // Get all the simulated channels for the event. These channels
    // include the energy deposited for each track.
    // A recob::Wire is all the charge collected (or induced) on a TDC channel,
    // as function of time. Tis is what we can observe from the detector.
    // A single SimChannel is similar, but being from simulation only, it also
    // keeps track of the separate charge contribution of each particle
    // (in Geant jargon, which "track").
    art::ValidHandle< std::vector<sim::SimChannel> > simChannelHandle
      = event.getValidHandle<std::vector<sim::SimChannel>>
      (fSimulationProducerLabel);

    //
    // Let's do simulation part first
    //
    
    // The MCParticle objects are not necessarily in any particular
    // order. Since we may have to search the list of particles, let's
    // put them into a sorted map that will make searching fast and
    // easy. To save both space and time, the map will not contain a
    // copy of the MCParticle, but a pointer to it.
    std::map< int, const simb::MCParticle* > particleMap;

    // This is a "range-based for loop" in the latest version of C++ (2011);
    // do a web search on "c++ range based for loop" for more
    // information. Here's how it breaks down:

    // - A range-based for loop operates on a
    // container. particleHandle is not a container; it's a pointer to
    // a container. If we want C++ to "see" a container, we have to
    // dereference the pointer, like this: *particleHandle.

    // - The loop variable that is set to each object in the container
    // is named "particle". As for the loop variable's type:

    //   - To save a little bit of typing and to make the code easier
    //   to maintain, and paying in readability, we're going to let
    //   the C++ compiler deduce the type of what's in the container
    //   (simb::MCParticle objects in this case), so we use "auto".

    //   - We do _not_ want to change the contents of the container,
    //   so we use the "const" keyword to make sure.

    //   - We don't need to copy each object from the container into
    //   the variable "particle". It's sufficient to refer to the
    //   object by its address. So we use the reference operator "&"
    //   to tell the compiler to just copy the address, not the entire
    //   object.

    // It sounds complicated, but the end result is that we loop over
    // the list of particles in the art::Event in the most efficient
    // way possible.

    for ( auto const& particle : (*particleHandle) )
      {
	// For the methods you can call to get particle information,
	// see ${NUTOOLS_INC}/SimulationBase/MCParticle.h.
	fSimTrackID = particle.TrackId();

	// Add the address of the MCParticle to the map, with the track ID as the key.
	particleMap[fSimTrackID] = &particle; 

	// Histogram the PDG code of every particle in the event.
	fSimPDG = particle.PdgCode();
	fPDGCodeHist->Fill( fSimPDG );

	// For this example, we want to fill the n-tuples and histograms
	// only with information from the primary particles in the
	// event, whose PDG codes match a value supplied in the .fcl file.
	if ( particle.Process() == "primary"  &&  fSimPDG == fSelectedPDG )
	  {
	    // A particle has a trajectory, consisting of a set of
	    // 4-positions and 4-mommenta.
	    const size_t numberTrajectoryPoints = particle.NumberTrajectoryPoints();

	    // For trajectories, as for vectors and arrays, the
	    // first point is #0, not #1.
	    const int last = numberTrajectoryPoints - 1;
	    const TLorentzVector& positionStart = particle.Position(0);
	    const TLorentzVector& positionEnd   = particle.Position(last);
	    const TLorentzVector& momentumStart = particle.Momentum(0);
	    const TLorentzVector& momentumEnd   = particle.Momentum(last);

	    // Make a histogram of the starting momentum.
	    fMomentumHist->Fill( momentumStart.P() );

	    // Fill arrays with the 4-values. (Don't be fooled by
	    // the name of the method; it just puts the numbers from
	    // the 4-vector into the array.)
	    positionStart.GetXYZT( fStartXYZT );
	    positionEnd.GetXYZT( fEndXYZT );
	    momentumStart.GetXYZT( fStartPE );
	    momentumEnd.GetXYZT( fEndPE );

	    // Use a polar-coordinate view of the 4-vectors to
	    // get the track length.
	    const double trackLength = ( positionEnd - positionStart ).Rho();
	    LOG_DEBUG("LaserCalibration")
	      << "Track length: " << trackLength << " cm";
	    
	    // Fill a histogram of the track length.
	    fTrackLengthHist->Fill( trackLength ); 

	    // Let's have some debug information on screen to see that everything
	    // is fine.
	    // LOG_DEBUG() is a messagefacility macro that prints stuff when the
	    // code is compiled in "debug" mode, but makes its code line "disappear"
	    // otherwise, so that there will be no overhead.
	    // A bit of debug information is always useful; too much is detrimental.
	    LOG_DEBUG("LaserCalibration")
	      << "track ID=" << fSimTrackID << " (PDG ID: " << fSimPDG << ") "
	      << trackLength << " cm long, momentum " << momentumStart.P()
	      << " GeV/c, has " << numberTrajectoryPoints << " trajectory points";
	    
	    // Determine the number of dE/dx bins for the n-tuple.
	    fSimNdEdxBins = int( trackLength / fBinSize ) + 1;
	    // Initialize the vector of dE/dx bins to empty.
	    fSimdEdxBins.clear();

	    // To look at the energy deposited by this particle's track,
	    // we loop over the SimChannel objects in the event.
	    for ( auto const& channel : (*simChannelHandle) )
	      {
		// Get the numeric ID associated with this channel.
		// (The channel number is a 32-bit unsigned int, which
		// normally requires a special data type. Let's use
		// "auto" so we don't have to remember that.
		// It's raw::ChannelID_t, anyway.)
		auto channelNumber = channel.Channel();

		// A little care: There is more than one plane that
		// reacts to charge in the TPC. We only want to
		// include the energy from the collection plane.
		// (geo::kCollection is defined in
		// ${LARCORE_INC}/SimpleTypesAndConstants/geo_types.h)
		if ( fGeometry->SignalType( channelNumber ) != geo::kCollection )
		  continue;
		
	    // Each channel has a map inside it that connects
	    // a time slice to energy deposits in the
	    // detector. This is an implementation of the concept of
	    // "sparse vector": very few of the time slices have charge
	    // information, for most of the time no particle passes nearby
	    // (this is MC, we don't have noise at this level).
	    // So instead of using a long vector full of zeroes, we use a map
	    // that for each time when there was actually charge, it stores
	    // both time and charge information.
	    // We'll use "auto", but it's worth
	    // noting that the full type of this map is
	    // std::map<unsigned short, std::vector<sim::IDE>>;
	    auto const& timeSlices = channel.TDCIDEMap();
	    
	    // For every time slice in this channel:
	    for ( auto const& timeSlice : timeSlices )
	      {
		// Each entry in a map is a pair<first,second>.
		// For the timeSlices map, the 'first' is a time
		// slice number, which we don't care about in this
		// example. The 'second' is a vector of IDE
		// objects.
		auto const& energyDeposits = timeSlice.second;
		
		// Loop over the energy deposits.
		// An energy deposit is something that knows how much charge/energy
		// was deposited in a small volume, by which particle, and where.
		// The type of 'energyDeposit' will be sim::IDE, which is
		// defined in ${LARSIM_INC}/Simulation/SimChannel.h.
		for ( auto const& energyDeposit : energyDeposits )
		  {
		    // Check if the track that deposited the
		    // energy matches the track of the particle.
		    if ( energyDeposit.trackID != fSimTrackID ) continue;
		// Get the (x,y,z) of the energy deposit.
		TVector3 location( energyDeposit.x,
				   energyDeposit.y,
				   energyDeposit.z );
		
		// Distance from the start of the track.
		const double distance = ( location - positionStart.Vect() ).Mag();
		
		// Into which bin of the dE/dx array do we add the energy?
		const unsigned int bin = (unsigned int)( distance / fBinSize );
		
		// Is the dE/dx array big enough to include this bin?
		if ( fSimdEdxBins.size() < bin+1 )
		  {
		    //  Increase the array size, padding it with zeros.
		    //  Accommodate the new one too (that has index: #bin).
		    fSimdEdxBins.resize( bin+1 , 0. );
		  }

		// Add the energy deposit to that bin. (If you look at the
		// definition of sim::IDE, you'll see that there's another
		// way to get the energy. Are the two methods equivalent?
		// Compare the results and see!)
		fSimdEdxBins[bin] += energyDeposit.numElectrons * fElectronsToGeV;
	  } // For each energy deposit
	        } // For each time slice
	      } // For each SimChannel

	    // At this point we've filled in the values of all the
	    // variables and arrays we want to write to the
	    // n-tuple for this particle.
	    // The following command actually writes those values.
	    fSimulationNtuple->Fill();

	  } // if primary and PDG selected by user
      } // loop over all particles in the event. 

    //
    // Reconstruction time
    //
    
    // All of the above is based on objects entirely produced by the
    // simulation. Let's try to do something based on reconstructed
    // objects. A Hit (see ${LARDATA_INC}/RecoBase/Hit.h) is a
    // 2D object in a plane.

    // This code duplicates much of the code in the previous big
    // simulation loop, and will produce the similar results. (It won't
    // be identical, because of shaping and other factors; not every bit
    // of charge in a channel ends up contributing to a hit.)  The point
    // is to show different methods of accessing information, not to
    // produce profound physics results -- that part is up to you!

    // For the rest of this method, I'm going to assume you've read the
    // comments in previous section; I won't repeat all the C++ coding
    // tricks and whatnot.

    // Read in the Hits. We don't use art::ValidHandle here
    // because we accept that there might be no hits in the input,
    // and we still want the simulated information.
    art::Handle< std::vector<recob::Hit> > hitHandle;
    event.getByLabel(fHitProducerLabel, hitHandle);

    // Did we read in any recob::Hit objects? If not, the hitHandle
    // will be invalid; we might as well skip the rest of this module.

    // I make this test because this module might be tested on the
    // output of a typical simulation job, instead of a reconstruction
    // job; without the reconstruction steps, there won't be any hits.

    // Note that if I don't make this test, and there aren't any
    // recob::Hit objects, then the first time we access hitHandle,
    // ART will display a "ProductNotFound" exception message and,
    // depending on art settings, it may skip all processing for
    // the rest of this event, including any subsequent analysis steps,
    // or stop the execution.

    if ( ! hitHandle.isValid() ) return;
  
    // Our goal is to accumulate the dE/dx of any particles associated
    // with the hits that match our criteria: primary particles with the
    // PDG code from the .fcl file. I don't know how many such particles
    // there will be. We'll use a map, with track ID as the key, to hold
    // the vectors of dE/dx information.
    std::map< int, std::vector<double> > dEdxMap;

    // For every Hit:
    for ( auto const& hit : (*hitHandle) )
      {
	// The channel associated with this hit.
	auto hitChannelNumber = hit.Channel();

	// Again, for this example let's just focus on the collection plane.
	if ( fGeometry->SignalType( hitChannelNumber ) != geo::kCollection )
	  continue;
    // In the simulation section, we started with particles to find
    // channels with a matching track ID. Now we search in reverse:
    // search the SimChannels for matching channel number, then look
    // at the tracks inside the channel.
    
    for ( auto const& channel : (*simChannelHandle) )
      {
	auto simChannelNumber = channel.Channel();
	
	if ( simChannelNumber != hitChannelNumber ) continue;
    // For every time slice in this channel:
    auto const& timeSlices = channel.TDCIDEMap();
    for ( auto const& timeSlice : timeSlices )
      {
	// A channel will contain all the energy
	// deposited on a wire, but there can be
	// more than one hit associated with a
	// wire. To prevent double-counting the
	// energy, make sure the time of these
	// energy deposits corresponds to the time
	// of the hit.
	int time = timeSlice.first; 
	if ( std::abs(hit.TimeDistanceAsRMS(time)) < 1.0 )
	  {
	    // Loop over the energy deposits.
	    auto const& energyDeposits = timeSlice.second;
	    for ( auto const& energyDeposit : energyDeposits )
	      {
		// Remember that map of MCParticles we created
		// near the top of this method? Now we can
		// use it. Search the map for the track ID
		// associated with this energy deposit.
		// Since a map is automatically sorted, we can
		// use a fast binary search method, 'find()'.
		
		// By the way, the type of "search" is an
		// iterator (to be specific, it's an
		// std::map<int,simb::MCParticle*>::const_iterator,
		// which makes you thankful for the "auto"
		// keyword). If you're going to work with
		// C++ containers, you'll have to learn
		// about iterators eventually; do a web
		// search on "STL iterator" to get started.
		auto search = particleMap.find( energyDeposit.trackID );
    
		// Did we find this track ID in the particle map? It's possible
		// for the answer to be "no"; some particles are too low in kinetic
		// energy to be written by the simulation (see 
		// ${LARSIM_DIR}/job/simulationservices.fcl, 
		// parameter ParticleKineticEnergyCut).
		if ( search == particleMap.end() ) continue;
	    // "search" points to a pair in the map: <track ID, MCParticle*>
	    int trackID = (*search).first;
	    const simb::MCParticle& particle = *((*search).second);
	    
	    // Is this a primary particle, with a PDG code that
	    // matches the user input?
	    if ( particle.Process() == "primary" 
		 && particle.PdgCode() == fSelectedPDG )
	      {
		// Determine the dE/dx of this particle.
		const TLorentzVector& positionStart = particle.Position(0);
		TVector3 location( energyDeposit.x,
				   energyDeposit.y,
				   energyDeposit.z );
		double distance = ( location - positionStart.Vect() ).Mag();
		unsigned int bin = int( distance / fBinSize );
		double energy = energyDeposit.numElectrons * fElectronsToGeV;
		
		// A feature of maps: if we refer to dEdxMap[trackID], and
		// there's no such entry in the map yet, it will be
		// automatically created with a zero-size vector. Test to see
		// if the vector for this track ID is big enough.
		// dEdxMap is a map; althought it's not the slowest container in the
		// known universe, it's not fast either. If we are going to access the
		// same element over and over, it is a good idea to find that element
		// once, and then refer to that item directly.
		// Since we don't really care of the specific type (a vector, by the way),
		// we can use "auto" to save some time. This must be a reference,
		// since we want to change the original value in the map, and
		// can't be constant (also note that the operator[] of maps do change
		// the map).
		auto& track_dEdX = dEdxMap[trackID];
		if ( track_dEdX.size() < bin+1 )
		  {
		    // Increase the array size, padding it 
		    // with zeroes.
		    track_dEdX.resize( bin+1, 0 ); 
		  }
		
		// Add the energy to the dE/dx bins for this track.
		track_dEdX[bin] += energy;
		
	      } // particle selected
	      } // loop over energy deposits
	  } // channel time matches hit time
      } // loop over time slices
      } // for each SimChannel
      } // for each Hit

    // We have a map of dE/dx vectors. Write each one of them to the
    // reconstruction n-tuple. 
    for ( const auto& dEdxEntry : dEdxMap )
      {
	// Here, the map entries are <first,second>=<track ID, dE/dx vector>
	fRecoTrackID = dEdxEntry.first;
	// This is an example of how we'd pick out the PDG code if
	// there are multiple particle types or tracks in a single
	// event allowed in the n-tuple.
	fRecoPDG = particleMap[fRecoTrackID]->PdgCode();

	// Get the number of bins for this track.
	const std::vector<double>& dEdx = dEdxEntry.second;
	fRecoNdEdxBins = dEdx.size();

	// Copy this track's dE/dx information.
	fRecodEdxBins = dEdx;

	// At this point, we've filled in all the reconstruction
	// n-tuple's variables. Write it.
	fReconstructionNtuple->Fill();
      }

    // Think about the two big loops above, One starts from the
    // particles then looks at the channels; the other starts with the
    // hits and backtracks to the particles. What links the
    // information in simb::MCParticle and sim::SimChannel is the
    // track ID number assigned by the LArG4 simulation; what links
    // sim::SimChannel and recob::Hit is the channel ID.

    // In general, that is not how objects in the LArSoft
    // reconstruction chain are linked together. Most of them are
    // linked using associations and the art::Assns class:
    // <https://cdcvs.fnal.gov/redmine/projects/larsoftsvn/wiki/Use_Associations>

    // The web page may a bit difficult to understand (at least, it is
    // for me), so let's try to simplify things:

    // - If you're doing an analysis task, you don't have to worry
    // about creating art::Assns objects. 

    // - You don't have to read the art:Assns objects on your
    // own. There are helper classes (FindXXX) which will do that for
    // you.

    // - There's only one helper you need: art::FindManyP. It will do
    // what you want with a minimum of fuss.

    // - Associations are symmetric. If you see an
    // art::Assns<ClassA,ClassB>, the helper classes will find all of
    // ClassA associated with ClassB or all of ClassB associated with
    // ClassA.

    // - To know what associations exist, you have to be a 'code
    // detective'. The important clue is to look for a 'produces' line
    // in the code that writes objects to an art::Event. For example,
    // in ${LARSIM_DIR}/source/LArG4/LArG4_module.cc, you'll see this
    // line:

    // produces< art::Assns<simb::MCTruth, simb::MCParticle> >();

    // That means a simulated event will have an association between
    // simb::MCTruth (the primary particle produced by the event
    // generator) and the simb::MCParticle objects (the secondary
    // particles produced in the detector simulation).

    // Let's try it. The following statement will find the
    // sim::MCTruth objects associated with the simb::MCParticle
    // objects in the event (referenced by particleHandle):

    const art::FindManyP<simb::MCTruth> findManyTruth(particleHandle, event, fSimulationProducerLabel);

    // Note that we still have to supply the module label of the step
    // that created the association. Also note that we did not have to
    // explicitly read in the simb::MCTruth objects from the
    // art::Event object 'event'; FindManyP did that for us.
    
    // Also note that at this point art::FindManyP has already found all the
    // simb::MCTruth associated with each of the particles in particleHandle.
    // This is a slow process, so in general you want to do it only once.
    // If we had a loop on the particles, we would still do this outside that loop.

    // Now we can query the 'findManyTruth' object to access the
    // information. First, check that there wasn't some kind of error:

    if ( findManyTruth.isValid() )
      {
	// I'm going to be lazy, and just look at the simb::MCTruth
	// object associated with the first simb::MCParticle we read
	// in. (The main reason I'm being lazy is that if I used the
	// single-particle generator in prodsingle.fcl, every
	// particle in the event is going to be associated with just
	// the one primary particle from the event generator.)

	size_t particle_index = 0; // look at first particle in
				   // particleHandle's vector.

	// I'm using "auto" to save on typing. The result of
	// FindManyP::at() is a vector of pointers, in this case
	// pointers to simb::MCTruth.  This will be a vector with just
	// one entry; I could have used art::FindOneP instead. (This
	// will be a vector of art::Ptr, which is a type of smart
	// pointer; see
	// <https://cdcvs.fnal.gov/redmine/projects/larsoftsvn/wiki/Using_the_Framework>.)
	// To avoid having the vector copied, I store a (constant) reference to it
	// (it is possible because art::FindManyP::at() returns a constant reference;
	// otherwise, problems would arise).

	auto const& truth = findManyTruth.at( particle_index );

	// Make sure there's no problem. 
	if ( ! truth.empty() )
	  {
	    // Use the message facility to write output. I don't have
	    // to write the event, run, or subrun numbers; the message
	    // facility takes care of that automatically.  I'm "going
	    // at warp speed" with the vectors, pointers, and methods;
	    // see ${NUTOOLS_INC}/SimulationBase/MCTruth.h and
	    // ${NUTOOLS_INC}/SimulationBase/MCParticle.h for the
	    // nested calls I'm using.
	    mf::LogInfo("LaserCalibration")  
	      << "Particle ID=" << particleHandle->at( particle_index ).TrackId()
	      << " primary PDG code=" << truth[0]->GetParticle(0).PdgCode();
	  }
	else
	  {
	    mf::LogError("LaserCalibration")  
	      << "Particle ID=" << particleHandle->at( particle_index ).TrackId()
	      << " has no primary!";
	  }
      } // FindMany valid
    else
      {
	mf::LogError("LaserCalibration")  
	  << "findManyTruth simb::MCTruth for simb::MCParticle failed!";
      }

    // Let's try a slightly more realistic example. Suppose I want to
    // read in the clusters, and learn what hits are associated with
    // them. Then I could backtrack from those hits to determine the
    // dE/dx of the particles in the clusters. (Don't worry; I won't
    // put you through the n-tuple creation procedure for a third
    // time.)

    // First, read in the clusters.
    // Again, here we require clusters to be available by gettin a ValidHandle.
    // Otherwise, the following code would not work.
    art::ValidHandle< std::vector<recob::Cluster> > clusterHandle
      = event.getValidHandle<std::vector<recob::Cluster>>
        (fClusterProducerLabel);

    // Now use the associations to find which hits are associated with
    // which clusters. Note that this is not as trivial a query as the
    // one with MCTruth, since it's possible for a hit to be assigned
    // to more than one cluster.

    // We have to include fClusterProducerLabel, since that's the step
    // that created the art::Assns<recob::Hit,recob::Cluster> object;
    // look at the modules in ${LARRECO_DIR}/source/ClusterFinder/ and
    // search for the 'produces' lines. (I did not know this before I
    // wrote these comments. I had to be a code detective and use UNIX
    // tools like 'grep' and 'find' to locate those routines.)
    const art::FindManyP<recob::Hit> findManyHits(clusterHandle, event, fClusterProducerLabel);

    if ( findManyHits.isValid() )
      {
	// Now we'll loop over the clusters to see the hits associated
	// with each one. Note that we're not using a range-based for
	// loop. That's because FindManyP::at() expects a numeric index
	// as an argument, so we might as well go through the cluster
	// objects numerically instead.
	for ( size_t cluster_index = 0; cluster_index != clusterHandle->size(); ++cluster_index )
	  {
	    // In this case, FindManyP::at() will return a vector of
	    // pointers to recob::Hit.
	    // FindManyP::at() returns the vector of hits corresponding to the
	    // cluster number cluster_index in the list that it was constructed with,
	    // that is in this case clusterHandle itself.
	    auto const& hits = findManyHits.at( cluster_index );

	    // We have a vector of pointers to the hits associated
	    // with the cluster, but for this example I'm not going to
	    // do anything fancy with them. I'll just print out how
	    // many there are.
	    mf::LogInfo("LaserCalibration")  
	      << "Cluster ID=" << clusterHandle->at( cluster_index ).ID()
	      << " has " << hits.size() << " hits";
	  }
      } // findManyHits valid
    else
      {
	mf::LogError("LaserCalibration")  
	  << "findManyHits recob::Hit for recob::Cluster failed;"
	  << " cluster label='" << fClusterProducerLabel << "'";
      }

  } // LaserCalibration::analyze()
  
  
  // This macro has to be defined for this module to be invoked from a
  // .fcl file; see LaserCalibration.fcl for more information.
  DEFINE_ART_MODULE(LaserCalibration)

} // namespace LaserCalibration


namespace {
  // time to define that function...
  float DetectorDiagonal(geo::GeometryCore const& geom) {
    const double length = geom.DetLength();
    const double width = 2. * geom.DetHalfWidth();
    const double height = 2. * geom.DetHalfHeight();
    
    return std::sqrt(length*length + width*width + height*height);
  } // DetectorDiagonal()
} // local namespace


#endif // LaserCalibration_Module
