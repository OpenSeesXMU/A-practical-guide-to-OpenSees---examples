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