////////////////////////////////////////////////////////////////////////
// Class:       Tracks2VTK
// Plugin Type: producer (art v2_04_00)
// File:        Tracks2VTK_module.cc
//
// Generated at Sun Oct  9 11:37:31 2016 by me using cetskelgen
// from cetlib version v1_20_00.
////////////////////////////////////////////////////////////////////////

// Create a multiblock dataset for each tag

// Art includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"

// LARSoft includes
#include "lardataobj/RecoBase/SpacePoint.h"

// artvtk includes
#include "artvtk/artvtk/VtkVizData.hh"

// larvtk includes
#include "larvtk/vtkData/TracksVtk.hh"

namespace larvtk {
  class Tracks2VTK;
}


// Shortcut for a vector of strings
using Strings = std::vector<std::string>;


class larvtk::Tracks2VTK : public art::EDProducer {
public:
  explicit Tracks2VTK(fhicl::ParameterSet const & p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  Tracks2VTK(Tracks2VTK const &) = delete;
  Tracks2VTK(Tracks2VTK &&) = delete;
  Tracks2VTK & operator = (Tracks2VTK const &) = delete;
  Tracks2VTK & operator = (Tracks2VTK &&) = delete;

  // Required functions.
  void produce(art::Event & e) override;

private:

  std::string myName_;
  Strings inputTags_;
};


/// Constructor for Tracks2VTK
///
/// \param [in] p Parameter set for module
larvtk::Tracks2VTK::Tracks2VTK(fhicl::ParameterSet const & p) :
  myName_("SHOULD NEVER SEE THIS"),
  inputTags_( p.get<Strings>("inputTags"))
{
  produces< artvtk::VtkVizData >();

  // Set the name
  std::ostringstream nameStream;
  nameStream << p.get<std::string>("module_type") << " (labelled " << p.get<std::string>("module_label") << ")";
  myName_ = nameStream.str();
}

/// Process the event
/// \param e The event information
void larvtk::Tracks2VTK::produce(art::Event & e)
{

  // We're going to make a multiblockdataset for each tag so we can separate them easily

  auto mb = vtkSmartPointer<vtkMultiBlockDataSet>::New();
  mb->SetNumberOfBlocks( inputTags_.size() );
  unsigned int blockNum = 0;

  // Loop over the input tags
  for ( const auto& tag : inputTags_ ) {

    // Make the vtkdata for these hits
    larvtk::TracksVtk vtkData;

    // Get the data corresponding to this tag
    auto tracks = * (e.getValidHandle< std::vector<recob::Track> >(tag) );


    for ( auto const& t : tracks ) {
      vtkData.addTrack(tag, t);
    }

    // Add this grid to the multiblockdataset
    mb->SetBlock( blockNum, vtkData.grid() );
    mb->GetMetaData( blockNum )->Set(vtkCompositeDataSet::NAME(), tag.c_str());
    blockNum++;
  }

  // Put it into the event
  auto vtkVizData = std::make_unique< artvtk::VtkVizData >(mb, myName_);
  e.put(std::move( vtkVizData ) );

}

DEFINE_ART_MODULE(larvtk::Tracks2VTK)
