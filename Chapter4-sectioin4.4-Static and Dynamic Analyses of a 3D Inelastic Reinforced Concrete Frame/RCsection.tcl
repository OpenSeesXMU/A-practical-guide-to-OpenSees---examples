# ����һ�������������ɸֽ����������,���������ܾ��ȷֲ�һ�Ÿֽ�,����ͼ
# 
#                       y
#                       /\
#                       |
#                       |    
#          4  ---------------------  3
#             |\                 /|
#             | \---------------/ |
#             | |               | |
#             | |               | |
# z /---------| |               | |  h
#   \         | |               | |
#             | |               | |
#             | /---------------\ |
#             |/                 \|
#          1  ---------------------  2
#                       b
#
# ��ʽ��Ԫ
#    id -      �������ɵĽ������к�
#    h -       ����ȫ��
#    b -       ����ȫ��
#    cover -   ��������
#    coreID -  �������������ֿ���Ϻ�
#    coverID - ������������ֿ���Ϻ�
#    steelID - �ֽ���Ϻ�
#    numBars - �����κ�һ�ߵĸֽ����
#    barArea - �ֽ���������� 
#    nfCoreY - ��������Y���򻮷ֵ���ά��Ԫ��
#    nfCoreZ - ��������Z���򻮷ֵ���ά��Ԫ��
#    nfCoverY -��������Y���򻮷ֵ���ά��Ԫ��
#    nfCoverZ -��������Z���򻮷ֵ���ά��Ԫ��
#
# ע:
#    ������������ĺ���ǳ���;
#    �ڽ����κ�һ���ϵĸֽ�������ͬ��;
#    ���иֽ���ͺ�����ͬ��;
#    �������ڶ̷������ά��Ԫ��������Ϊ1;
# 
proc RCsection {id h b cover coreID coverID steelID numBars barArea nfCoreY nfCoreZ nfCoverY nfCoverZ} {

   # --------Y��������Ľ���Z�ᵽ����������ߵľ���------------
   
   set coverY [expr $h/2.0]

   # --------Z��������Ľ���Y�ᵽ����������ߵľ���-------------
   
   set coverZ [expr $b/2.0]

   # --------Y�Ḻ����Ľ���Z�ᵽ����������ߵľ���-------------
  
   set ncoverY [expr -$coverY]

   # --------Z�Ḻ����Ľ���Y�ᵽ����������ߵľ���-------------
   
   set ncoverZ [expr -$coverZ]

   # --------��Ӧ���ߵ�����������������ߵľ���-----------------
   
   set coreY  [expr $coverY-$cover]
   set coreZ  [expr $coverZ-$cover]
   set ncoreY [expr -$coreY]
   set ncoreZ [expr -$coreZ]

   # --------������ά����---------------------------------------------------------------------------------
   section fiberSec $id {

	#----------������������--------------------------------------------
        
        #--�ı��ε�Ԫ ���ֵ�Ԫ����� Z���򻮷ָ��� Y���򻮷ָ���   1��X��Y��������    2��X��Y��������    3��X��Y��������    4��X��Y�������� 
	patch quadr     $coreID      $nfCoreZ     $nfCoreY     $ncoreY $coreZ    $ncoreY $ncoreZ    $coreY $ncoreZ    $coreY $coreZ
      
	#----------�����������--------------------------------------------
	        
        #--�ı��ε�Ԫ ���ֵ�Ԫ����� Z���򻮷ָ��� Y���򻮷ָ���   1��X��Y��������    2��X��Y��������    3��X��Y��������    4��X��Y�������� 
	patch quadr     $coverID        1         $nfCoverY    $ncoverY $coverZ  $ncoreY  $coreZ    $coreY  $coreZ    $coverY $coverZ
	patch quadr     $coverID        1         $nfCoverY    $ncoreY  $ncoreZ  $ncoverY $ncoverZ  $coverY $ncoverZ  $coreY  $ncoreZ
	patch quadr     $coverID    $nfCoverZ         1        $ncoverY $coverZ  $ncoverY $ncoverZ  $ncoreY $ncoreZ   $ncoreY $coreZ
	patch quadr     $coverID    $nfCoverZ         1        $coreY   $coreZ   $coreY   $ncoreZ   $coverY $ncoverZ  $coverY $coverZ
 
	#----------ƽ����Z����ֽ�------------------------------------------
       
        # ---ֱ�ŵ�Ԫ ���ֵ�Ԫ���Ϻ� �ֽ����       �ֽ����       ���X��������       ���Y��������      �յ�X��������       �յ�Y�������� 
	layer straight  $steelID    $numBars      $barArea        $ncoreY             $coreZ             $ncoreY           $ncoreZ
	layer straight  $steelID    $numBars      $barArea        $coreY              $coreZ             $coreY            $ncoreZ

	# ---------����Y�����ʣ��ռ�---------------------------------------
	
	set spacingY [expr ($coreY-$ncoreY)/($numBars-1)]

	#----------��ֹ�ظ�����ֽ����--------------------------------------
	
	set numBars [expr $numBars-2]

	#----------ƽ����Y����ֽ�-------------------------------------------
	
	layer straight  $steelID    $numBars     $barArea  [expr $coreY-$spacingY]   $coreZ    [expr $ncoreY+$spacingY]    $coreZ
	layer straight  $steelID    $numBars     $barArea  [expr $coreY-$spacingY]   $ncoreZ   [expr $ncoreY+$spacingY]    $ncoreZ
  
   }

}