## Reference:Displacement-based design of slender reinforced concrete structural walls- experimental verification
# John H. Thomsen and Hohn W. Wallace
wipe;
model basic -ndm 2 -ndf 3
set startTime [clock seconds];

source DisplayModel2D.tcl
source DisplayPlane.tcl
set L 1032;
set H 3660;
set nL 6;
set nH 10;#division per floor
set nColDiv 5;
set deltL [expr $L/$nL];
set deltH [expr $H/$nH/$nColDiv]
set deltQuadH [expr $H/$nH]
set t 102.0;
set k1 0.25;
set k2 1500
set data "RW2-sensitivity-boundary-$nColDiv-$nL-$nH-$k1-$k2"
file mkdir "$data"


set nodeID 0;
set LsideColNodeID 1;
for {set i 0} {$i<=$nH*$nColDiv} {incr i} {
    set nodeID [expr $nodeID+1];
    node $nodeID [expr 0*$deltL] [expr $i*$deltH];
}
set RsideColNodeID [expr $nodeID+1]
for {set i 0} {$i<=$nH*$nColDiv} {incr i} {
    set nodeID [expr $nodeID+1];
    node $nodeID [expr $nL*$deltL] [expr $i*$deltH];
}
set TopBeamNodeID [expr $nodeID+1]
for {set j 1} {$j<$nL} {incr j} {
   set nodeID [expr $nodeID+1];
   node $nodeID [expr $j*$deltL] $H;
} 
fixY 0.0 1 1 1
set ControlNode [expr $nodeID+1]
set nodeID [expr $nodeID+1];
node $nodeID [expr $L/2.0] [expr $H+305/2.0];

uniaxialMaterial Concrete02  1             -47.6     -0.0032        -33   -0.015          0.1      2.6       3.0e3 ;  
uniaxialMaterial Concrete02  2             -42.8    -0.0021        -8.56      -0.02         0.1      2.1        3.0e3 ; 
uniaxialMaterial Steel02 3                   395.2   200000.0       0.0185    18.0   0.925 0.15   ;              
set colWidth  153.0
set colDepth  102.0
set cover 19.0
set As 71.2; # area of no. 8 bars
set cy1 [expr $colDepth/2.0]
set cz1 [expr $colWidth/2.0]
section Fiber 1 {
	patch rect 1 20 10 [expr $cover-$cy1] [expr $cover-$cz1] [expr $cy1-$cover] [expr $cz1-$cover]
	# Create the concrete cover fibers (top, bottom, left, right)
	patch rect 2  20 1 [expr -$cy1] [expr $cz1-$cover] $cy1 $cz1
	patch rect 2  20 1 [expr -$cy1] [expr -$cz1] $cy1 [expr $cover-$cz1]
	patch rect 2  1 10  [expr -$cy1] [expr $cover-$cz1] [expr $cover-$cy1] [expr $cz1-$cover]
	patch rect 2  1 10  [expr $cy1-$cover] [expr $cover-$cz1] $cy1 [expr $cz1-$cover]
	# Create the reinforcing fibers (2 layers)
	layer straight 3 4 $As  [expr $cover-$cy1] [expr $cz1-$cover] [expr $cy1-$cover] [expr $cz1-$cover]
	layer straight 3 4 $As  [expr $cover-$cy1] [expr $cover-$cz1] [expr $cy1-$cover] [expr $cover-$cz1]
} 
section Elastic 2 1e10 100 1e10;
set beamType dispBeamColumn
geomTransf Linear 1 
set np 5
# -------------定义柱单元           单元号  节点1  节点2   高斯点个数   截面号   变换号

for {set i 1} {$i<=$nH*$nColDiv} {incr i} {
    # the left columns
    element $beamType [expr 1000+$i] [expr $LsideColNodeID+$i-1] [expr $LsideColNodeID+$i] $np 1 1;
    # the right columns
    element $beamType [expr 2000+$i] [expr $RsideColNodeID+$i-1] [expr $RsideColNodeID+$i] $np 1 1; 
} 
element $beamType [expr 3000] $ControlNode [expr $LsideColNodeID+$nH*$nColDiv] $np 2 1;	
for {set i 1} {$i<$nL} {incr i} {
    element $beamType [expr 3000+$i] $ControlNode [expr $TopBeamNodeID+$i-1] $np 2 1;	
}  
element $beamType [expr 3000+$nL] $ControlNode [expr $RsideColNodeID+$nH*$nColDiv] $np 2 1;	
  
