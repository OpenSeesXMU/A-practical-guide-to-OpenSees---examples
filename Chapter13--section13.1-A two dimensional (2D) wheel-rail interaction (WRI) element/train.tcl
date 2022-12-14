#  | 单位: m、kg、sec、N、Pa            |
#  | 坐标：X沿线路走向为正，Z向上为正 。|
#
if { [file exists DataT] == 0 } {file mkdir DataT}
# definition parameter
set g  9.801;


# kg 考虑车体和转向架两端为无质量和惯性节点
set Mt     5.2e4;
set JMt    2.31e6;
set Mb     3.2e3;
set JMb    3.12e3;
set MWheel 1.4e3;

# 轨道桥梁间连接单元长度-----车辆局部坐标系到总体坐标系的转换；
set ytranslation    [expr 0.05+$pRWheel];        
set xtranslation    [expr 10.25+$pInitLocation];

node   2001    [expr -10.25+$xtranslation]   [expr 0+$ytranslation]
node   2002    [expr -7.75+$xtranslation]   [expr 0+$ytranslation]
node   2003    [expr 7.75+$xtranslation]     [expr 0+$ytranslation]
node   2004    [expr 10.25+$xtranslation]    [expr 0+$ytranslation]
node   2005    [expr -10.25+$xtranslation]   [expr 0.22+$ytranslation]
node   2006    [expr -9+$xtranslation]       [expr 0.22+$ytranslation]
node   2007    [expr -7.75+$xtranslation]     [expr 0.22+$ytranslation]
node   2008    [expr 7.75+$xtranslation]     [expr 0.22+$ytranslation]
node   2009    [expr 9+$xtranslation]        [expr 0.22+$ytranslation]
node   2010    [expr 10.25+$xtranslation]    [expr 0.22+$ytranslation]
node   2011    [expr -9+$xtranslation]       [expr 0.52+$ytranslation]
node   2012    [expr 0+$xtranslation]        [expr 0.52+$ytranslation]
node   2013    [expr 9+$xtranslation]        [expr 0.52+$ytranslation]


mass 2001 $MWheel $MWheel 0.0
mass 2002 $MWheel $MWheel 0.0
mass 2003 $MWheel $MWheel 0.0
mass 2004 $MWheel $MWheel 0.0
mass 2006 $Mb $Mb $JMb
mass 2009 $Mb $Mb $JMb
mass 2012 $Mt $Mt $JMt

fix 2001 1 0 1
fix 2002 1 0 1
fix 2003 1 0 1
fix 2004 1 0 1

fix  2005 1 0 0
fix  2006 1 0 0
fix  2007 1 0 0
fix  2008 1 0 0
fix  2009 1 0 0
fix  2010 1 0 0
fix  2011 1 0 0
fix  2012 1 0 0
fix  2013 1 0 0

equalDOF 2012 2011  3
equalDOF 2012 2013  3
equalDOF 2006 2005  3
equalDOF 2006 2007  3
equalDOF 2009 2008  3
equalDOF 2009 2010  3

set E  2.06e11;     # steel elasticity modulus  unit: Pa
set po 0.3 ;       # steel Poisson's ratio


set Kv1 1.87e6;  # 竖向刚度
set Cv1 5.0e5
set Kv2 1.72e6
set Cv2 1.96e5

set Av1 0.2200;
set Av2 0.3000;
set Ev1 [expr $Kv1]
set Ev2 [expr $Kv2]


# 有待确定？？？
set Izb [expr 4.05e7];   #转动惯性无限大
set Ab  [expr 0.54]
set At  [expr 8.4]
set Izt [expr 3.0e7]

uniaxialMaterial Elastic  801  $Ev1
uniaxialMaterial Elastic  802  $Ev2
# uniaxialMaterial Viscous $matTag $C $alpha
uniaxialMaterial Viscous 701 $Cv1  1
uniaxialMaterial Viscous 702 $Cv2  1

# element elasticBeamColumn $eleTag $iNode $jNode $A $E $Iz $transfTag <-mass $massDens> <-cMass>

# 左转向架
element elasticBeamColumn 2001 2005 2006 $Ab $E $Izb 2
element elasticBeamColumn 2002 2006 2007 $Ab $E $Izb 2

# 右转向架
element elasticBeamColumn 2003 2008 2009 $Ab $E $Izb 2
element elasticBeamColumn 2004 2009 2010 $Ab $E $Izb 2

# 车体
element elasticBeamColumn 2005 2011 2012 $At $E $Izt 2
element elasticBeamColumn 2006 2012 2013 $At $E $Izt 2

# 一系弹簧
element truss 2007  2001  2005  $Av1   801
element truss 2008  2002  2007  $Av1   801
element truss 2009  2003  2008  $Av1   801
element truss 2010  2004  2010  $Av1   801

# 二系弹簧
element truss 2011  2006  2011  $Av2   802
element truss 2012  2009  2013  $Av2   802

# 一系阻尼
element truss 2013  2001  2005  $Av1   701
element truss 2014  2002  2007  $Av1   701
element truss 2015  2003  2008  $Av1   701
element truss 2016  2004  2010  $Av1   701

# 二系阻尼
element truss 2017  2006  2011  $Av2   702
element truss 2018  2009  2013  $Av2   702      
        
pattern Plain 1 Linear {

		load 2001  0.0 [expr -$MWheel*$g] 0.0;
		load 2002  0.0 [expr -$MWheel*$g] 0.0;
		load 2003  0.0 [expr -$MWheel*$g] 0.0;
		load 2004  0.0 [expr -$MWheel*$g] 0.0;

		load 2006  0.0 [expr -$Mb*$g] 0.0;
		load 2009  0.0 [expr -$Mb*$g] 0.0;

		load 2012  0.0 [expr -$Mt*$g] 0.0;

}
recorder Node -file DataT/WnodeDisp.txt       -time -node 2001 2002 2003 2004 -precision 16  -dof  2  disp;
recorder Node -file DataT/WnodeVel.txt        -time -node 2001 2002 2003 2004 -precision 16  -dof  2  vel;
recorder Node -file DataT/WnodeAccel.txt      -time -node 2001 2002 2003 2004 -precision 16  -dof 2 accel;
recorder Element -file DataT/SpringEleF.txt   -time -ele  2007 2008 2009 2010 -precision 16   localForce;

recorder Node -file DataT/bodyDisp.txt   -time -node 2011 2012 2013 -precision 16  -dof  2  disp;
recorder Node -file DataT/bodyVel.txt    -time -node 2011 2012 2013 -precision 16  -dof  2  vel;
recorder Node -file DataT/bodyAccel.txt  -time -node 2011 2012 2013 -precision 16  -dof  2  accel;
recorder Node -file DataT/bogieDisp.txt  -time -node 2005 2006 2007 2008 2009 2010 -precision 16  -dof  2  disp;
recorder Node -file DataT/bogieVel.txt   -time -node 2005 2006 2007 2008 2009 2010 -precision 16  -dof  2  vel;
recorder Node -file DataT/bogieAccel.txt -time -node 2005 2006 2007 2008 2009 2010 -precision 16  -dof  2  accel;

recorder Element -file DataT/bodyForce.txt  -time -ele 2005 2006 -precision 16  localForce;
recorder Element -file DataT/bogieForce.txt -time -ele 2001 2002 2003 2004 -precision 16  localForce;