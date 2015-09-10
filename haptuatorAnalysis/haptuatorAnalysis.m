close all;
clear;
Fs=1000;
M=800;
N = 25;

%% Read data file
run('acc.m');
acc_x = data(:,1);
acc_y = data(:,2);
acc_z = data(:,3);
acc_sim = data(:,4);

NSamples = length(acc_x);
nfft = 2^nextpow2(NSamples);
t = (1/Fs)*(1:NSamples);

%% Fourier transform
f = Fs/2*linspace(0,1,nfft/2);
    acc_x_fft= fft(acc_x,nfft);
    acc_y_fft= fft(acc_y,nfft);
    acc_z_fft= fft(acc_z,nfft);    
    
    a0_x = acc_x_fft(1)/nfft;
    a_x = 2*real(acc_x_fft(2:nfft/2+1))/nfft;
    b_x = -2*imag(acc_x_fft(2:nfft/2+1))/nfft;

    a0_y = acc_y_fft(1)/nfft;
    a_y = 2*real(acc_y_fft(2:nfft/2+1))/nfft;
    b_y = -2*imag(acc_y_fft(2:nfft/2+1))/nfft;

    a0_z = acc_z_fft(1)/nfft;
    a_z = 2*real(acc_z_fft(2:nfft/2+1))/nfft;
    b_z = -2*imag(acc_z_fft(2:nfft/2+1))/nfft;

    
%% DFT321 algorithm
  
a0 = sqrt(a0_x.^2 + 0*a0_y.^2 +0*a0_z.^2);
a = sqrt(a_x.^2 + 0*a_y.^2 +0*a_z.^2);
b = sqrt(b_x.^2 + 0*b_y.^2 +0*b_z.^2);

%% Reconstruct signal
t_r = t(1:1000);
for i = 1:N
%     re = A(:,i) .* cos(phase(:,i));
%     im = A(:,i) .* sin(phase(:,i));
%     acc_new_fft = re + 1i*im;
%     %%% apply filter
%     acc_new = real(ifft(acc_new_fft,nfft));    
    y = a0/2;
    for j = 1:length(f)
        y = y + a(j)*cos(f(j)*2*pi*t_r)+b(j)*2*sin(f(j)*pi*t_r);
    end   
    acc_new = y;
end

%% plot
figure(1)
plot(t(1:M),acc_sim(1:M));
hold on;
plot(t_r,acc_new*1/3.3,'r');
%hold on
%plot(t(1:M),acc_z(1:M),'k');


