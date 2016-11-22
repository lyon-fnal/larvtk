//
// Created by Adam Lyon on 10/10/16.
//

#ifndef LARVTK_SPACEPOINTSVTK_HH
#define LARVTK_SPACEPOINTSVTK_HH

#include "lardataobj/RecoBase/SpacePoint.h"

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

namespace larvtk {
  class SpacePointsVtk;
}


/// Represent a set of SpacePoints in VTK
class larvtk::SpacePointsVtk {
public:

  /// Constructor
  SpacePointsVtk();

  /// Add a SpacePoint
  /// \param tag The inputTag that made the SpacePoint
  /// \param sp The SpacePoint itself
  void addSpacePoint(const std::string& tag, const recob::SpacePoint& sp);

  /// Get the vtkPolyData for the SpacePoints
  /// \return a vtkSmartPointer<vtkPolyData> representing the grid
  vtkSmartPointer<vtkPolyData> grid() const;

private:
  // One VtkPolyData object for all of the space hits
  // XYZ -> Points
  // Vertex cells
  // metadata will be input tag, ID, diagonal error matrix elements, chisq
  vtkSmartPointer<vtkPoints> pts_;
  vtkSmartPointer<vtkCellArray> cells_;
  vtkSmartPointer<vtkIntArray> ids_;
  vtkSmartPointer<vtkDoubleArray> err_;
  vtkSmartPointer<vtkDoubleArray> chisq_;
  vtkSmartPointer<vtkStringArray> tag_;
};


#endif //LARVTK_SPACEPOINTSVTK_HH
