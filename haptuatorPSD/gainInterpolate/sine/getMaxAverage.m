function [max_r,max_x] = getMaxAverage(r,x)

    N = 100;
    M = 0;
    max_r = 0;
    max_x = 0;
    for i = 0:49
       max_r = max_r + max(r(N*i+1:N*(i+1)));
       max_x = max_x + max(x(N*i+1:N*(i+1)));
       M = M +1;
    end
    max_r = max_r/M;
    max_x = max_x/M;

end

