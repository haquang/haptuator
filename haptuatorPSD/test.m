Fs = 10000;            % Sampling frequency
T = 1/Fs;             % Sampling period
L = 400000;             % Length of signal
t = (0:L-1)*T;        % Time vector

S = sin(2*pi*100*t);
X = S + 0*randn(size(t));

plot(1000*t(1:100),X(1:100))
title('Signal Corrupted with Zero-Mean Random Noise')
xlabel('t (milliseconds)')
ylabel('X(t)')


Y = fft(X);
P2 = abs(Y/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);

f = Fs*(0:(L/2))/L;
plot(f,P1)
title('Single-Sided Amplitude Spectrum of X(t)')
xlabel('f (Hz)')
ylabel('|P1(f)|')