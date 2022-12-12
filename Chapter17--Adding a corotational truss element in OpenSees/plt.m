clear all
close all
clc
load node.tcl
load element.tcl
load displacement.out
step=1000;
for i=1:length(node(:,1))
cCoor(i,1)=node(i,2)+displacement(step,2*i);
cCoor(i,2)=node(i,3)+displacement(step,2*i+1);
end
figure (1)
axis equal 
grid on
xlim([-1,8])
ylim([-6,1])
set(gca,'Fontsize',18) 
xlabel('X Coordinate [m]')
ylabel('Y Coordinate [m]')
hold on
for i=1:length(element(:,1))
plot([cCoor(element(i,2),1),cCoor(element(i,3),1)],[cCoor(element(i,2),2),cCoor(element(i,3),2)],'lineWidth',3,'Color','b')
end
