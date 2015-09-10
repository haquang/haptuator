close all;
clear;
Fs=1000;
M=200;
NSamples = 1000;

nfft = 2^nextpow2(M);

%% read data from file
run('acc.m');
acc_x = data(53575:nfft+53575,1);
acc_y = data(53575:nfft+53575,2);
acc_z = data(53575:nfft+53575,3);
acc_sim = data(53575:nfft+53575,4);
%% Fourier transform
f = Fs/2*linspace(0,1,nfft/2);
acc_x_fft= fft(acc_x,nfft);
acc_y_fft= fft(acc_y,nfft);
acc_z_fft= fft(acc_z,nfft);


%% apply filter
% fullw = zeros(1, numel(acc_x_fft));
% fullw( 1 : 140 ) = 1;
% acc_x_fft_filter = acc_x_fft.*fullw';
% acc_y_fft_filter = acc_y_fft.*fullw';
% acc_z_fft_filter = acc_z_fft.*fullw';

%% DFT321 algorithm
A_x = abs(acc_x_fft(1:nfft));
A_y = abs(acc_y_fft(1:nfft));
A_z = abs(acc_z_fft(1:nfft));
A = sqrt(A_x.^2 +A_y.^2 +A_z.^2);
phase = atan2(imag(acc_x_fft+acc_y_fft+acc_z_fft),real(acc_x_fft+acc_y_fft+acc_z_fft));

%% Reconstruc signal
re = A .* cos(phase);
im = A .* sin(phase);
acc_new_fft = re + 1i*im;
%%% apply filter
fullw = zeros(1, numel(acc_new_fft));
fullw( 1 : 140 ) = 1;
acc_new_fft_filter = acc_new_fft;
acc_new = real(ifft(acc_new_fft_filter,nfft));
%% Plot
plot(acc_sim,'r-');
hold on;
plot(acc_new,'b');
% hold on;
% plot(acc_x,'k');
% hold on;
% plot(acc_y,'g');
% hold on;
% plot(acc_z,'y');



