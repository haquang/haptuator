function [ acc,vel ] = accFromDragData2( input_args , Nsamples )
%ACCFROMDRAGDATA Summary of this function goes here
%   Detailed explanation goes here
%FORCEFROMDATA Summary of this function goes here
%   Detailed explanation goes here
epsilon = 0.1;

% Read data from file
data = dlmread(input_args,'\t');
a = -data(:,2);
v = data(:,1);
% determine the contact point
 curacc = a(1);
 count=0;
 j = 0;
 for i=1:length(a)
    if (abs((a(i) - curacc)) > epsilon)
        count = count +1
        i-j
    else
        curacc = a(i);
        j = i;
    end    
 end
 acc = a;
 vel = v;
end

