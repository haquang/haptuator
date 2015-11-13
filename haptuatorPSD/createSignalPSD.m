clc;
clear;
close all;
%% Inititalization
T = 0.3;
Fs = 10000;            % Sampling frequency
dt = 1/Fs;             % Sampling period
N = T/dt;

block_size = 400;
nfft = block_size;             % Length of signal
t = (0:N-1)*dt;        % Time vector


freq1 = 0;  % signal frequency
freq2 = 200;  % signal frequency
freq3 = 100;  % signal frequency
freq4 = 150;  % signal frequency
alpha = -0; 

a0 = 0;
a = NaN(nfft/2);
b = NaN(nfft/2);

%% Create Signal
x = exp(alpha*t).*(sin(2*pi*freq1*t) + sin(2*pi*freq2*t)+ sin(2*pi*freq3*t)+ sin(2*pi*freq4*t));
%% Processing
%% FFT
x_fft = fft(x,nfft);
x_fft = x_fft(1:block_size/2);
a0 = x_fft(1)/nfft;
a = 2*real(x_fft(1:nfft/2))/nfft;
b = -2*imag(x_fft(1:nfft/2))/nfft;
mag =sqrt(real(x_fft).^2 + imag(x_fft).^2);

% 
PSD1 = mag.^2/nfft;
f = Fs*(0:(nfft) -1)/nfft;
%% SDFT
x_dft = dft(x(1:block_size+1),block_size,f,Fs);
x_dft = x_dft(1:block_size/2);
%[x_dft] = sdft(X1,x(1),x(block_size+1),block_size/2,f);

a0 = x_dft(1)/block_size;
a = 2*real(x_dft(1:block_size/2))/block_size;
b = -2*imag(x_dft(1:block_size/2))/block_size;
mag =sqrt(real(x_dft).^2 + imag(x_dft).^2);

PSD2 = mag.^2/block_size;
f = f(1:length(f)/2);

% Filter and reconstruct signal
y(1) = a0/2;
f_start = 4;
f_stop = 11;
for i = f_start: f_stop
   y = y + a(i)*cos(f(i)*2*pi*t)+b(i)*sin(f(i)*2*pi*t);
end
A0 = a0;
A = a(f_start:f_stop);
B = b(f_start:f_stop);
F = f(f_start:f_stop);
PSD = PSD1(f_start:f_stop);

%% Save to file
dlmwrite('driven_data.txt',A0','delimiter','\t','precision',5,'-append');
dlmwrite('driven_data.txt',F,'delimiter','\t','precision',5,'-append');
dlmwrite('driven_data.txt',A','delimiter','\t','precision',5,'-append');
dlmwrite('driven_data.txt',B','delimiter','\t','precision',5,'-append');
 dlmwrite('driven_data.txt',PSD,'delimiter','\t','precision',5,'-append');


%% Plot

figure(1);
plot(t,x);

figure(2);
stem(f,PSD1(1:length(f)),'+');

hold on;
stem(f,PSD2(1:length(f)),'-r');
% figure(3);
% stem(f,PSD);

% 
figure(3);
plot(t,x);
hold on;
plot(t,y,'--r');
