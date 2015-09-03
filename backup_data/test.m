close all;
clear;

input = strcat('Test_Acc_N',num2str(1) , '.txt');
data = dlmread(input,'\t');
acc = -data(:,2);
vel = data(:,1);

%Plot
figure(1)
subplot(2,1,1)
plot(vel)
subplot(2,1,2)
plot(acc)