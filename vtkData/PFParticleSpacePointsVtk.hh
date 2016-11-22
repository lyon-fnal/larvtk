//
// Created by Adam Lyon on 10/10/16.
//

#ifndef LARVTK_PFPARTICLESPACEPOINTSVTK_HH
#define LARVTK_PFPARTICLESPACEPOINTSVTK_HH

#include "lardataobj/RecoBase/PFParticle.h"
#include "larvtk/vtkData/SpacePointsVtk.hh"


namespace larvtk {
  class PFParticleSpacePointsVtk;
}

class larvtk::PFParticleSpacePointsVtk {
public:

  /// Constructor
  /// \param particle The PFParticle that will get associated spacepoints
  PFParticleSpacePointsVtk(const recob::PFParticle &particle, const std::string &tag);

  /// Add a SpacePoint
  /// \param tag The inputTag that made the SpacePoint
  /// \param sp The SpacePoint itself
  void addSpacePoint(const recob::SpacePoint& sp);

  /// Get the vtkPolyData for the SpacePoints
  /// \return a vtkSmartPointer<vtkPolyData> representing the grid
  vtkSmartPointer<vtkPolyData> grid() const;

  /// Return the name
  /// \return the Name
  std::string name() const { return name_; }

private:

  std::string tag_;
  std::string name_;

  larvtk::SpacePointsVtk spacePoints_;

  vtkSmartPointer<vtkIntArray> pdg_;
  vtkSmartPointer<vtkIntArray> isPrimary_;
  vtkSmartPointer<vtkIntArray> numDaughters_;
  vtkSmartPointer<vtkIntArray> self_;
  vtkSmartPointer<vtkIntArray> parent_;
  vtkSmartPointer<vtkIntArray> daughters_;
};


#endif //LARVTK_PFPARTICLESPACEPOINTSVTK_HH
