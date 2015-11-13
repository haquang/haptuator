clc;
clear;

%% Inititalization
T = 1; % 1 seconds
dt = 0.001; % 10 KHz sampling freq
Nsample = T/dt;    % Number of samples
nfft = 2^nextpow2(Nsample);
Fs = 1/dt;


freq1 = 150;  % signal frequency
freq2 = 100;  % signal frequency
freq3 = 80;  % signal frequency
freq4 = 250;  % signal frequency
alpha = -15; 

a0 = 0;
a = NaN(nfft/2);
b = NaN(nfft/2);

fs = 50;
%% Create Signal
t = 0:dt:T;
%x = exp(alpha*t).*(sin(2*pi*freq1*t) + sin(2*pi*freq2*t)+ sin(2*pi*freq3*t)+ sin(2*pi*freq4*t));
%x = 8/pi^2 *(sin(2*pi*fs*t) - 1/9 * sin(6*pi*fs*t) + 1/25 * sin(10*pi*fs*t)  - 1/49 * sin(14*pi*fs*t) + 1/81 * sin(18*pi*fs*t) );
x = 4/pi *(sin(2*pi*fs*t) + 1/3 * sin(6*pi*fs*t) + 1/5 * sin(10*pi*fs*t)  + 1/7 * sin(14*pi*fs*t) + 1/9 * sin(18*pi*fs*t) + 1/11 * sin(22*pi*fs*t) );

%% Processing
%% FFT
% x_fft = fft(x);
% f = Fs/2*linspace(0,1,nfft/2 -1);
% 
% a0 = x_fft(1)/nfft;
% a = 2*real(x_fft(2:nfft/2))/nfft;
% b = -2*imag(x_fft(2:nfft/2))/nfft;
% mag =sqrt(b.^2+a.^2);
% 
% % Filter and reconstruct signal
% tr = t(1:round(length(t)/2));
% y(1) = a0/2;
% f_start = 60;
% f_stop = 200;
% for i = f_start: f_stop
%    y = y + a(i)*cos(f(i)*2*pi*tr)+b(i)*2*sin(f(i)*pi*tr);
% end
% A0 = a0;
% A = a(f_start:f_stop);
% B = b(f_start:f_stop);
% F = f(f_start:f_stop);
% %% Save to file
% dlmwrite('driven_data.txt',A0','delimiter','\t','precision',5,'-append');
% dlmwrite('driven_data.txt',F,'delimiter','\t','precision',5,'-append');
% dlmwrite('driven_data.txt',A,'delimiter','\t','precision',5,'-append');
% dlmwrite('driven_data.txt',B,'delimiter','\t','precision',5,'-append');



%% Plot

figure(1);
plot(t,x);

% figure(2);
% stem(f,mag);
% 
% figure(3);
% plot(tr(1:length(y)),y);
