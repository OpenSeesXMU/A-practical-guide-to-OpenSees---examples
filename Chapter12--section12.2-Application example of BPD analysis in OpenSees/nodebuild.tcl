proc nodebuild { ndivx  ndivy dx}  {
set m 0.01
global nodenum;
set nodenum 0
for { set i 1 }  { $i <= $ndivx} { incr i}  {
for { set j 1 }  { $j <= $ndivy} { incr j}  {
set nodenum [expr $nodenum+1]
node $nodenum [expr ($i-1)*$dx] [expr ($j-1)*$dx]  -mass   $m  $m


}
}
}