puts "frmae model"
# Create ModelBuilder for 2D element (with two-dimensions and 2 DOF/node)# -------------------------------------------------------------------------
model basic -ndm 2 -ndf 2
set quadNodeID [expr $nodeID+1]
for {set i 0} {$i<=$nH} {incr i} {
   for {set j 0} {$j<=$nL} {incr j} {
       set nodeID [expr $nodeID+1];
       node $nodeID [expr $j*$deltL] [expr $i*$deltQuadH];
    }
}
puts "quad node built"
# tie nodes between beam,column and 2D elements
for {set i 1} {$i <= $nH} {incr i} {
    equalDOF [expr $LsideColNodeID+$i*$nColDiv] [expr $quadNodeID+($nL+1)*$i] 1 2;
    equalDOF [expr $RsideColNodeID+$i*$nColDiv] [expr $quadNodeID+($nL+1)*$i+$nL] 1 2;
}
for {set j 1} {$j<$nL} {incr j} {
    equalDOF  [expr $TopBeamNodeID+$j-1] [expr $quadNodeID+($nL+1)*$nH+$j] 1 2;
}
fixY 0.0 1 1;

set wfc1 42.8;
set wfc2 45.7;
set wfc3 40.8;
set wfc4 41.3;
set wfy 336;
set wE  200000.0;
set rou1  0.0024;
set rou2  0.0024;
# uniaxialMaterial    SmearedSteel  11     $wfy    $wE  $wfc1  $rou1  
# uniaxialMaterial    SmearedSteel  12    $wfy    $wE  $wfc1  $rou2  
uniaxialMaterial Steel02 11     $wfy    $wE     0.035    18.0   0.925 0.15   ; 
uniaxialMaterial Steel02 12     $wfy    $wE     0.035    18.0   0.925 0.15   ; 

uniaxialMaterial SmearedConcrete  13  [expr -$wfc1]   -0.0021 
uniaxialMaterial SmearedConcrete  14  [expr -$wfc2]   -0.0021
uniaxialMaterial SmearedConcrete  15  [expr -$wfc3]   -0.0021
uniaxialMaterial SmearedConcrete  16  [expr -$wfc4]   -0.0021

# uniaxialMaterial Concrete02  13   [expr -$wfc1]   -0.0021   -8.2 -0.0115 0.1      2.6       3.0e3 ; 


set pi 3.141592654
set t 102.0;
nDMaterial SmearedCompositePlaneStress 111  0.0 11 12 13 13 13 13 [expr 0*$pi]  [expr 0.5*$pi]   $rou1 $rou2  $wfc1 $wfy $wE  0.002 $k1 $k2
nDMaterial SmearedCompositePlaneStress 112  0.0 11 12 14 14 14 14 [expr 0*$pi]  [expr 0.5*$pi]   $rou1 $rou2  $wfc2 $wfy $wE  0.002 $k1 $k2
nDMaterial SmearedCompositePlaneStress 113  0.0 11 12 15 15 15 15 [expr 0*$pi]  [expr 0.5*$pi]   $rou1 $rou2  $wfc3 $wfy $wE  0.002 $k1 $k2
nDMaterial SmearedCompositePlaneStress 114  0.0 11 12 16 16 16 16 [expr 0*$pi]  [expr 0.5*$pi]   $rou1 $rou2  $wfc4 $wfy $wE  0.002 $k1 $k2

for {set i 1} {$i<=$nH} {incr i} {
   for {set j 1} {$j<=$nL} {incr j} {
      element quad [expr 5000+($i-1)*$nL+$j] [expr $quadNodeID + ($i-1)*($nL+1) + $j -1] \
	  [expr $quadNodeID + ($i-1)*($nL+1) +$j ]  [expr $quadNodeID + ($i)*($nL+1) +$j ] \
	  [expr $quadNodeID + ($i)*($nL+1) +$j -1]  $t  PlaneStress 111;
     }  
}

# for {set i [expr $nColDiv+1]} {$i<=[expr 2*$nColDiv]} {incr i} {
   # for {set j 1} {$j<=$nL} {incr j} {
      # element quad [expr 5000+($i-1)*$nL+$j] [expr $quadNodeID + ($i-1)*($nL+1) + $j -1] \
	  # [expr $quadNodeID + ($i-1)*($nL+1) +$j ]  [expr $quadNodeID + ($i)*($nL+1) +$j ] \
	  # [expr $quadNodeID + ($i)*($nL+1) +$j -1]  $t  PlaneStress 112;
     # }  
