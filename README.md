# larvtk
VTK interface for LArSoft

To get started quickly...

```bash
source /cvmfs/uboone.opensciencegrid.org/products/setup_uboone.sh
setup uboonecode v06_15_00 -q e10:prof
cd <somewhere>
mrb newDev
source localProducts*/setup
cd srcs
git clone https://github.com/lyon-fnal/artvtk.git
git clone https://github.com/lyon-fnal/larvtk.git
mrb uc  # Update the top level CMakeLists.txt file

export PRODUCTS=$PRODUCTS:/cvmfs/gm2.opensciencegrid.org/prod7/external
setup paraviewvtk v5_1_2 -q prof:e10

cd $MRB_BUILDDIR
setup ninja v1_6_0b
mrb b -C --generator ninja
ninja

cd $MRB_TOP
mkdir try
cd try

lar -c $MRB_SOURCE/larvtk/fcl/rawDigits2VTK.fcl -s /Users/lyon/Downloads/uboone_2_mc_events.root
```
