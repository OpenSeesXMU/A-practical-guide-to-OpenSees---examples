proc elementbuild { nnum  horizon ndivx}  {
	#set filename "ele.tcl"
	#set content [open $filename w+] 
	
	
global elenum;
global active;
global bond;
set elenum 0
for { set i 1 }  { $i <= [expr $nnum-1]} { incr i}  {
if { [expr $i+$ndivx+int($horizon)+1]<= $nnum} {
set maxnode [expr $i+$ndivx+int($horizon)+1]
} else {
set maxnode $nnum
}
for { set j [expr $i+1] }  { $j <= $maxnode} { incr j}  {
if {761<=$i && $i<=781 && 801<=$j && $j<=821} {} else {
set cAx [nodeCoord $i 1];
set cAy [nodeCoord $i 2];
set cBx [nodeCoord $j 1];
set cBy [nodeCoord $j 2];
set length [expr pow(pow(($cBx-$cAx),2)+pow(($cBy-$cAy),2),0.5)];
if {$length <= $horizon} {
set elenum [expr $elenum+1]
#puts "length=$length,horizon=$horizon,elenum=$elenum"
element truss    $elenum    $i    $j  1.0  1
#puts $content "element truss    $elenum    $i    $j  1.0  1"
set bond($elenum,1) $i;set bond($elenum,2) $j;
set active($elenum,1) 1
}

}
}
}
#close $content
}
