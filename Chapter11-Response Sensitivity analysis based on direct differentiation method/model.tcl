wipe
model basic -ndm 2 -ndf 2
reliability

# Create nodes & add to Domain - command: node nodeId xCrd yCrd
node 1   0.0  0.0
node 2   10.0  0.0  
node 3   20.0 0.0    
# Set the boundary conditions - command: fix nodeID xResrnt? yRestrnt?
fix 1 1 1 
fix 2 0 1
fix 3 0 1

uniaxialMaterial Steel01   1     248200   2.0e8  0.05

element truss     1      1     2    0.1    1
element truss     2      2     3    0.1    1

# ---------------- add sensitivity ---------------------------
parameter 1
addToParameter 1 -element 1 -material E
addToParameter 1 -element 2 -material E
recorder Node -file ddm2E.out   -time -precision 16 -node 2 -dof 1 "sensitivity 1"


# create a Recorder object for the nodal displacements at nodes
recorder Node -file node2.out -time  -precision 16   -node 2 -dof 1 2 disp
recorder Element -file force.out  -time  -precision 16    -ele 1  axialForce
recorder Element -file deformation.out  -time  -precision 16    -ele 1  deformations


timeSeries Path 1 -time {0 0.5 1.0 10000.0} -values {0 0.5 1.0 1.0 }
# ADD PATTERNo
pattern Plain 1 1 {
load 3 2e4 0.0
}


constraints Transformation
test NormDispIncr 1e-12 25  2
algorithm Newton
numberer RCM
system BandGeneral
integrator LoadControl 0.1;	
analysis Static     
sensitivityAlgorithm -computeAtEachStep

analyze 10

wipeAnalysis


# --------------------
pattern UniformExcitation    2     1    -accel "Series -factor 300 -filePath el.txt -dt 0.01"

mass  1  100.0 0.0 
mass  2  100.0 0.0 
mass  3  100.0 0.0 

# ADD PATTERN
constraints Transformation
test NormDispIncr 1e-12 25  2
algorithm Newton
numberer RCM
system BandGeneral   
integrator Newmark  0.5      0.25    
analysis Transient

sensitivityAlgorithm -computeAtEachStep
analyze 100 0.01

