//
// Created by Adam Lyon on 10/10/16.
//

#ifndef LARVTK_TRACKSVTK_HH
#define LARVTK_TRACKSVTK_HH

#include "lardataobj/RecoBase/Track.h"

// VTK includes
#include "vtkSmartPointer.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkStringArray.h"
#include "vtkCell.h"
#include "vtkPolyData.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkInformation.h"
#include "vtkNew.h"

namespace larvtk {
  class TracksVtk;
}


/// Represent a set of Tracks in VTK
class larvtk::TracksVtk {
public:

  /// Constructor
  TracksVtk();

  /// Add a SpacePoint
  /// \param tag The inputTag that made the SpacePoint
  /// \param t The Track itself
  void addTrack(const std::string& tag, const recob::Track& t);

  /// Get the vtkPolyData for the Tracks
  /// \return a vtkSmartPointer<vtkPolyData> representing the grid
  vtkSmartPointer<vtkPolyData> grid() const;

private:
  vtkNew<vtkPoints> pts_;
  vtkNew<vtkCellArray> cells_;
  vtkNew<vtkIntArray> ids_;
  vtkNew<vtkDoubleArray> direction_;
  vtkNew<vtkDoubleArray> momentum_;
  vtkNew<vtkDoubleArray> dQdx_;
  vtkNew<vtkStringArray> tag_;
  unsigned int nViews_;
};


#endif //LARVTK_TRACKSVTK_HH
