clc
clear all;
close all;

%L = 10000;    % lunghezza input x1 e x2 e output y1 e y2
%M = 512;      % lunghezza dei filtri c11, c12, c21, c22
%N = 1024;     % lunghezza dei filtri da calcolare h11, h12, h21, h22


L = 1024;
M = L/2;
fs = L + M - 1;

x1 = 0.1*randn(1,10*L);  % input 1 (left)
x2 = 0.1*randn(1,10*L);  % input 2 (right)
nPoints = length(x1);
%t = 0:L-1;
%x1 = sin(t)';
%x2 = cos(t)';

% il segnale desiderato è x ritardato di tau campioni

y1 = zeros(1,10*L);  % output 1 (left)
y2 = zeros(1,10*L);  % output 2 (right)

e1 = zeros(1,10);  % errore 1 (left)
e2 = zeros(1,10);  % errore 2 (right)

% c11 = fir1(M-1,0.3);    % HRIR input 1 output 1
% c12 = fir1(M-1,0.4);    % HRIR input 2 output 1
% c21 = fir1(M-1,0.3);    % HRIR input 1 output 2
% c22 = fir1(M-1,0.4);    % HRIR input 2 output 2

% https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.39.9751&rep=rep1&type=pdf
% within "HRTF_measurements" folder, each filename has the format
% "XEEeAAAa.wav", where X is either "L" or "R" for left or right ear response, 
% respectively, EE is the elevation angle of the source in degrees, 
% from -40 to 90, and AAA is the azimuth of the source in degrees, from 0
% to 355. Elevation and azimuth angles indicate the location of the source
% relative to the KEMAR dummy head, for example elevation 0 azimuth 90 is
% directly to the right of the KEMAR.
% c11: HRIR left loudspeaker - left ear
[c11,Fs] = audioread("HRTF_measurements/elev0/L0e330a.wav");  
% c12: HRIR right loudspeaker - left ear
[c12,~] = audioread("HRTF_measurements/elev0/L0e030a.wav");     
% c21: HRIR left loudspeaker - right ear
[c21,~] = audioread("HRTF_measurements/elev0/R0e330a.wav");    
% c22: HRIR right loudspeaker - righ ear
[c22,~] = audioread("HRTF_measurements/elev0/R0e030a.wav");  
%{
Fs = 44100;
c11 = [1; zeros(M-1,1)];
c12 = [zeros(100,1); 1; zeros(M-101,1)];
c21 = [zeros(100,1); 1; zeros(M-101,1)];
c22 = [1; zeros(M-1,1)];
%}

% h11 = zeros(N,1);     % filtro da calcolare input 1 output 1
% h12 = zeros(N,1);     % filtro da calcolare input 2 output 1
% h21 = zeros(N,1);     % filtro da calcolare input 1 output 2
% h22 = zeros(N,1);     % filtro da calcolare input 2 output 2

fftLen = 2.^nextpow2(fs);

H11 = zeros(1,fftLen);     % filtro di cancellazione del crosstalk input 1 output 1
H12 = zeros(1,fftLen);     % filtro di cancellazione del crosstalk input 2 output 1
H21 = zeros(1,fftLen);     % filtro di cancellazione del crosstalk input 1 output 2
H22 = zeros(1,fftLen);     % filtro di cancellazione del crosstalk input 2 output 2

