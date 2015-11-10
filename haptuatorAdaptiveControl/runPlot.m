close all;
clear
run('/home/haquang/workspace/rtai/haptuator/haptuatorAdaptiveControl/acc.m')
x = data(:,1);
r = data(:,2);
    

figure(1);
plot(x);
hold on;
plot(r,'r')