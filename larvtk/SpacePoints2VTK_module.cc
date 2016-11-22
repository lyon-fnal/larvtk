////////////////////////////////////////////////////////////////////////
// Class:       SpacePoints2VTK
// Plugin Type: producer (art v2_04_00)
// File:        SpacePoints2VTK_module.cc
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
#include "larvtk/vtkData/SpacePointsVtk.hh"

namespace larvtk {
  class SpacePoints2VTK;
}


// Shortcut for a vector of strings
using Strings = std::vector<std::string>;


class larvtk::SpacePoints2VTK : public art::EDProducer {
public:
  explicit SpacePoints2VTK(fhicl::ParameterSet const & p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  SpacePoints2VTK(SpacePoints2VTK const &) = delete;
  SpacePoints2VTK(SpacePoints2VTK &&) = delete;
  SpacePoints2VTK & operator = (SpacePoints2VTK const &) = delete;
  SpacePoints2VTK & operator = (SpacePoints2VTK &&) = delete;

  // Required functions.
  void produce(art::Event & e) override;

private:

  std::string myName_;
  Strings inputTags_;
};


/// Constructor for SpacePoints2VTK
///
/// \param [in] p Parameter set for module
larvtk::SpacePoints2VTK::SpacePoints2VTK(fhicl::ParameterSet const & p) :
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
void larvtk::SpacePoints2VTK::produce(art::Event & e)
{

  // We're going to make a multiblockdataset for each tag so we can separate them easily

  auto mb = vtkSmartPointer<vtkMultiBlockDataSet>::New();
  mb->SetNumberOfBlocks( inputTags_.size() );
  unsigned int blockNum = 0;

  // Loop over the input tags
  for ( const auto& tag : inputTags_ ) {

    // Make the vtkdata for these hits
    larvtk::SpacePointsVtk vtkData;

    // Get the data corresponding to this tag
    auto spH = e.getValidHandle< std::vector<recob::SpacePoint> >(tag);


    for ( auto const& sp : (*spH) ) {
      vtkData.addSpacePoint(tag, sp);
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

DEFINE_ART_MODULE(larvtk::SpacePoints2VTK)
