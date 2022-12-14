global R0 la lb Ew miuWheel;

set R0 0.43;
set zFrame 0.46;
set zCar 1.849;
set la 2.1;
set lb 11.1;

set Ew 2.06e11; 
set miuWheel 0.296;


set Mw 933; 	set Iwx 461.4; 	set Iwy 61.6;  	set Iwz 461.4;
set Ma 176;		set Iax 144.5;	set Iay 2.2; 	set Iaz 144.5;
set Mt 1982; 	set Itx 1398.5; set Ity 2667; 	set Itz 2667;
set Mc 11160; 	set Icx 14952.9; 	set Icy 225365.4; 	set Icz 224994.9;


set Mwheel  407.629373;
set Mcenter 117.741254;


set y0 0.7523;
set y1 0.90;
node 1    0              	 -$y0  $R0;

node 1001 0              	  $y0  $R0;
node 51   0              	 -$y1  $R0;
node 1051 0              	  $y1  $R0;
node 2001 0              	 0    $R0  -mass $Mw $Mw $Mw  $Iwx $Iwy $Iwz;

fix 2001 	0 0 0  0 1 0;


set ARigid 1.0e3; 	set GRigid 7.9E10; 	set ERigid 2.0e11;
set IxRigid 0.1; 	set IyRigid 0.1; 	set IzRigid 0.1;

set transf1 1;
geomTransf Linear $transf1 0 0 1;


element elasticBeamColumn 2001    1    	2001 $ARigid $ERigid $GRigid $IxRigid $IyRigid $IzRigid $transf1;
element elasticBeamColumn 2101    2001 	1001 $ARigid $ERigid $GRigid $IxRigid $IyRigid $IzRigid $transf1;
element elasticBeamColumn 2051    1    	51   $ARigid $ERigid $GRigid $IxRigid $IyRigid $IzRigid $transf1;
element elasticBeamColumn 2151    1001 	1051 $ARigid $ERigid $GRigid $IxRigid $IyRigid $IzRigid $transf1;


pattern Plain 2 { Series -time  { 0.0   0.01   10000.0 }
                        -values { 0.0   1.0   1.0 } }  {
	set g 9.801;
	load 2001  0 0 [expr -$Mw*$g]   0 0 0;
}

pattern Plain 3 { Series          -time  { 0.0  0.01  0.02      	0.200001  800.0 }  
                                 -values { 0.0	   0  2.48e-4  	0.0       0  }   }  {
	
	load 2001  0  [expr $Mw*$g]  0   0 0 0;
}



recorder Node -file ex1/W1DispL.txt		-time 	-node 1 -precision 16  -dof 1 2 3 4 6  disp;
recorder Node -file ex1/W1VelL.txt    	-time 	-node 1 -precision 16  -dof 1 2 3 4 6  vel; 
recorder Node -file ex1/W1AccelL.txt 	-time 	-node 1 -precision 16  -dof 1 2 3 4 6 accel;


recorder Node -file ex1/W1001DispR.txt  	-time 	-node 1001 -precision 16  -dof 1 2 3 4 6  disp;
recorder Node -file ex1/W1001VelR.txt   	-time 	-node 1001 -precision 16  -dof 1 2 3 4 6  vel; 
recorder Node -file ex1/W1001AccelR.txt 	-time 	-node 1001 -precision 16  -dof 1 2 3 4 6 accel;


recorder Node -file ex1/W2001Disp.txt  	-time 	-node 2001 -precision 16  -dof 1 2 3 4 6  disp;
recorder Node -file ex1/W2001Vel.txt   	-time 	-node 2001 -precision 16  -dof 1 2 3 4 6  vel; 
recorder Node -file ex1/W2001Accel.txt 	-time 	-node 2001 -precision 16  -dof 1 2 3 4 6 accel;

