////////////////////////////////////////////////////////////////////////
// Class:       FastRawDigits2VTK
// Plugin Type: producer (art v2_04_00)
// File:        FastRawDigits2VTK_module.cc
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
#include "vtkNew.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkPixel.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkShortArray.h"
#include "vtkStringArray.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPolyData.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkInformation.h"


namespace larvtk {
  class FastRawDigits2VTK;
}


class larvtk::FastRawDigits2VTK : public art::EDProducer {
public:
  explicit FastRawDigits2VTK(fhicl::ParameterSet const & p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  FastRawDigits2VTK(FastRawDigits2VTK const &) = delete;
  FastRawDigits2VTK(FastRawDigits2VTK &&) = delete;
  FastRawDigits2VTK & operator = (FastRawDigits2VTK const &) = delete;
  FastRawDigits2VTK & operator = (FastRawDigits2VTK &&) = delete;

  // Required functions.
  void produce(art::Event & e) override;

private:

  std::string myName_;
  std::string inputTag_;
};


/// Constructor for FastRawDigits2VTK
///
/// \param [in] p Parameter set for module
larvtk::FastRawDigits2VTK::FastRawDigits2VTK(fhicl::ParameterSet const & p) :
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
void larvtk::FastRawDigits2VTK::produce(art::Event & e)
{

  // Need the geometry service
  art::ServiceHandle<  geo::Geometry > geom;

  LOG_INFO("FastRawDigits2VTK") << "Max Planes=" << geom->MaxPlanes() << ", Max wires=" << geom->MaxWires();

  auto mb = vtkSmartPointer<vtkMultiBlockDataSet>::New();
  mb->SetNumberOfBlocks( geom->MaxPlanes() );

  // Get the data corresponding to this tag
  auto digis = * ( e.getValidHandle< std::vector<raw::RawDigit> >(inputTag_) );

  if ( geom->MaxPlanes() > 3 ) throw cet::exception("FastRawDigits2VTK") << "Too many planes\n";

  // We need an unstructured grid per view
  std::vector< std::string > planeNames = { "U", "V", "Z" };
  std::vector< vtkSmartPointer<vtkPoints> >          points( geom->MaxPlanes() );
  std::vector< vtkSmartPointer<vtkCellArray> >        cells( geom->MaxPlanes() );
  std::vector< vtkSmartPointer<vtkShortArray> >        adcs( geom->MaxPlanes() );

  // Make the vector contents
  {
    for ( unsigned int i = 0; i < geom->MaxPlanes(); ++i ) {
      points[i] = vtkSmartPointer<vtkPoints>::New();
      cells[i] = vtkSmartPointer<vtkCellArray>::New();
      adcs[i] = vtkSmartPointer<vtkShortArray>::New();
      adcs[i]->SetName( planeNames[i].c_str());
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
      LOG_ERROR("FastRawDigits2VTK") << "Channel " << digi.Channel() << " corresponds to " << wires.size() << " wires";
    }
    auto wire = wires[0];

    LOG_INFO("RawDigits2VTK") << "wire = " << wire.Wire << ", plane = " << wire.Plane;

    if ( digi.Compression() != 0 ) {
      LOG_ERROR("RawDigits2VTK") << "Channel " << digi.Channel() << " has compressed ADC - not sure what we'll get";
    }

    // Loop over ADC counts for this wire
    std::vector<vtkIdType> pointIds;
    for ( size_t tick = 0; tick < digi.Samples(); ++tick ) {

      // Add this information
      pointIds.push_back( points[wire.Plane]->InsertNextPoint(wire.Wire, tick, 0) );
      adcs[wire.Plane]->InsertNextValue(digi.ADC(tick));
    }

    // Set the cells for this ROI
    cells[wire.Plane]->InsertNextCell( pointIds.size(), &pointIds[0]);
  }

  for ( unsigned int i = 0; i < geom->MaxPlanes(); ++i ) {
    auto grid = vtkSmartPointer<vtkPolyData>::New();
    grid->SetPoints(points[i]);
    grid->SetLines(cells[i]);
    grid->GetPointData()->AddArray(adcs[i]);
    mb->SetBlock(i, grid);
    mb->GetMetaData(i)->Set(vtkCompositeDataSet::NAME(), planeNames[i].c_str());
  }

  // Put it into the event
  auto vtkVizData = std::make_unique< artvtk::VtkVizData >(mb, myName_);
  e.put(std::move( vtkVizData ) );

}

DEFINE_ART_MODULE(larvtk::FastRawDigits2VTK)
