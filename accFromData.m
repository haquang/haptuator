function [ acc, vel] = accFromData( input_args, Nsamples )
%FORCEFROMDATA Summary of this function goes here
%   Detailed explanation goes here
epsilon = 0.02;

% Read data from file
data = dlmread(input_args,'\t');
acc = -data(:,2);
vel = data(:,1);
% determine the contact point
 curacc = acc(1);
 for i=1:length(acc)
    if (abs((acc(i) - curacc)) > epsilon)
         break;
        curacc = acc(i);
    end    
 end
 contactPoint = i
 window = 5;
 vel = 0;
 for j = i-window:i
    vel = vel + data(j,1)/window; 
 end
 
 acc = acc(contactPoint:contactPoint+Nsamples);
% pos = data(contactPoint:contactPoint+Nsamples,8);
% vel = NaN(Nsamples,1);
% curPos = pos(1);
% for i=1:length(pos)
%    vel(i) = pos(i) - curPos;
%    curPos = pos(i);
% end

end

