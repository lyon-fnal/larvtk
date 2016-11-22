//
// Created by Adam Lyon on 10/10/16.
//

#include "larvtk/vtkData/PFParticleSpacePointsVtk.hh"
#include "vtkFieldData.h"
#include "vtkRearrangeFields.h"
#include <sstream>

#include "messagefacility/MessageLogger/MessageLogger.h"

larvtk::PFParticleSpacePointsVtk::PFParticleSpacePointsVtk(const recob::PFParticle &particle, const std::string &tag) :
    tag_{ tag },
    name_ {},
    spacePoints_(),
    pdg_( vtkSmartPointer<vtkIntArray>::New() ),
    isPrimary_( vtkSmartPointer<vtkIntArray>::New() ),
    numDaughters_( vtkSmartPointer<vtkIntArray>::New() ),
    self_( vtkSmartPointer<vtkIntArray>::New() ),
    parent_( vtkSmartPointer<vtkIntArray>::New() ),
    daughters_( vtkSmartPointer<vtkIntArray>::New() )
{
  pdg_->SetName("PDG");
  pdg_->InsertNextValue(particle.PdgCode());

  isPrimary_->SetName("isPrimary");
  isPrimary_->InsertNextValue(particle.IsPrimary());

  numDaughters_->SetName("numberOfDaughters");
  numDaughters_->InsertNextValue(particle.NumDaughters());

  self_->SetName("self");
  self_->InsertNextValue(static_cast<int>(particle.Self()));

  parent_->SetName("parent");
  parent_->InsertNextValue(static_cast<int>(particle.Parent()));

  daughters_->SetName("daughters");
  for (auto & aDaughter : particle.Daughters()) {
    daughters_->InsertNextValue(aDaughter);
  }

  // We want to create a name for this PFParticle to put in the multiblockdataset name
  // Remember that ParaView hover doesn't show strings except in the MB dataset name
  std::ostringstream nameStream;
  nameStream << "Tag=" << tag_ << ", Self=" << particle.Self() << ", PDG=" << particle.PdgCode();
  name_ = nameStream.str();
}

void larvtk::PFParticleSpacePointsVtk::addSpacePoint(const recob::SpacePoint &sp) {
  spacePoints_.addSpacePoint(tag_, sp);
}

vtkSmartPointer<vtkPolyData> larvtk::PFParticleSpacePointsVtk::grid() const {
  // Get the grid for the space points
  auto grid = spacePoints_.grid();

  // Add to the grid
  grid->GetFieldData()->AddArray(pdg_);
  grid->GetFieldData()->AddArray(isPrimary_);
  grid->GetFieldData()->AddArray(numDaughters_);
  grid->GetFieldData()->AddArray(self_);
  grid->GetFieldData()->AddArray(parent_);
  grid->GetFieldData()->AddArray(daughters_);

  return grid;
}
