function [ acc,vel ] = accFromDragData( input_args , Nsamples )
%ACCFROMDRAGDATA Summary of this function goes here
%   Detailed explanation goes here
%FORCEFROMDATA Summary of this function goes here
%   Detailed explanation goes here
epsilon = 0.1;

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
 contactPoint = i;
 vel = 0;
 for i = contactPoint:contactPoint+Nsamples
    vel = vel + data(i,1)/Nsamples; 
 end
 
 acc = acc(contactPoint:contactPoint+Nsamples);
end
