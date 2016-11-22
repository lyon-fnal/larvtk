////////////////////////////////////////////////////////////////////////
// Class:       PFParticleSpacePoints2VTK
// Plugin Type: producer (art v2_04_00)
// File:        PFParticleSpacePoints2VTK_module.cc
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
#include "canvas/Persistency/Common/Assns.h"

// LARSoft includes
#include "lardataobj/RecoBase/PFParticle.h"
#include "lardataobj/RecoBase/SpacePoint.h"

// artvtk includes
#include "artvtk/artvtk/VtkVizData.hh"

// larvtk includes
#include "larvtk/vtkData/PFParticleSpacePointsVtk.hh"

namespace larvtk {
  class PFParticleSpacePoints2VTK;
}


// Shortcut for a vector of strings
using Strings = std::vector<std::string>;
using theAssn = art::Assns<recob::PFParticle, recob::SpacePoint >;

class larvtk::PFParticleSpacePoints2VTK : public art::EDProducer {
public:
  explicit PFParticleSpacePoints2VTK(fhicl::ParameterSet const & p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  PFParticleSpacePoints2VTK(PFParticleSpacePoints2VTK const &) = delete;
  PFParticleSpacePoints2VTK(PFParticleSpacePoints2VTK &&) = delete;
  PFParticleSpacePoints2VTK & operator = (PFParticleSpacePoints2VTK const &) = delete;
  PFParticleSpacePoints2VTK & operator = (PFParticleSpacePoints2VTK &&) = delete;

  // Required functions.
  void produce(art::Event & e) override;

private:

  std::string myName_;
  Strings inputTags_;

  unsigned int fillMultiblockDataSetForTagWithGrid( vtkSmartPointer< vtkMultiBlockDataSet > mbForTag,
                                                    const PFParticleSpacePointsVtk& vtkData,
                                                    unsigned int blockNumForTag);
};


/// Constructor for PFParticleSpacePoints2VTK
///
/// \param [in] p Parameter set for module
larvtk::PFParticleSpacePoints2VTK::PFParticleSpacePoints2VTK(fhicl::ParameterSet const & p) :
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
void larvtk::PFParticleSpacePoints2VTK::produce(art::Event & e)
{

  // We're going to make a multiblockdataset for each tag so we can separate them easily

  auto mb = vtkSmartPointer<vtkMultiBlockDataSet>::New();
  mb->SetNumberOfBlocks( inputTags_.size() );
  unsigned int blockNum = 0;

  // Loop over the input tags
  for ( const auto& tag : inputTags_ ) {

    // Get the data corresponding to this tag -- we want to take advantage of the ordering
    // of data as added to the Assns
    auto assns = *(e.getValidHandle< theAssn >(tag));

    // We need to keep track of when the PFParticle Self ID changes;
    size_t pfpSelfWatcher = 0;
    bool first = true;

    // We're going to make a multiblock dataset for this tag
    auto mbForTag = vtkSmartPointer< vtkMultiBlockDataSet >::New();
    unsigned int blockNumForTag = 0;

    // We need a dynamic PFParticleSpacePointsVtk
    std::unique_ptr< PFParticleSpacePointsVtk > vtkData;

    // There is no Assn data, which is good because using the range for makes that inaccessible
    for ( auto const& thePair : assns ) {

      // thePair should be a theAssn::assn_t which is actually std::pair< PFParticle, SpacePoint >
      auto pfpPtr = thePair.first;
      auto spsPtr = thePair.second;

      // Have we hit a new PFParticle?
      if ( first || pfpPtr->Self() != pfpSelfWatcher ) {

        // Make the vtkPolyData for this PFParticle
        if ( ! first ) {
          blockNumForTag = fillMultiblockDataSetForTagWithGrid(mbForTag, *vtkData, blockNumForTag);
        }

        // We can reset the watcher
        first = false;
        pfpSelfWatcher = pfpPtr->Self();

        // Create the PFParticleSpacePointsVtk object
        vtkData = std::make_unique< PFParticleSpacePointsVtk >( *(pfpPtr), tag);
      }

      // Add this SpacePoint
      vtkData->addSpacePoint( *(spsPtr));
    }

    // If there's vtkData, then write it to the MultiBlockDataSet to save the last one
    if ( vtkData ) {
      blockNumForTag = fillMultiblockDataSetForTagWithGrid(mbForTag, *vtkData, blockNumForTag);
    }

    // Add this multiblock dataset to the main one
    mb->SetBlock( blockNum, mbForTag );
    mb->GetMetaData( blockNum )->Set(vtkCompositeDataSet::NAME(), tag.c_str());
    blockNum++;
  }

  // Put it into the event
  auto vtkVizData = std::make_unique< artvtk::VtkVizData >(mb, myName_);
  e.put(std::move( vtkVizData ) );

}

unsigned int larvtk::PFParticleSpacePoints2VTK::fillMultiblockDataSetForTagWithGrid(
    vtkSmartPointer<vtkMultiBlockDataSet> mbForTag, const PFParticleSpacePointsVtk &vtkData,
    unsigned int blockNumForTag) {

  // Set the grid in the multiblock dataset
  mbForTag->SetBlock(blockNumForTag, vtkData.grid());
  mbForTag->GetMetaData(blockNumForTag)->Set(vtkCompositeDataSet::NAME(), vtkData.name().c_str());

  blockNumForTag++;
  return blockNumForTag;
}

DEFINE_ART_MODULE(larvtk::PFParticleSpacePoints2VTK)
