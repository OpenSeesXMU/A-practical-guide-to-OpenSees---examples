#==============================================
#      Created by XMU
#      |units: m、kg、sec、N、Pa    |
# #====================================================    
global NodeIDLeft NodeIDRight DataB Ar Gr Iry Er Jr Irz mrail Lele g wb miuRail zr Nnode transfRail;
# =============================
set Nnode 200;
set zr 0.15;
set wb 1.505;
set Lele 0.6;

for {set i 1} {$i<=$Nnode} {incr i 1} {
	# the left one               x                y                z
	node [expr $i+10000]  [expr ($i-1)*$Lele]   [expr -$wb/2]  [expr -$zr]
	lappend NodeIDLeft  [expr $i+10000]
	# the right one               x                y                z
	node [expr $i+50000]  [expr ($i-1)*$Lele]   [expr  $wb/2]  [expr -$zr]
	lappend NodeIDRight  [expr $i+50000]
}

for {set i 1} {$i<=$Nnode} {incr i 1} {#should be "incr i 2" if rail is flexible
	fix [expr $i+10000] 1 1 1     1 1 1;
	fix [expr $i+50000] 1 1 1     1 1 1;
}

set transfRail 2;
geomTransf Linear $transfRail 0 0 1;#vec_xz X x = y

set Er 2.06e11; 
set miuRail 0.296;

set Ar 7.745e-3; 
set Gr [expr $Er/(1+$miuRail)/2.0];
set Jr 2.104e-6;
set Irz 5.24e-6;
set Iry 3.217e-5;#
set mrail 60.64;


for {set i 1} {$i<$Nnode} {incr i 1} {
	# the left elements
	element elasticBeamColumn [expr $i+10000] [expr $i+10000] [expr $i+10001] $Ar $Er $Gr $Jr $Iry $Irz $transfRail -mass $mrail
	# the right elements
	element elasticBeamColumn [expr $i+50000] [expr $i+50000] [expr $i+50001] $Ar $Er $Gr $Jr $Iry $Irz $transfRail -mass $mrail
}