function [ X ] = sdft( X1,old, new, size,f )
    X = NaN(size,1);
    
    for i = 0:size-1
 %       sum = 0;
          X(i+1) = (X1(i+1) - old + new)* exp(-j*2*pi*f(i+1));
%        for k = 0:size
% 
%            sum = sum + x(k+1) * exp(-j*2*pi*i*k/size);
%        end
%            X(i+1) = sum;
    end

end

