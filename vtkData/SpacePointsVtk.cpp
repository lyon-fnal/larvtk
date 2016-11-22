//
// Created by Adam Lyon on 10/10/16.
//


#include "larvtk/vtkData/SpacePointsVtk.hh"

larvtk::SpacePointsVtk::SpacePointsVtk() :
    pts_( vtkSmartPointer<vtkPoints>::New() ),
    cells_( vtkSmartPointer<vtkCellArray>::New() ),
    ids_( vtkSmartPointer<vtkIntArray>::New() ),
    err_( vtkSmartPointer<vtkDoubleArray>::New() ),
    chisq_( vtkSmartPointer<vtkDoubleArray>::New() ),
    tag_( vtkSmartPointer<vtkStringArray>::New() )
{
  ids_->SetName("ID");

  err_->SetName("err");
  err_->SetNumberOfComponents(3);
  err_->SetComponentName(0, "ex");
  err_->SetComponentName(1, "ey");
  err_->SetComponentName(2, "ez");

  chisq_->SetName("chisq");

  tag_->SetName("tag");
}

void larvtk::SpacePointsVtk::addSpacePoint(const std::string &tag, const recob::SpacePoint &sp) {

  // Make the point, keeping the point ID
  auto pointID = pts_->InsertNextPoint(sp.XYZ());
  cells_->InsertNextCell(1, &pointID);

  // Add the metadata
  ids_->InsertNextValue(sp.ID());
  err_->InsertNextTuple3(sp.ErrXYZ()[0], sp.ErrXYZ()[1], sp.ErrXYZ()[2]);
  chisq_->InsertNextValue(sp.Chisq());
  tag_->InsertNextValue(tag);
}

vtkSmartPointer<vtkPolyData> larvtk::SpacePointsVtk::grid() const {
  auto grid = vtkSmartPointer<vtkPolyData>::New();
  grid->SetPoints(pts_);
  grid->SetVerts(cells_);
  grid->GetPointData()->AddArray(ids_);
  grid->GetPointData()->AddArray(err_);
  grid->GetPointData()->AddArray(chisq_);
  grid->GetPointData()->AddArray(tag_);
  return grid;
}