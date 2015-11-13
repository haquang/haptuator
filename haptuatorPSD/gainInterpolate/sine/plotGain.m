%close all;
clear;
f = NaN(40,1);
K = NaN(40,1);
for i = 1:40
    if (i < 10)
        input = strcat('/home/haquang/workspace/rtai/haptuator/haptuatorPSD/gainInterpolate/sine/acc0',num2str(50+i*5) , '.m')
    else 
        input = strcat('/home/haquang/workspace/rtai/haptuator/haptuatorPSD/gainInterpolate/sine/acc',num2str(50+i*5) , '.m')
    end
	run(input);

    x(:,i+1) = data(:,1);
    r(:,i+1) = data(:,2);
    f(i+1) = 50+i*5;
    [max_r,max_x] =  getMaxAverage(r(:,i+1),x(:,i+1));
    K(i+1) = max_x/max_r;

end
dlmwrite('gain.txt',f','delimiter','\t','precision',5,'-append');
dlmwrite('gain.txt',K','delimiter','\t','precision',5,'-append');
% dlmwrite('gain.txt',K,'delimiter','\t','precision',5,'-append');

figure(1);
plot(f,K,'r-*');