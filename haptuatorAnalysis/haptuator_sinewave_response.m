close all;
clear;
Fs=10000;

%% Read data file
run('acc.m');
acc_x = data(:,1);
acc_y = data(:,2);
acc_z = data(:,3);
acc_sim = data(:,4);
freq = data(:,5);
NSamples = length(acc_x);
nfft = 2^nextpow2(NSamples);
t = (1/Fs)*(1:NSamples);

%% Fourier transform
% f = Fs/2*linspace(0,1,nfft/2);
% acc_x_fft= fft(acc_x,nfft);
% acc_y_fft= fft(acc_y,nfft);
% acc_z_fft= fft(acc_z,nfft);    
% 
% a0_x = acc_x_fft(1)/nfft;
% a_x = 2*real(acc_x_fft(2:nfft/2+1))/nfft;
% b_x = -2*imag(acc_x_fft(2:nfft/2+1))/nfft;
% 
% a0_y = acc_y_fft(1)/nfft;
% a_y = 2*real(acc_y_fft(2:nfft/2+1))/nfft;
% b_y = -2*imag(acc_y_fft(2:nfft/2+1))/nfft;
% 
% a0_z = acc_z_fft(1)/nfft;
% a_z = 2*real(acc_z_fft(2:nfft/2+1))/nfft;
% b_z = -2*imag(acc_z_fft(2:nfft/2+1))/nfft;

    
%% DFT321 algorithm
  
% A_x = abs(acc_x_fft(1:nfft));
% A_y = abs(acc_y_fft(1:nfft));
% A_z = abs(acc_z_fft(1:nfft));
% A = sqrt(A_x.^2 +A_y.^2 +A_z.^2);
% phase = atan2(imag(acc_x_fft+acc_y_fft+acc_z_fft),real(acc_x_fft+acc_y_fft+acc_z_fft));

%   fullw = zeros(1, numel(A));
%   fullw( 100 : length(A)) = 1;
%   A = A.*fullw';
%  

%% Reconstruct signal
% re = A .* cos(phase);
% im = A .* sin(phase);
% acc_new_fft = re + 1i*im;
% acc_new = real(ifft(acc_new_fft,nfft));


%% plot
figure(1)
subplot(2,1,1);
plot(t,acc_sim);
xlim([0,3]);
ylim([-3,3]);
subplot(2,1,2);
plot(t,acc_x,'r');
hold on;
plot(t,acc_y,'b');
hold on;
plot(t,acc_z,'k');
xlim([0,3]);
% hold on
%plot(t(1:nfft/2),acc_new(1:nfft/2),'k');
%xlim([0,5]);
%ylim([-3,3]);