C11 = fft(c11', fftLen);     % HRTF left loudspeaker - left ear
C12 = fft(c12', fftLen);     % HRTF right loudspeaker - left ear
C21 = fft(c21', fftLen);     % HRTF left loudspeaker - right ear
C22 = fft(c22', fftLen);     % HRTF right loudspeaker - right ear

tau = 2*length(c11);   % ritardo temporale

d1 = [zeros(1,tau) x1(1:end-tau)]; % segnale desiderato 1 (left)
d2 = [zeros(1,tau) x2(1:end-tau)]; % segnale desiderato 2 (right)

% r111 = zeros(1,10*L);    % uscita di x1 filtrato da c11 per output y1
% r112 = zeros(1,10*L);    % uscita di x1 filtrato da c12 per output y1
% r211 = zeros(1,10*L);    % uscita di x2 filtrato da c11 per output y1
% r212 = zeros(1,10*L);    % uscita di x2 filtrato da c12 per output y1
% r222 = zeros(1,10*L);    % uscita di x2 filtrato da c22 per output y2
% r221 = zeros(1,10*L);    % uscita di x2 filtrato da c21 per output y2
% r122 = zeros(1,10*L);    % uscita di x1 filtrato da c22 per output y2
% r121 = zeros(1,10*L);    % uscita di x1 filtrato da c21 per output y2

%mu1 = 1/trace(x1*x1');
% mu2 = 1/trace(x2*x2');

% mu1 = 2e-2; 
% mu2 = 1e-5;

mu1 = 5e-4; 
mu2 = 5e-4;

r111 = filter(c11,1,x1);    % uscita di x1 filtrato da c11 per output y1
r112 = filter(c12,1,x1);    % uscita di x1 filtrato da c12 per output y1
r211 = filter(c11,1,x2);    % uscita di x2 filtrato da c11 per output y1
r212 = filter(c12,1,x2);    % uscita di x2 filtrato da c12 per output y1
r222 = filter(c22,1,x2);    % uscita di x2 filtrato da c22 per output y2
r221 = filter(c21,1,x2);    % uscita di x2 filtrato da c21 per output y2
r122 = filter(c22,1,x1);    % uscita di x1 filtrato da c22 per output y2
r121 = filter(c21,1,x1);    % uscita di x1 filtrato da c21 per output y2

x1Buff = zeros(1,L);
x2Buff = zeros(1,L);

for i = 1 : floor(nPoints/L)
    % analysis
    % copy L values of x vector from M - 1 onwards
    x1Buff(fs - L + 1 : fs) = x1((i - 1) * L + 1 : i * L); 
    x2Buff(fs - L + 1 : fs) = x2((i - 1) * L + 1 : i * L); 
    X1BUFF = fft(x1Buff, fftLen);
    X2BUFF = fft(x2Buff, fftLen);
    
    d1Buff(fs - L + 1 : fs) = d1((i - 1) * L + 1 : i * L); 
    d2Buff(fs - L + 1 : fs) = d2((i - 1) * L + 1 : i * L); 
    D1BUFF = fft(d1Buff, fftLen);
    D2BUFF = fft(d2Buff, fftLen);
    
    r111Buff(fs - L + 1 : fs) = r111((i - 1) * L + 1 : i * L); 
    R111BUFF = fft(r111Buff, fftLen);
    
    r112Buff(fs - L + 1 : fs) = r112((i - 1) * L + 1 : i * L); 
    R112BUFF = fft(r112Buff, fftLen);
    
    r211Buff(fs - L + 1 : fs) = r211((i - 1) * L + 1 : i * L); 
    R211BUFF = fft(r211Buff, fftLen);
        
    r212Buff(fs - L + 1 : fs) = r212((i - 1) * L + 1 : i * L); 
    R212BUFF = fft(r212Buff, fftLen);
        
    r222Buff(fs - L + 1 : fs) = r222((i - 1) * L + 1 : i * L); 
    R222BUFF = fft(r222Buff, fftLen);
    
    r221Buff(fs - L + 1 : fs) = r221((i - 1) * L + 1 : i * L); 
    R221BUFF = fft(r221Buff, fftLen);
    
    r122Buff(fs - L + 1 : fs) = r122((i - 1) * L + 1 : i * L); 
    R122BUFF = fft(r122Buff, fftLen);
    
    r121Buff(fs - L + 1 : fs) = r121((i - 1) * L + 1 : i * L); 
    R121BUFF = fft(r121Buff, fftLen);
    
%     R111BUFF = X1BUFF.*C11;
%     R112BUFF = X1BUFF.*C12;
%     R211BUFF = X2BUFF.*C11;
%     R212BUFF = X2BUFF.*C12;
%     R222BUFF = X2BUFF.*C22;
%     R221BUFF = X2BUFF.*C21;
%     R122BUFF = X1BUFF.*C22;
%     R121BUFF = X1BUFF.*C21;
    
    Y1BUFF = R111BUFF.*H11+R112BUFF.*H21+R211BUFF.*H12+R212BUFF.*H22;
    Y2BUFF = R121BUFF.*H11+R122BUFF.*H21+R221BUFF.*H12+R222BUFF.*H22;
    
    E1 = D1BUFF - Y1BUFF;
    E2 = D2BUFF - Y2BUFF;
    
    H11 = H11+mu1*E1.*R111BUFF+mu2*E2.*R121BUFF;
    H12 = H12+mu1*E1.*R211BUFF+mu2*E2.*R221BUFF;
    H21 = H21+mu1*E1.*R112BUFF+mu2*E2.*R122BUFF;
    H22 = H22+mu1*E1.*R212BUFF+mu2*E2.*R222BUFF;
    
    y1Buff = real(ifft(Y1BUFF));
    y2Buff = real(ifft(Y2BUFF));
    
        % discard first M - 1 values
    y1((i-1) * L + 1 : i * L) = y1Buff(fs - L + 1 : fs); 
    y2((i-1) * L + 1 : i * L) = y2Buff(fs - L + 1 : fs); 
    
    % first M - 1 values of the new vector must be the last ones of the previous array
    x1Buff(1 : M - 1) = x1Buff(fs - M + 2 : fs); 
    x2Buff(1 : M - 1) = x2Buff(fs - M + 2 : fs); 
    
    r111Buff(1 : M - 1) = r111Buff(fs - M + 2 : fs); 
    r112Buff(1 : M - 1) = r112Buff(fs - M + 2 : fs);     
    r211Buff(1 : M - 1) = r211Buff(fs - M + 2 : fs); 
    r212Buff(1 : M - 1) = r212Buff(fs - M + 2 : fs); 
    r222Buff(1 : M - 1) = r222Buff(fs - M + 2 : fs); 
    r221Buff(1 : M - 1) = r221Buff(fs - M + 2 : fs); 
    r122Buff(1 : M - 1) = r122Buff(fs - M + 2 : fs); 
    r121Buff(1 : M - 1) = r121Buff(fs - M + 2 : fs); 
end
    
figure('Name','Confronto tra d1 e y1','NumberTitle','off');
plot(d1); 
hold on; 
plot(y1);
title('Confronto tra d_1 e y_1')
xlabel('Campioni')
ylabel('Ampiezza')
legend('d_1(n)', 'y_1(n)');

figure('Name','Confronto tra d2 e y2','NumberTitle','off');
plot(d2); 
hold on; 
plot(y2);
title('Confronto tra d_2 e y_2')
xlabel('Campioni')
ylabel('Ampiezza')
legend('d_2(n)', 'y_2(n)');   

% C11 = fft(c11);
% C12 = fft(c12);
% C21 = fft(c21);
% C22 = fft(c22);

% H11 = fft(h11, M);
% H12 = fft(h12, M);
% H21 = fft(h21, M);
% H22 = fft(h22, M);

% costruzione dell'asse delle frequenze
f = Fs*(0:(M/2))/M;

% Left channel separation
JL_num = C11.*H11+C12.*H21;
JL_den = C21.*H11+C22.*H21;
% spettro monolaterale
JL_num = JL_num(1:M/2+1);
JL_den = JL_den(1:M/2+1);
figure('Name','Left channel separation','NumberTitle','off');
plot(f, 20*log10(abs(JL_num)));
hold on
plot(f, 20*log10(abs(JL_den)));
title('Left channel separation');
xlabel('Frequenza [Hz]');
ylabel('Ampiezza [dB]');
legend('JL_{num}', 'JL_{den}')

% Confronto left channel separation con finestra rettangolare e con 
% filtro di cancellazione del crosstalk
W = ones(1,length(C11));  % finestra rettangolare in frequenza
JL_after = (C11.*H11+C12.*H21)./(C21.*H11+C22.*H21);
JL_before = (C11.*W+C12.*W)./(C21.*W+C22.*W);
% spettro monolaterale
JL_after = JL_after(1:M/2+1);
JL_before = JL_before(1:M/2+1);
figure('Name','Left channel separation','NumberTitle','off');
plot(f, 20*log10(abs(JL_after)));
hold on
plot(f, 20*log10(abs(JL_before)));
title({'Confronto left channel separation con finestra rettangolare','e con filtro di cancellazione del xtalk'});
xlabel('Frequenza [Hz]');
ylabel('Ampiezza [dB]');
legend('JL Cancellazione xtalk', 'JL Finestra rettangolare')

% Right channel separation
JR_num = C22.*H22+C21.*H12;
JR_den = C12.*H22+C11.*H12;
% spettro monolaterale
JR_num = JR_num(1:M/2+1);
JR_den = JR_den(1:M/2+1);
figure('Name','Right channel separation','NumberTitle','off');
plot(f, 20*log10(abs(JR_num)));
hold on
plot(f, 20*log10(abs(JR_den)));
title('Right channel separation');
xlabel('Frequenza [Hz]');
ylabel('Ampiezza [dB]');
legend('JR_{num}', 'JR_{den}')

% Confronto right channel separation con finestra rettangolare e con 
% filtro di cancellazione del crosstalk
W = ones(1,length(C11));  % finestra rettangolare in frequenza
JR_after = (C22.*H22+C21.*H12)./(C12.*H22+C11.*H12);
JR_before = (C22.*W+C21.*W)./(C12.*W+C11.*W);
% spettro monolaterale
JR_after = JR_after(1:M/2+1);
JR_before = JR_before(1:M/2+1);
figure('Name','Right channel separation','NumberTitle','off');
plot(f, 20*log10(abs(JR_after)));
hold on
plot(f, 20*log10(abs(JR_before)));
title({'Confronto right channel separation con finestra rettangolare','e con filtro di cancellazione del xtalk'});
xlabel('Frequenza [Hz]');
ylabel('Ampiezza [dB]');
legend('JR Cancellazione xtalk', 'JR Finestra rettangolare')