close all;
clear;
N = 61;
Fs=1000;
M=200;
nfft = 2^nextpow2(M);
NSamples = 200;
t = (1/Fs)*(1:NSamples+1);
acc = NaN(NSamples+1,N);
vel = NaN(N,1);
a0 = NaN(N,1);
a = NaN(nfft/2,N);
b = NaN(nfft/2,N);
nfft = 2^nextpow2(M);

% read data from file
for i = 1:N
input = strcat('Test_Acc_N',num2str(i) , '.txt')
[acc(:,i),vel(i)] =  accFromData(input,NSamples);
end

for i = 1: N
y = acc(:,i);
%Do Fourier Transform
y_fft= fft(y,nfft);
f = Fs/2*linspace(0,1,nfft/2);
a0(i) = y_fft(1)/nfft;
an{i}(:,i) = 2*real(y_fft(2:nfft))/nfft;
a(:,i) = an{i}(1:length(an{i}(:,i))/2 +1,i);
bn{i}(:,i) = -2*imag(y_fft(2:nfft))/nfft;
b(:,i) = bn{i}(1:length(bn{i}(:,i))/2 +1,i);
mag{i}=sqrt(bn{i}(:,i).^2+an{i}(:,i).^2);
end

% Filter and reconstruct signal
y_filtered = NaN(NSamples/2,N);
t_r = t(1:length(t)/2);
for i = 1: N
   y = a0(i)/2;
   for j = 18:35
    y = y + a(j,i)*cos(f(j)*2*pi*t_r)+b(j,i)*2*sin(f(j)*pi*t_r);
   end   
   y_filtered(:,i) = y;
end

% Memory

A0 = a0;
A = a(18:35,:);
B = b(18:35,:);
F = f(18:35);
%Sorting
for i=1:length(vel)
   for j = 1:i
       if vel(j) > vel(i)
           vel([i j]) = vel([j i]);
           acc(:,[i j]) = acc(:,[j i]);
           y_filtered(:,[i j]) = y_filtered(:,[j i]);
           A0([i j]) = A0([j i]);
           A(:,[i j]) =  A(:,[j i]);
           B(:,[i j]) = B(:,[j i]);
       end
   end
    
end

 dlmwrite('driven_data.txt',[length(vel) length(A(:,1)) vel(1) vel(end)],'delimiter','\t','precision',10);
 dlmwrite('driven_data.txt',vel','delimiter','\t','precision',10,'-append');
 dlmwrite('driven_data.txt',F,'delimiter','\t','precision',10,'-append');
 dlmwrite('driven_data.txt',A0','delimiter','\t','precision',10,'-append');
 dlmwrite('driven_data.txt',A,'delimiter','\t','precision',10,'-append');
dlmwrite('driven_data.txt',B,'delimiter','\t','precision',10,'-append');

%Interpolation with velocity
vi = vel(1);
Ai = NaN(length(A(:,1)),1);
Bi = NaN(length(A(:,1)),1);
A01 = interp1(vel,A0,vi,'CUBIC');
for i = 1:length(Ai)
   Ai(i) = interp1(vel,A(i,:),vi,'CUBIC')
   Bi(i) = interp1(vel,B(i,:),vi,'CUBIC') 
end

yy = A01/2;

t_r = t(1:length(t)/2);
% t_r = 0:0.001:0.1;
 for i = 1:length(Ai)
    yy = yy + Ai(i)*cos(F(i)*2*pi*t_r)+B(i)*sin(F(i)*2*pi*t_r);
 end  
 figure(1);
 plot(t_r,yy);
 legend(sprintf('Vel(%f)',vi));
 
 figure(2);
 plot(t_r,acc(1:length(t_r),1));

% Plot
% 
% for i = 1:N
%    figure(i);
% 
%    subplot(2,1,1);
%    plot(t,acc(:,i));
%    xlim([0,0.1]);
%    subplot(2,1,2);
%    plot(t_r,y_filtered(:,i));
%    legend(sprintf('Vel(%f)',vel(i)));
% end



% figure(2);
% j=1;
% for i = 1:N
%      subplot(4,6,i)
%     stem(f,a(1:length(f),i));
%     legend(sprintf('Vel(%f)',vel(i)));
%     j=j+1;
%     hold on;
%     title('an');
% %    xlim([100 180]);
%     xlabel('Freq');
%     ylabel('Amplitude');
% 
% end
%     
% figure(3);
% j=1;
% for i = 1:N
%      subplot(4,6,i)
%     stem(f,b(1:length(f),i));
%     legend(sprintf('Vel(%f)',vel(i)));
%     j=j+1;
%     hold on;
%     title('bn');
%     xlabel('Freq');
%  %   xlim([100 180]);
%     ylabel('Amplitude');
% 
% end

    
% figure(1);
% j=1;
% for i = 1:N
%     stem(f,a(:,i));
%     s{j}=sprintf('%s%g','v=',vel(i));
%     j=j+1;
%     hold on;
%     title('an');
% %    xlim([100 180]);
%     xlabel('Freq');
%     ylabel('Amplitude');
% 
% end
%     legend(s);
%     
% figure(2);
% j=1;
% for i = 1:N
%     stem(f,b(:,i));
%     s{j}=sprintf('%s%g','v=',vel(i));
%     j=j+1;
%     hold on;
%     title('bn');
%     xlabel('Freq');
% %    xlim([100 180]);
%     ylabel('Amplitude');
% 
% end
%     legend(s);
