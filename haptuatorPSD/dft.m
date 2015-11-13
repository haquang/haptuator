function [ X ] = dft( x, size,f,Fs )
    X = NaN(size,1);
    
    for i = 0:size-1
        sum = 0;
        for k = 0:size
            sum = sum + x(k+1) * exp(-j*2*pi*k*i/size);
        end
            f(i+1)
            sum
            X(i+1) = sum;
    end

end

