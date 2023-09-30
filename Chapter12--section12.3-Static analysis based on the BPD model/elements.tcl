 set trussnum 0
 #puts "$nodeid(           0,0,0)"
for { set i 0 }  { $i <= $ndivx} { incr i}  {
for { set j 0 }  { $j <= $ndivy} { incr j}  {
for { set k 0 }  { $k <= $ndivz} { incr k}  {
set imax [expr int($i+$horizonratem)];
set imin [expr int($i-	$horizonratem)];
set jmax [expr int($j+$horizonratem)];
set jmin 	[expr int($j-$horizonratem)];
set kmax [expr 	int($k+$horizonratem)];
set kmin [expr int($k-$horizonratem)];
if {$imax>$ndivx} {
set imax [expr $ndivx]}
if {$jmax>$ndivy} {
set jmax [expr $ndivy] }
if {$kmax>$ndivz} {
	set kmax [expr $ndivz] }
	if {$imin<0} {
	set imin 0    }
	if {$jmin<0} {
	set jmin 0    }
	if {$kmin<0} {
	set kmin 0    }

	set cellnum($nodeid($i,$j,$k)) 0
	
	  for { set l $imin }  { $l <= $imax} { incr l}  {
         for { set m $jmin }  { $m <= $jmax} { incr m}  {
           for { set n $kmin }  { $n <= $kmax} { incr n}  {
 		    if {$nodeid($i,$j,$k)<$nodeid($l,$m,$n)} {
          set Cid $nodeid($i,$j,$k); set Tid $nodeid($l,$m,$n);
  } else {
          set Tid $nodeid($i,$j,$k);set Cid $nodeid($l,$m,$n);
  }
 		        set contact($Cid,$Tid) 0

		        if {$contact($Cid,$Tid)==0} {

	set idist2($nodeid($i,$j,$k),$nodeid($l,$m,$n)) [expr 	pow($nodecoordx($nodeid($i,$j,$k))-$nodecoordx($nodeid($l,$m,$n)),2)+pow($nodecoordy($nodeid($i,$j,$k))-$nodecoordy($nodeid($l,$m,$n)),2)+pow($nodecoordz($nodeid($i,$j,$k))-$nodecoordz($nodeid($l,$m,$n)),2)]

	set idist($nodeid($i,$j,$k),$nodeid($l,$m,$n)) [expr 	pow($idist2($nodeid($i,$j,$k),$nodeid($l,$m,$n)),0.5)]

	if {$idist($nodeid($i,$j,$k),$nodeid($l,$m,$n))>0.0 && 	$idist($nodeid($i,$j,$k),$nodeid($l,$m,$n))<= [expr $horizon-	$radij]} {
	set trussnum [expr $trussnum+1]		
	set family($trussnum,1) $nodeid($i,$j,$k);
	set family($trussnum,2) $nodeid($l,$m,$n)
	set fac($trussnum) 1.0
		
      if {$tag($nodeid($i,$j,$k))==1 &&$tag($nodeid($l,$m,$n))==1} 	{

	uniaxialMaterial Concrete02 $trussnum $cfpc $cepsc0 $cfpcu 	$cepsU $clambda $cft $cEts
	element  truss  $trussnum   $nodeid($i,$j,$k)   $nodeid($l,$m,$n)         	$A      $trussnum   
	set family($trussnum,3) 1 	}
	set nodelink([expr int($nodeid($i,$j,$k))],[expr 	int($nodeid($l,$m,$n))])	$trussnum 
	set cellnum($nodeid($i,$j,$k)) [expr 	$cellnum($nodeid($i,$j,$k))+1]	
	set cell($nodeid($i,$j,$k),$cellnum($nodeid($i,$j,$k)))	$nodeid($l,$m,$n)
	if {$idist($nodeid($i,$j,$k),$nodeid($l,$m,$n))>[expr $horizon-	$radij] && $idist($nodeid($i,$j,$k),$nodeid($l,$m,$n))< [expr 	$horizon+$radij]} {
	set trussnum [expr $trussnum+1]
	set family($trussnum,1) $nodeid($i,$j,$k)
	set family($trussnum,2) $nodeid($l,$m,$n)
	set fac($trussnum)  [expr ($horizon+$radij-	$idist($nodeid($i,$j,$k),$nodeid($l,$m,$n)))/(2.0*$radij)]
	if {$tag($nodeid($i,$j,$k))==1 && $tag($nodeid($l,$m,$n))==1} 	{
	uniaxialMaterial Concrete02 $trussnum [expr 	$cfpc*$fac($trussnum)] $cepsc0 [expr $cfpcu*$fac($trussnum)] 	$cepsU $clambda [expr $cft*$fac($trussnum)]  [expr 	$cEts*$fac($trussnum)] 
	element truss $trussnum $nodeid($i,$j,$k)	$nodeid($l,$m,$n)         $A        $trussnum   	
	}
	set family($trussnum,3) 1; 	set contact($Cid,$Tid) 1
	set nodelink([expr int($nodeid($i,$j,$k))],[expr 	int($nodeid($l,$m,$n))])	$trussnum 
	set cellnum($nodeid($i,$j,$k)) [expr 	$cellnum($nodeid($i,$j,$k))+1]	
	set cell($nodeid($i,$j,$k),$cellnum($nodeid($i,$j,$k)))	   	$nodeid($l,$m,$n)  }}}}}}}}}
