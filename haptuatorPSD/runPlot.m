clear
close all;
%% Without PSD
run('/home/haquang/workspace/rtai/haptuator/haptuatorPSD/acc1.m')
x1 = data(:,1);
r1 = data(:,2);
%x1 = shift(x1,8);
%% With PSD
run('/home/haquang/workspace/rtai/haptuator/haptuatorPSD/acc.m')
x = data(:,1);
r = data(:,2);
x = shift(x,8);



figure(1);
subplot(2,1,1);
plot(x,'k');
hold on;
plot(r,'r')

subplot(2,1,2);
plot(x1,'k');
hold on;
plot(r1,'r')