close all;
clear;
%% Model
%run('simulation.m')
%t = data(:,1);
%y = data(:,2);

%% Actual
run('acc.m')
acc_x = data(:,1);
acc_y = data(:,2);
acc_z = data(:,3);
acc_sim = data(:,4);
freq = data(:,5);
%% plot
figure(1)
%subplot(2,1,1);
plot(acc_sim);
hold on;
%ylim([-2,2]);
%subplot(2,1,2);
plot(acc_x,'--');
%hold on;
%plot(t,acc_x,'r');
%hold on;
%plot(t,y,'k');

