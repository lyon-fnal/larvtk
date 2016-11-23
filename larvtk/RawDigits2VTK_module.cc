////////////////////////////////////////////////////////////////////////
// Class:       RawDigits2VTK
// Plugin Type: producer (art v2_04_00)
// File:        RawDigits2VTK_module.cc
//
// Generated at Sun Oct  9 11:37:31 2016 by me using cetskelgen
// from cetlib version v1_20_00.
////////////////////////////////////////////////////////////////////////

// Create a multiblock dataset for each tag

#include <string>
#include <vector>

// Art includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "cetlib/exception.h"

// LARSoft includes
#include "larcore/Geometry/Geometry.h"
#include "lardataobj/RawData/RawDigit.h"

// artvtk includes
#include "artvtk/artvtk/VtkVizData.hh"

// larvtk includes
// /#include "larvtk/vtkData/SpacePointsVtk.hh"

// vtk includes
#include "vtkSmartPointer.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkShortArray.h"
#include "vtkStringArray.h"
#include "vtkCell.h"
#include "vtkImageData.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkInformation.h"


namespace larvtk {
  class RawDigits2VTK;
}


class larvtk::RawDigits2VTK : public art::EDProducer {
public:
  explicit RawDigits2VTK(fhicl::ParameterSet const & p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  RawDigits2VTK(RawDigits2VTK const &) = delete;
  RawDigits2VTK(RawDigits2VTK &&) = delete;
  RawDigits2VTK & operator = (RawDigits2VTK const &) = delete;
  RawDigits2VTK & operator = (RawDigits2VTK &&) = delete;

  // Required functions.
  void produce(art::Event & e) override;

private:

  std::string myName_;
  std::string inputTag_;
};


/// Constructor for RawDigits2VTK
///
/// \param [in] p Parameter set for module
larvtk::RawDigits2VTK::RawDigits2VTK(fhicl::ParameterSet const & p) :
  myName_("SHOULD NEVER SEE THIS"),
  inputTag_( p.get<std::string>("inputTag"))
{
  produces< artvtk::VtkVizData >();

  // Set the name
  std::ostringstream nameStream;
  nameStream << p.get<std::string>("module_type") << " (labelled " << p.get<std::string>("module_label") << ")";
  myName_ = nameStream.str();
}

/// Process the event
/// \param e The event information
void larvtk::RawDigits2VTK::produce(art::Event & e)
{

  // Need the geometry service
  art::ServiceHandle<  geo::Geometry > geom;

  LOG_INFO("RawDigits2VTK") << "Max Planes=" << geom->MaxPlanes() << ", Max wires=" << geom->MaxWires();

  auto mb = vtkSmartPointer<vtkMultiBlockDataSet>::New();
  mb->SetNumberOfBlocks( 1 );
  unsigned int blockNum = 0;

  // Get the data corresponding to this tag
  auto digis = * ( e.getValidHandle< std::vector<raw::RawDigit> >(inputTag_) );

  auto nSamples = digis[0].Samples();

  // Set up the vtkImageData
  vtkSmartPointer<vtkImageData> img = vtkSmartPointer<vtkImageData>::New();
  img->SetDimensions(digis[0].Samples(), geom->MaxWires(), 1 );
  img->SetOrigin(0, 0, 0);
  img->SetSpacing(1, 1, 1);

  // Make vector of short vectors for planes
  std::vector< vtkSmartPointer<vtkShortArray> > adcs( geom->MaxPlanes() );
  std::vector< std::string > planeNames = { "U", "V", "Z" };
  {
    unsigned int i = 0;
    for ( auto & anADCSet : adcs ) {
      anADCSet = vtkSmartPointer<vtkShortArray>::New();
      anADCSet->SetNumberOfComponents(1);
      anADCSet->SetNumberOfTuples(geom->MaxWires() * digis[0].Samples());
//      if ( i > 2 ) throw cet::exception("RawDigits2VTK") << "Too many planes\n";
       anADCSet->SetName( planeNames[i].c_str() );
      img->GetPointData()->AddArray(anADCSet);
      ++i;
    }
  }

  // We're going to make an unstructured grid for the wire data
  // x = ChannelID
  // y = TDC tick
  // z = 0
  // Metadata ADC
  // We'll do a grid per view (uboone has 3 views (0=U, 1=V, 2=W)
  for ( auto const & digi : digis ) {

    // Get the wire information that corresponds to this channel
    std::vector<geo::WireID> wires = geom->ChannelToWire(digi.Channel());

    // Just look at the first wire associated with the channel (should be only one anyway)
    if ( wires.size() > 1 ) {
      LOG_ERROR("RawDigits2VTK") << "Channel " << digi.Channel() << " corresponds to " << wires.size() << " wires";
    }
    auto wire = wires[0];

    if ( digi.Compression() != 0 ) {
      LOG_ERROR("RawDigits2VTK") << "Channel " << digi.Channel() << " has compressed ADC - not sure what we'll get";
    }

    LOG_INFO("RawDigits2VTK") << "There are " << digi.Samples() << " for wire " << wire.Wire << " in plane " << wire.Plane;
    // Loop over ADC counts
    for ( size_t i = 0; i < digi.Samples(); ++i ){
      // adcs[wire.Plane]->SetTuple1(i*geom->MaxWires() + wire.Wire, static_cast<double>(digi.ADC(i)));
      adcs[wire.Plane]->SetTuple1(nSamples*wire.Wire + i, static_cast<double>(digi.ADC(i)));
    }
  }

  // Add this grid to the multiblockdataset
  mb->SetBlock( blockNum, img );
  mb->GetMetaData( blockNum )->Set(vtkCompositeDataSet::NAME(), "RawDigits");

  // Put it into the event
  auto vtkVizData = std::make_unique< artvtk::VtkVizData >(mb, myName_);
  e.put(std::move( vtkVizData ) );

}

DEFINE_ART_MODULE(larvtk::RawDigits2VTK)
