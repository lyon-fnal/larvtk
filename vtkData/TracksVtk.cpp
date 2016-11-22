//
// Created by Adam Lyon on 10/21/16.
//

#include "larvtk/vtkData/TracksVtk.hh"

#include "cetlib/exception.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "larcore/Geometry/Geometry.h"

#include "vtkPolyLine.h"

larvtk::TracksVtk::TracksVtk() :
  pts_(),
  cells_(),
  ids_(),
  direction_(),
  momentum_(),
  dQdx_(),
  tag_(),
  nViews_()
{
  ids_->SetName("ID");

  direction_->SetName("direction");
  direction_->SetNumberOfComponents(3);
  direction_->SetComponentName(0, "x");
  direction_->SetComponentName(1, "y");
  direction_->SetComponentName(2, "z");

  momentum_->SetName("p");

  tag_->SetName("tag");

  // Determine the views
  art::ServiceHandle< geo::Geometry > geom;
  nViews_ = geom->Nviews();
  //if ( nViews_ > 2 ) throw cet::exception("TracksVtk") << "Too many views\n";
  const std::vector<std::string> viewNames = {"U", "V", "Z"};
  dQdx_->SetName("dQdx");
  dQdx_->SetNumberOfComponents( nViews_ );
  for (unsigned int i = 0; i < nViews_; ++i) {
    dQdx_->SetComponentName(i, viewNames[i].c_str());
  }
}

void larvtk::TracksVtk::addTrack(const std::string &tag, const recob::Track &t) {



  // Make the polyLine
  vtkNew<vtkPolyLine> pl;

  // Loop over trajectory points
  for (size_t i=0; i < t.NumberTrajectoryPoints(); ++i ) {

    auto v = t.LocationAtPoint(i);
    auto pointID = pts_->InsertNextPoint( v.x(), v.y(), v.z() );
    pl->GetPointIds()->InsertNextId(pointID);

    ids_->InsertNextValue(t.ID());

    auto const & dir = t.DirectionAtPoint(i);
    direction_->InsertNextTuple3( dir.x(), dir.y(), dir.z());

//    momentum_->InsertNextValue(t.MomentumAtPoint(i));

//    for (unsigned int v = 0; v < nViews_; ++v) {
//      dQdx_->SetComponent(pointID, v, t.DQdxAtPoint(i, static_cast<geo::View_t>(v)));
//    }

    tag_->InsertNextValue(tag);
  }
  cells_->InsertNextCell(pl.GetPointer());
}

vtkSmartPointer<vtkPolyData> larvtk::TracksVtk::grid() const {
  auto grid = vtkSmartPointer<vtkPolyData>::New();
  grid->SetPoints(pts_.GetPointer());
  grid->SetLines(cells_.GetPointer());
  grid->GetPointData()->AddArray(ids_.GetPointer());
  grid->GetPointData()->AddArray(direction_.GetPointer());
//  grid->GetPointData()->AddArray(momentum_.GetPointer());
//  grid->GetPointData()->AddArray(dQdx_.GetPointer());
  grid->GetPointData()->AddArray(tag_.GetPointer());
  return grid;
}