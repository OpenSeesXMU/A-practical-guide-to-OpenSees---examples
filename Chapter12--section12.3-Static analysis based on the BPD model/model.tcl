logFile "mp.log"
wipe
model basic -ndm 3 -ndf 3
set x 0.15; 
set y 0.30; 
set z 0.15;
set dx 0.015; 
set dy 0.015;
set dz 0.015;
set A [expr $dx*$dx/4*3.14];
set cfpc -8.2e6
set cepsc0 -0.003
set cfpcu -1.0e6
set cepsU -0.035
set clambda 0.1
set cft 2.0e6
set cEts 1e8

set horizonrate 2.015;
set horizonratem 3.0;
set horizon [expr $horizonrate*$dx];
set radij   [expr 0.5*$dx]
set ndivx [expr int($x/$dx)];
set ndivy [expr int($y/$dx)];
set ndivz [expr int($z/$dx)];
set nodenum 0;
for { set i 0 }  { $i <= $ndivx} { incr i}  {
for { set j 0 }  { $j <= $ndivy} { incr j}  {
for { set k 0 }  { $k <= $ndivz} { incr k}  {
set nodenum [expr $nodenum+1];set nodeid($i,$j,$k) $nodenum;
set nodecoordx($nodeid($i,$j,$k)) [expr $dx*($i)];
set nodecoordy($nodeid($i,$j,$k)) [expr $dy*($j)];
set nodecoordz($nodeid($i,$j,$k)) [expr $dz*($k)];
set tag($nodeid($i,$j,$k)) 1
node $nodeid($i,$j,$k) $nodecoordx($nodeid($i,$j,$k))	$nodecoordy($nodeid($i,$j,$k)) 	$nodecoordz($nodeid($i,$j,$k))
}}}

source elements.tcl

for { set i 0 }  { $i <= $ndivx} { incr i}  {
   for { set k 0 }  { $k <= $ndivz} { incr k}  {
        fix $nodeid($i,0,$k) 1 1 1  
		#puts "$nodeid($i,0,$k)"
}}
for { set i 0 }  { $i <= $ndivx} { incr i}  {
for { set k 0 }  { $k <= $ndivz} { incr k}  {
if { $nodeid([expr int($ndivx/2)],$ndivy,[expr int($ndivz/2)]) != 	$nodeid($i,$ndivy,$k)   } {
equalDOF $nodeid([expr int($ndivx/2)],$ndivy,[expr 	int($ndivz/2)]) $nodeid($i,$ndivy,$k)   2  		 }}}
for { set i 0 }  { $i <= $ndivx} { incr i}  {
   for { set k 0 }  { $k <= $ndivz} { incr k}  {
          fix  $nodeid($i,$ndivy,$k)   1   0 1
	}}
pattern Plain 1 Linear {
for { set i 0 }  { $i <= $ndivx} { incr i}  {
   for { set k 0 }  { $k <= $ndivz} { incr k}  {
        load  $nodeid($i,$ndivy,$k)  0.0 -2.0e6 0.0
}}}
recorder Node -file reaction.out  -node 1 2 3 4 5 6 7 8 9 10 11 232 233 234 235 236 237 238 239 240 241 242 463 464 465 466 467 468 469 470 471 472 473 694 695 696 697 698 699 700 701 702 703 704 925 926 927 928 929 930 931 932 933 934 935 1156 1157 1158 1159 1160 1161 1162 1163 1164 1165 1166 1387 1388 1389 1390 1391 1392 1393 1394 1395 1396 1397 1618 1619 1620 1621 1622 1623 1624 1625 1626 1627 1628 1849 1850 1851 1852 1853 1854 1855 1856 1857 1858 1859 2080 2081 2082 2083 2084 2085 2086 2087 2088 2089 2090 2311 2312 2313 2314 2315 2316 2317 2318 2319 2320 2321 -dof 2 reaction
recorder Node -file disp.out  -node $nodeid([expr 	$ndivx/2],$ndivy,[expr $ndivz/2]) -dof 2 disp
set startT [clock seconds]
constraints Transformation
test NormDispIncr 2.0e-5 10 1
algorithm Newton
system BandGeneral
numberer Plain
integrator DisplacementControl $nodeid([expr 	$ndivx/2],$ndivy,[expr $ndivz/2]) 2 -0.000005
analysis Static
analyze 400
set endT [clock seconds]
puts "Over time: [expr $endT-$startT] seconds."

