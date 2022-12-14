 ##############################################
 #        ------------------------------
 #        |units: m、kg、sec、N、Pa    |
 #        ------------------------------
 ##############################################

global NodeID   


if { [file exists DataB] == 0 } {file mkdir DataB}

set LenE 1.5;    # 单元长度1.0（m）
set H    0.05;        # 轨道桥梁间连接单元长度

# 定义轨道节点
for {set i 1} {$i<=81} {incr i 1} {
	node [expr $i]  [expr ($i-1)*$LenE]   $H
	lappend NodeID  [expr $i]
}

# 定义基础桥梁节点
for {set i 101} {$i<=181} {incr i 1} {
	node [expr $i]  [expr ($i-101)*$LenE]   0.0;
}

for {set i 101} {$i<=130} {incr i 1} {
	fix [expr $i]   1 1 1
}

for {set i 152} {$i<=181} {incr i 1} {
fix [expr $i]   1 1 1
}

fix 131  1 1 0;
fix 151  0 1 0;
fix 1    1 0 1;     # 轨道两端伸缩和点头约束
fix 81   1 0 1;

# 定义桥梁单元材料及截面
set Ec  2.943e9;    
set AB  7.94;
set IzB 2.88;
section       Elastic           2          $Ec       $AB        $IzB;

# 定义轨道单元材料
# define the material parameters
set AR       77.45e-4;       
set E_rail   2.06e11;       
set Iz_rail  [expr 2*2.037e-5];     

# 定义轨道与桥梁（路基）间参数
set Krb [expr 2*6.58e7];    
set Crb [expr 2*3.21e4];   
set Arb  $H;
set Erb [expr $Krb]

uniaxialMaterial Elastic  101  $Erb
uniaxialMaterial Viscous  201  $Crb  1

geomTransf Linear 2;

# 定义桥梁单元
for {set i 131} {$i<151} {incr i 1} {
element   dispBeamColumn      $i     $i   [expr $i+1]    5      2    2    -mass  1.2e4 -cMass;
}

# 定义轨道梁单元
for {set i 1} {$i<81} {incr i 1} {
# element elasticBeamColumn $eleTag $iNode $jNode $A $E $Iz $transfTag <-mass $massDens> <-cMass>
element elasticBeamColumn     $i    $i [expr ($i+1) ]   $AR   $E_rail  $Iz_rail   2   -mass   [expr 2*51.5]  -cMass
}

# 定义桥梁和轨道件弹簧阻尼单元
for {set i 1} {$i<=81} {incr i 1} {
element truss [expr $i+6000]  [expr $i]  [expr $i+100]  $Arb   101
element truss [expr $i+7000]  [expr $i]  [expr $i+100]  $Arb   201
}


# # puts "Damp"
# set xDamp 0.025;    #————设置阻尼比为 0.05
# set nEigenI 1;      # ————主振型 1 为第 1 振型
# set nEigenJ 2;      # ————主振型 2 为第 2 振型
# set lambdaN [eigen  $nEigenJ];         # ————求解两阶振型即可
# set lambdaI [lindex $lambdaN [expr $nEigenI-1]];     # ————提取第 1 阶特征值
# set lambdaJ [lindex $lambdaN [expr $nEigenJ-1]];     # ————提取第 2 阶特征值
# set omegaI  [expr pow($lambdaI,0.5)];     #  ————从特征值求圆频率
# set omegaJ  [expr pow($lambdaJ,0.5)];     # ————从特征值求圆频率

# # set omegaI 6.32;     #  ————从特征值求圆频率
# # set omegaJ 18.57;     # ————从特征值求圆频率
# set alphaM    [expr $xDamp*(2*$omegaI*$omegaJ)/($omegaI+$omegaJ)];        #————alphaM 为 0    a ，即质量相关系数
# set betaKcurr [expr 2.*$xDamp/($omegaI+$omegaJ)];    #————betaKcurr 为 1  a ，即刚度相关系数
# # ————定义瑞利阻尼，只需要填写 0  a ， 1   a ，其它值为 0 ,rayleigh $alphaM $betaK $betaKinit $betaKcomm
# rayleigh $alphaM $betaKcurr 0 0

 # 轨  1-------------26-----------41------------56----------------81
 # 桥 101-----------126----------141-----------156---------------181
recorder Node -file DataB/RailDisp.txt    -time -node  16  41  66   -precision 16  -dof 2  disp;
recorder Node -file DataB/RailAccel.txt   -time -node  16  41  66   -precision 16  -dof 2  accel;
recorder Node -file DataB/BridgeDisp.txt  -time -node  141   -precision 16  -dof 2  disp;
recorder Node -file DataB/BridgeAccel.txt -time -node  141   -precision 16  -dof 2  accel;












