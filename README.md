To run a sim, you need an environment with ROOT and Geant4. I used conda: 
  conda create -n mu2e -c conda-forge geant4 root cmake make compilers

Next go into the build folder to make the geometry:
  cmake ..
  make -j4

You prep a simulation with a mac file. run.mac is a geantino with tracking verbosity to do geometry checks. I wrote the output of a test to test1.txt
run1.mac is an optical sim to test photons. Currently this is monoenergetic and single direction but you can control these things easily in the mac file

to run a sim: 
  ./FiberSim ../run1.mac

mac files let you set a particle (type, energy, location, etc), the number of events (particles generated), and the output root file name and location. 

src and include make the geometry and tell Geant4 how to store the data in a root file
ROOT is where I store my output files
old is where I keep backups in txt files in case I need them. The tester geometry is in there right now

If you use VSCode, I recommend the JSROOT extension. It works like the TBrowser so you can quickly check the contents of the file
the plots aren't pretty or useful, but I like being able to find the name and location of things this way
