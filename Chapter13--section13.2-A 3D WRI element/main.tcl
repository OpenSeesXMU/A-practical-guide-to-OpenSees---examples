wipe ; #==============================================
#      Created by XMU
# #====================================================    
if { [file exists ex1] == 0 } {
	file mkdir ex1;
} 
model basic -ndm 3 -ndf 6;

source rail.tcl;
source wheelset.tcl;

set dT 0.001;
set pVel 20;#m/s
set nL 20;
set omiga [expr $pVel/$R0*1.0];#53.14434782608696
set pInitLocation 0.0;# the initial location of the rearest wheel
set Loc1 [expr 0+$pInitLocation]
set Loc2 [expr $la+$pInitLocation]
set Loc3 [expr $lb+$pInitLocation]
set Loc4 [expr $la+$lb+$pInitLocation]

# =============================
set WheelP "0 0 0    0 0 0.25 $omiga $pVel $Ew $miuWheel $R0";
set RailL  "0 0 $zr -0.05 0 0 $Er $miuRail $Iry $Irz $Jr $Ar";
set RailR  "0 0 $zr  0.05 0 0 $Er $miuRail $Iry $Irz $Jr $Ar";
set DelList "0 0 0";#
set DelLocList "14.5 15 15.5";#

set NodeListL1 "[concat 1 $NodeIDLeft]";
set NodeListR1 "[concat 1001 $NodeIDRight]";

#Right wheel-rail elements 
#Notation: The WRI element provided here can only be used for rigid track example
element WheelRail 200001 $dT $Loc1 $nL -WPara $WheelP -RPara $RailR -NodeList $NodeListR1 UIC50Fine.txt irregRht.txt ML95_FineR.txt R;
#Left wheel-rail elements
element WheelRail 100001 $dT $Loc1 $nL -WPara $WheelP -RPara $RailL -NodeList $NodeListL1 UIC50Fine.txt irregLft.txt ML95_FineL.txt L;

recorder Element -file ex1/eleForceR1.txt -time  -ele 200001 -precision 10  localForce;
recorder Element -file ex1/eleForceL1.txt -time  -ele 100001 -precision 10  localForce;


set error 1.0e-3;
set maxIt 20;
constraints Transformation;
numberer Plain;
system BandGeneral;
test NormUnbalance $error $maxIt 1;
algorithm Newton;
integrator LoadControl  0.001;
analysis Static;

set startT [clock seconds];
analyze $nL;#
puts "!!!!!!!!!!!!!!!!!!!!!!!! static analysis finished !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"


# ----------------------------------------------------------------------------
wipeAnalysis;

constraints Transformation;
numberer Plain;
system BandGeneral;
test NormUnbalance $error $maxIt 1;

algorithm Newton;
integrator Newmark 0.5 0.25;
analysis Transient;
analyze 6000  $dT;

set endT [clock seconds]
puts "Execution time: [expr $endT-$startT] seconds."
wipe;
