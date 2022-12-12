clear all
close all
clc

figure(1)

a = load('ddm2E.out');
load node2.out
load nodeE1.out
load nodeE2.out
load nodeE3.out


param = 2e8;
plot(a(:,1),a(:,2),'k','linewidth',2)
hold on
plot(a(:,1),(nodeE1(:,2)-node2(:,2))/(0.00001*param),'b','linewidth',2)
plot(a(:,1),(nodeE2(:,2)-node2(:,2))/(0.000001*param),'g','linewidth',2)
plot(a(:,1),(nodeE3(:,2)-node2(:,2))/(0.0000001*param),'m','linewidth',2)
legend('DDM','FFD 1e-5','FFD 1e-6','FFD 1e-7')
xlabel('Time [s]','Fontsize',14)
ylabel('\partialu/\partial\theta [m^3/KN]','Fontsize',14)
set(gca,'Fontsize',14)

figure(2)
load force.out
load deformation.out
plot(deformation(:,2)/10,force(:,2)/0.1,'r','linewidth',2)
xlabel('Strain','Fontsize',14)
ylabel('Stress [KPa]','Fontsize',14)
set(gca,'Fontsize',14)
