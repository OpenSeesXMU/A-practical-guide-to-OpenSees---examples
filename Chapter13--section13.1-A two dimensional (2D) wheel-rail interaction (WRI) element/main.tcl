wipe ; #============================

model basic -ndm 2 -ndf 3

set pInitLocation 10;
set pDeltT 0.001;
set pVel 27.78;
set pRWheel 0.4575; 
set pI [expr 2*2.037e-5];
set pE 2.06e11;
set pA 77.45e-4;
set transfTag 1;
geomTransf Linear $transfTag;

# ----------bridge and train-------------
source bridge.tcl;
source train.tcl;
# ----------bridge and train end -------------

# wheel rail elements
set pInitLocation1 [expr 0+$pInitLocation]
set pInitLocation2 [expr 2.5+$pInitLocation]
set pInitLocation3 [expr 17.5+$pInitLocation]
set pInitLocation4 [expr 20+$pInitLocation]



element WheelRail 3001 $pDeltT $pVel $pInitLocation1 2001 $pRWheel $pI $pE $pA $transfTag 10 -NodeList $NodeID rail_Irreg.txt;
element WheelRail 3002 $pDeltT $pVel $pInitLocation2 2002 $pRWheel $pI $pE $pA $transfTag 10 -NodeList $NodeID rail_Irreg.txt;
element WheelRail 3003 $pDeltT $pVel $pInitLocation3 2003 $pRWheel $pI $pE $pA $transfTag 10 -NodeList $NodeID rail_Irreg.txt;
element WheelRail 3004 $pDeltT $pVel $pInitLocation4 2004 $pRWheel $pI $pE $pA $transfTag 10 -NodeList $NodeID rail_Irreg.txt;

constraints Plain;     				                      # 边界条件
numberer Plain;					                          # 编码
system BandGeneral;				                          # 存储和解方程
test NormDispIncr 1.0e-8 100 2;		                      # 收敛性检验
algorithm Newton;					                      # 采用NEWTON算法
integrator LoadControl 0.1;		                          # 荷载控制,定义时间步长
analysis Static;					                      # 静力分析
analyze 10;                                           # 运行10次
puts "finished!"
loadConst -time 0.0;				
wipeAnalysis;	
				                                                               
constraints Plain;     				                       
numberer Plain;					                       
system BandGeneral;	
		
test NormDispIncr 1.0e-8 100 2;
algorithm Newton;					
integrator Newmark 0.5 0.25 ;  
analysis Transient;
analyze 3000 $pDeltT;

puts "over!"
wipe;
