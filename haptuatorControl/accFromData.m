function [ vel,acc_x, acc_y,acc_z] = accFromData( input_args, Nsamples )
%FORCEFROMDATA Summary of this function goes here
%   Detailed explanation goes here
epsilon = 0.25;

% Read data from file
data = dlmread(input_args,'\t');
acc_x = -data(:,2);
acc_y = -data(:,3);
acc_z = -data(:,4);
% determine the contact point
 curacc_x = acc_x(1);
 curacc_y = acc_y(1);
 curacc_z = acc_z(1);
 for i=2:length(acc_x)
    if ((abs((acc_x(i) - curacc_x)) > epsilon) || (abs((acc_y(i) - curacc_y)) > epsilon) ||(abs((acc_z(i) - curacc_z)) > epsilon))
         break;
        curacc_x = acc_x(i);
        curacc_y = acc_y(i);
        curacc_z = acc_z(i);
    end    
 end
 contactPoint = i;
 window = 5;
 vel = 0;
 for j = i-window:i
    vel = vel + data(j,1)/window; 
 end
 
 acc_x = acc_x(contactPoint:contactPoint+Nsamples);
 acc_y = acc_y(contactPoint:contactPoint+Nsamples);
 acc_z = acc_z(contactPoint:contactPoint+Nsamples);
end

