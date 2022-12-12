wipe ; 
model basic -ndm 2 -ndf 2
 
node   1	0	0
node   2	0.5	0
node   3	1	0
node   4	1.5	0
node   5	2	0
node   6	2.5	0
node   7	3	0
node   8	3.5	0
node   9	4	0
node   10	4.5	0
node   11	5	0
node   12	5.5	0
node   13	6	0
node   14	6.5	0
node   15	7	0
node   16	7.5	0
node   17	8	0
node   18	8.5	0
node   19	9	0
node   20	9.5	0
node   21	0	0.5
node   22	0.5	0.5
node   23	1	0.5
node   24	1.5	0.5
node   25	2	0.5
node   26	2.5	0.5
node   27	3	0.5
node   28	3.5	0.5
node   29	4	0.5
node   30	4.5	0.5
node   31	5	0.5
node   32	5.5	0.5
node   33	6	0.5
node   34	6.5	0.5
node   35	7	0.5
node   36	7.5	0.5
node   37	8	0.5
node   38	8.5	0.5
node   39	9	0.5
node   40	9.5	0.5
 
fix 1 1 1 
fix 21 1 1
 
uniaxialMaterial  Elastic 1 20    
 
element corotTruss2D 1	1	2      1   1   
element corotTruss2D 2	2	3      1   1   
element corotTruss2D 3	3	4      1   1   
element corotTruss2D 4	4	5      1   1   
element corotTruss2D 5	5	6      1   1   
element corotTruss2D 6	6	7      1   1   
element corotTruss2D 7	7	8      1   1   
element corotTruss2D 8	8	9      1   1   
element corotTruss2D 9	9	10     1   1   
element corotTruss2D 10	10	11     1   1   
element corotTruss2D 11	11	12     1   1   
element corotTruss2D 12	12	13     1   1   
element corotTruss2D 13	13	14     1   1   
element corotTruss2D 14	14	15     1   1   
element corotTruss2D 15	15	16     1   1   
element corotTruss2D 16	16	17     1   1   
element corotTruss2D 17	17	18     1   1   
element corotTruss2D 18	18	19     1   1   
element corotTruss2D 19	19	20     1   1   
element corotTruss2D 20	21	22     1   1   
element corotTruss2D 21	22	23     1   1   
element corotTruss2D 22	23	24     1   1   
element corotTruss2D 23	24	25     1   1   
element corotTruss2D 24	25	26     1   1   
element corotTruss2D 25	26	27     1   1   
element corotTruss2D 26	27	28     1   1   
element corotTruss2D 27	28	29     1   1   
element corotTruss2D 28	29	30     1   1   
element corotTruss2D 29	30	31     1   1   
element corotTruss2D 30	31	32     1   1   
element corotTruss2D 31	32	33     1   1   
element corotTruss2D 32	33	34     1   1   
element corotTruss2D 33	34	35     1   1   
element corotTruss2D 34	35	36     1   1   
element corotTruss2D 35	36	37     1   1   
element corotTruss2D 36	37	38     1   1   
element corotTruss2D 37	38	39     1   1   
element corotTruss2D 38	39	40     1   1   
element corotTruss2D 39	1	21     1   1   
element corotTruss2D 40	2	22     1   1   
element corotTruss2D 41	3	23     1   1   
element corotTruss2D 42	4	24     1   1   
element corotTruss2D 43	5	25     1   1   
element corotTruss2D 44	6	26     1   1   
element corotTruss2D 45	7	27     1   1   
element corotTruss2D 46	8	28     1   1   
element corotTruss2D 47	9	29     1   1   
element corotTruss2D 48	10	30     1   1   
element corotTruss2D 49	11	31     1   1   
element corotTruss2D 50	12	32     1   1   
element corotTruss2D 51	13	33     1   1   
element corotTruss2D 52	14	34     1   1   
element corotTruss2D 53	15	35     1   1   
element corotTruss2D 54	16	36     1   1   
element corotTruss2D 55	17	37     1   1   
element corotTruss2D 56	18	38     1   1   
element corotTruss2D 57	19	39     1   1   
element corotTruss2D 58	20	40     1   1   
element corotTruss2D 59	1	22     1   1   
element corotTruss2D 60	2	23     1   1   
element corotTruss2D 61	3	24     1   1   
element corotTruss2D 62	4	25     1   1   
element corotTruss2D 63	5	26     1   1   
element corotTruss2D 64	6	27     1   1   
element corotTruss2D 65	7	28     1   1   
element corotTruss2D 66	8	29     1   1   
element corotTruss2D 67	9	30     1   1   
element corotTruss2D 68	10	31     1   1   
element corotTruss2D 69	11	32     1   1   
element corotTruss2D 70	12	33     1   1   
element corotTruss2D 71	13	34     1   1   
element corotTruss2D 72	14	35     1   1   
element corotTruss2D 73	15	36     1   1   
element corotTruss2D 74	16	37     1   1   
element corotTruss2D 75	17	38     1   1   
element corotTruss2D 76	18	39     1   1   
element corotTruss2D 77	19	40     1   1   
                                 
 
 
 
 
 recorder Node -file displacement.out -time -nodeRange  1 40  -dof 1 2 disp
 recorder Node -file force.out -time -nodeRange 1 40  -dof 1 2 reaction


pattern Plain 100 Linear {
	sp     40     2     -6.0 
} 
 
system BandGeneral
numberer RCM
constraints Transformation
integrator LoadControl 0.001
test NormDispIncr  1.0e-8 200 2
algorithm Newton
analysis Static
analyze 1000

 