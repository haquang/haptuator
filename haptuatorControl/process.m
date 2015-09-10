close all;
clear;
Fs=1000;
M=200;
N = 24;
NSamples = 500;
t = (1/Fs)*(1:NSamples+1);

acc_x = NaN(NSamples+1,N);
acc_y = NaN(NSamples+1,N);
acc_z = NaN(NSamples+1,N);

vel = NaN(N,1);

nfft = 2^nextpow2(M);
acc_x_fft_org = NaN(nfft,N);
acc_y_fft_org = NaN(nfft,N);
acc_z_fft_org = NaN(nfft,N);

acc_x_fft = NaN(nfft,N);
acc_y_fft = NaN(nfft,N);
acc_z_fft = NaN(nfft,N);

A = NaN(nfft/2+1,N);
phase = NaN(nfft/2+1,N);

a0_x = NaN(N,1);
a_x = NaN(nfft/2,N);
b_x = NaN(nfft/2,N);

a0_y = NaN(N,1);
a_y = NaN(nfft/2,N);
b_y = NaN(nfft/2,N);

a0_z = NaN(N,1);
a_z = NaN(nfft/2,N);
b_z = NaN(nfft/2,N);

a0 = NaN(N,1);
a = NaN(nfft/2,N);
b = NaN(nfft/2,N);

acc_new = NaN(M,N);

%% read data from file
for i = 1: N
    input = strcat('Test_Acc_N',num2str(i) , '.txt')
    [vel(i),acc_x(:,i),acc_y(:,i),acc_z(:,i)] =  accFromData(input,NSamples);    
end

%% Fourier transform
f = Fs/2*linspace(0,1,nfft/2);
for i=1:N
    acc_x_fft(:,i)= fft(acc_x(:,i),nfft);
    acc_y_fft(:,i)= fft(acc_y(:,i),nfft);
    acc_z_fft(:,i)= fft(acc_z(:,i),nfft);    
    
    a0_x(i) = acc_x_fft(1,i)/nfft;
    a_x(:,i) = 2*real(acc_x_fft(2:nfft/2+1,i))/nfft;
    b_x(:,i) = -2*imag(acc_x_fft(2:nfft/2+1,i))/nfft;

    a0_y(i) = acc_y_fft(1,i)/nfft;
    a_y(:,i) = 2*real(acc_y_fft(2:nfft/2+1,i))/nfft;
    b_y(:,i) = -2*imag(acc_y_fft(2:nfft/2+1,i))/nfft;

    a0_z(i) = acc_z_fft(1,i)/nfft;
    a_z(:,i) = 2*real(acc_z_fft(2:nfft/2+1,i))/nfft;
    b_z(:,i) = -2*imag(acc_z_fft(2:nfft/2+1,i))/nfft;
end

%% DFT321 algorithm
for i = 1:N
%  A_x = abs(acc_x_fft(1:nfft/2+1,i));
%  A_y = abs(acc_y_fft(1:nfft/2+1,i));
%  A_z = abs(acc_z_fft(1:nfft/2+1,i));
%  A(:,i) = sqrt(A_x.^2 +A_y.^2 +A_z.^2);
%  phase(:,i) = atan2(imag(acc_x_fft(1:nfft/2+1,i)+acc_y_fft(1:nfft/2+1,i)+acc_z_fft(1:nfft/2+1,i)),real(acc_x_fft(1:nfft/2+1,i)+acc_y_fft(1:nfft/2+1,i)+acc_z_fft(1:nfft/2+1,i)));   
%  
a0(i) = sqrt(a0_x(i).^2 + a0_y(i).^2 +a0_z(i).^2);
a(:,i) = sqrt(a_x(:,i).^2 + a_y(:,i).^2 +a_z(:,i).^2);
b(:,i) = sqrt(b_x(:,i).^2 + b_y(:,i).^2 +b_z(:,i).^2);


end
 

%% apply filter
%   fullw = zeros(1, numel(acc_x_fft_org));
%   fullw( 15 : 40 ) = 1;
%   acc_x_fft = acc_x_fft_org.*fullw';
%   acc_y_fft = acc_y_fft_org.*fullw';
%   acc_z_fft = acc_z_fft_org.*fullw';


%% Reconstruc signal
t_r = t(1:M);
for i = 1:N
%     re = A(:,i) .* cos(phase(:,i));
%     im = A(:,i) .* sin(phase(:,i));
%     acc_new_fft = re + 1i*im;
%     %%% apply filter
%     acc_new = real(ifft(acc_new_fft,nfft));    
    y = a0(i)/2;
    for j = 15:40
        y = y + a(j,i)*cos(f(j)*2*pi*t_r)+b(j,i)*2*sin(f(j)*pi*t_r);
    end   
    acc_new(:,i) = y;

end

%% Memory for driven data

% Memory

A0 = a0;
A = a(15:40,:);
B = b(15:40,:);
F = f(15:40);
%Sorting
for i=1:length(vel)
   for j = 1:i
       if vel(j) > vel(i)
           vel([i j]) = vel([j i]);
           acc_x(:,[i j]) = acc_x(:,[j i]);
           acc_y(:,[i j]) = acc_y(:,[j i]);
           acc_z(:,[i j]) = acc_z(:,[j i]);
           acc_new(:,[i j]) = acc_new(:,[j i]);
%           y_filtered(:,[i j]) = y_filtered(:,[j i]);
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


%% Plot
%plot(acc_sim,'r-');
%hold on;
for i = 1:N
    figure(i)
    subplot(4,1,1);
    plot(acc_x(:,i));
    subplot(4,1,2);
    plot(acc_y(:,i));
    subplot(4,1,3);
    plot(acc_z(:,i));
    subplot(4,1,4);
    plot(acc_new(:,i),'b');  
end

% hold on;
% plot(acc_x,'k');
% hold on;
% plot(acc_y,'g');
% hold on;
% plot(acc_z,'y');

%  figure(2);
%  subplot(3,1,1);
%  plot(acc_x(:,1));
%  subplot(3,1,2);
%  plot(acc_y(:,1));
%  subplot(3,1,3);
%  plot(acc_z(:,1));

% for i=1:N
%    figure(i);
%    subplot(2,1,1);
%    stem(f,real(A(1:length(f),i)));
%    subplot(2,1,2);
%    stem(f,real(phase(1:length(f),i)));
% end
