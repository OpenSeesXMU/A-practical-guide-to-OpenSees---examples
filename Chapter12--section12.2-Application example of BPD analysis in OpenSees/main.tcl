wipe
model basic -ndm 2 -ndf 2
if { [file exists output] == 0 } {
    file mkdir output;
    }
set nx 40;set dx 1.0;set horizon 1.5;
source nodebuild.tcl
nodebuild $nx $nx $dx
#puts "nodenum=$nodenum"
uniaxialMaterial Elastic 1 	2.0e11
source elementbuild.tcl
elementbuild $nodenum [expr $horizon*$dx] $nx
fixX 0.0 1 1
set temp [eigen 1]
scan $temp "%e"  w1s
set w1 [expr sqrt($w1s)]

set ksi 0.02
set a0 0
set a1 [expr $ksi*2.0/$w1]
rayleigh $a0 0.0 $a1 0.0


recorder Node -file Dispx.out -nodeRange 1  $nodenum  -dof 1 disp
recorder Node -file Dispy.out -nodeRange 1  $nodenum  -dof 2 disp
recorder Element -file force.out -ele 4641  globalForce;
set force 20.0
set timestep 0.000000025
source timeSeries.tcl
#source rigidLink.tcl
   pattern MultipleSupport 1  {
   groundMotion 11 Plain -disp 22
   for { set i [expr $nodenum-$nx+1] }  { $i <= $nodenum} { incr i}  {
     imposedMotion  $i   1  11
   }

   #imposedMotion  [expr $nodenum-$nx/2]  1  11
}      
constraints Transformation
numberer Plain
system BandGeneral
test NormDispIncr 1.0e-8 6 2
#algorithm Newton
algorithm Linear
integrator CentralDifference
#integrator LoadControl 1
analysis Transient
for { set times 1 }  { $times <= 4000} { incr times}  {
analyze 1 $timestep
for { set i 2984 }  { $i <= 3080} { incr i}  {
if {$active($i,1)==1} {

set cAx [expr [nodeCoord $bond($i,1) 1]+[nodeDisp $bond($i,1) 1]];
set cAy [expr [nodeCoord $bond($i,1) 2]+[nodeDisp $bond($i,1) 2]];
set cBx [expr [nodeCoord $bond($i,2) 1]+[nodeDisp $bond($i,2) 1]];
set cBy [expr [nodeCoord $bond($i,2) 2]+[nodeDisp $bond($i,2) 2]];

set length [expr pow(pow(($cBx-$cAx),2)+pow(($cBy-$cAy),2),0.5)]
#puts "length=$length"
if {$length>1.5} {
    puts "remove ele $i"
    
    remove element $i
	set active($i,1) 0
    }
    }}
                    
					
}
wipe
