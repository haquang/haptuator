close all;
clear;

input = strcat('Test_Acc_N',num2str(1) , '.txt');
data = dlmread(input,'\t');
vel = abs(data(:,1));
acc = -data(:,2);

%[acc,vel] =  accFromDragData(input,200);

%Plot
figure(1)
subplot(2,1,1)
plot(vel)
subplot(2,1,2)
plot(acc)