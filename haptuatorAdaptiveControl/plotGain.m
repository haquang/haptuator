close all;
clear;
f = NaN(35,1);
K = NaN(35,1);
for i = 0:35
    if (i < 10)
        input = strcat('/home/haquang/workspace/rtai/haptuator/haptuatorAdaptiveControl/acc0',num2str(50+i*5) , '.m')
    else 
        input = strcat('/home/haquang/workspace/rtai/haptuator/haptuatorAdaptiveControl/acc',num2str(50+i*5) , '.m')
    end
	run(input);

    x(:,i+1) = data(:,1);
    r(:,i+1) = data(:,2);
    f(i+1) = 50+i*5;
    K(i+1) = max(x(:,i+1))/max(r(:,i+1));

end

figure(1);
plot(f,K,'r-*');