# }
# for {set i [expr 2*$nColDiv+1]} {$i<=[expr 3*$nColDiv]} {incr i} {
   # for {set j 1} {$j<=$nL} {incr j} {
      # element quad [expr 5000+($i-1)*$nL+$j] [expr $quadNodeID + ($i-1)*($nL+1) + $j -1] \
	  # [expr $quadNodeID + ($i-1)*($nL+1) +$j ]  [expr $quadNodeID + ($i)*($nL+1) +$j ] \
	  # [expr $quadNodeID + ($i)*($nL+1) +$j -1]  $t  PlaneStress 113;
     # }  
# }
# for {set i [expr 3*$nColDiv+1]} {$i<=[expr 4*$nColDiv]} {incr i} {
   # for {set j 1} {$j<=$nL} {incr j} {
      # element quad [expr 5000+($i-1)*$nL+$j] [expr $quadNodeID + ($i-1)*($nL+1) + $j -1] \
	  # [expr $quadNodeID + ($i-1)*($nL+1) +$j ]  [expr $quadNodeID + ($i)*($nL+1) +$j ] \
	  # [expr $quadNodeID + ($i)*($nL+1) +$j -1]  $t  PlaneStress 114;
     # }  
# }

set N  378000.0;
pattern Plain 1 "Linear" {
# for {set i 1} {$i<$nL} {incr i} {
       # load [expr ($nH+1)*100+$i+1] 0 [expr -$N/$nL];
       # }
   # load [expr ($nH+1)*100+1] 0 [expr -$N/(2*$nL)];
   # load [expr ($nH+1)*100+$nL+1] 0 [expr -$N/(2*$nL)];
   load $ControlNode 0 [expr -$N] 0;
}
DisplayModel2D DeformedShape 
# DisplayModel2D NodeNumbers
recorder Node -file  $data/disp.out -time  -node  $ControlNode -dof 1 disp
region 1 -nodeRange [expr $quadNodeID] [expr $quadNodeID+$nL]  -node $LsideColNodeID $RsideColNodeID
recorder Node -file  $data/force.out -time  -region 1 -dof 1 reaction;
recorder Node -file $data/disprofile.out -time -node [expr $LsideColNodeID+ $nColDiv] [expr $LsideColNodeID+ 2*$nColDiv] [expr $LsideColNodeID+ 3*$nColDiv] [expr $LsideColNodeID+ 4*$nColDiv] -dof 1 disp;

puts "model built"
system BandGeneral
constraints Transformation
numberer Plain
test NormDispIncr 1.0e-3 100 
algorithm KrylovNewton
integrator LoadControl 0.1
analysis Static
analyze 10
puts "重力分析完成....."


loadConst  -time 0.0
set P 1000.0;
pattern Plain 2 "Linear" {

    load $ControlNode $P 0 0;

}
system BandGeneral 
constraints Transformation
numberer Plain
test NormDispIncr 1.0e-3 200
algorithm Newton
analysis Static
  
set iDmax "3.788 9.986 16.41 24.22 38.91 54.52 71.05 71.28";
set Dincr 0.02;
set Fact 1.0;              
source GeneratePeaks.tcl
set CycleType Full;
foreach Dmax $iDmax {
	set iDstep [GeneratePeaks $Dmax $Dincr $CycleType $Fact];	# this proc is defined above
      for {set i 1} {$i <= 1} {incr i 1} {
		set zeroD 0
		set D0 0.0
		foreach Dstep $iDstep {
			set D1 $Dstep
			set Dincr [expr $D1 - $D0]
			integrator DisplacementControl  $ControlNode 1 $Dincr
			analysis Static
      set ok [analyze 1]
      if {$ok != 0} {
			puts "Trying Newton with Initial Tangent .."
			test NormDispIncr 1.0e-3 50
			algorithm Newton -initial
			set ok [analyze 1]
			test NormDispIncr 1.0e-3 50
			algorithm Newton
		}
		if {$ok != 0} {
			puts "Trying Broyden .."
			algorithm Broyden 8
			set ok [analyze 1 ]
			algorithm Newton
		}
		if {$ok != 0} {
			puts "Trying NewtonWithLineSearch .."
			algorithm NewtonLineSearch 0.8 
			set ok [analyze 1]
			algorithm Newton
		}
    if {$ok != 0} {
			puts "Trying KrylovNewton .."
			algorithm KrylovNewton 
			set ok [analyze 1]
			algorithm Newton
		}
      set D0 $D1;			# move to next step
      }
    }
  }	
	
set endTime [clock seconds];
puts "Execution time: [expr $endTime-$startTime]"
puts "低周反复分析完成..."
wipe;
exit;













