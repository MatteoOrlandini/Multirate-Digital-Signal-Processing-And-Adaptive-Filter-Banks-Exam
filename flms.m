clc
clear all;
close all;

L = 512;
nPoints = 10*L;

x1 = 0.1*randn(nPoints,1);  % input 1 (left)
x2 = 0.1*randn(nPoints,1);  % input 2 (right)

% il segnale desiderato è x ritardato di tau campioni
tau = 2*L;   % ritardo temporale
d1 = [zeros(tau,1); x1(1:end-tau)]; % segnale desiderato 1 (left)
d2 = [zeros(tau,1); x2(1:end-tau)]; % segnale desiderato 2 (right)

y1 = zeros(nPoints,1);  % output 1 (left)
y2 = zeros(nPoints,1);  % output 2 (right)

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
c11 = [zeros(10,1); 1; zeros(M-10,1)];
c12 = [zeros(100,1); 1; zeros(M-101,1)];
c21 = [zeros(100,1); 1; zeros(M-101,1)];
c22 = [zeros(10,1); 1; zeros(M-10,1)];
%}

h11 = zeros(L,1);     % filtro da calcolare input 1 output 1
h12 = zeros(L,1);     % filtro da calcolare input 2 output 1
h21 = zeros(L,1);     % filtro da calcolare input 1 output 2
h22 = zeros(L,1);     % filtro da calcolare input 2 output 2

r111 = zeros(nPoints,1);    % uscita di x1 filtrato da c11 per output y1
r112 = zeros(nPoints,1);    % uscita di x1 filtrato da c12 per output y1
r211 = zeros(nPoints,1);    % uscita di x2 filtrato da c11 per output y1
r212 = zeros(nPoints,1);    % uscita di x2 filtrato da c12 per output y1
r222 = zeros(nPoints,1);    % uscita di x2 filtrato da c22 per output y2
r221 = zeros(nPoints,1);    % uscita di x2 filtrato da c21 per output y2
r122 = zeros(nPoints,1);    % uscita di x1 filtrato da c22 per output y2
r121 = zeros(nPoints,1);    % uscita di x1 filtrato da c21 per output y2

%mu1 = 1/trace(x1*x1');
% mu2 = 1/trace(x2*x2');

% mu1 = 2e-2; 
% mu2 = 1e-5;

mu1 = 1e-2; 
mu2 = 1e-2;

% H11 = fft([h11; zeros(L, 1)], 2*L);
% H12 = fft([h12; zeros(L, 1)], 2*L);
% H21 = fft([h21; zeros(L, 1)], 2*L);
% H22 = fft([h22; zeros(L, 1)], 2*L);

for k = 1 : floor(nPoints/L - 1)      
    [r111(k*L+1 : k*L+L), X1_k, ~]= ola(x1((k-1)*L+1: (k-1)*L+L), ...
                                 x1(k*L+1 : k*L+L), c11, L);
    
    [r112(k*L+1 : k*L+L), ~, ~] = ola(x1((k-1)*L+1: (k-1)*L+L), ...
                                 x1(k*L+1 : k*L+L), c12, L);
                             
    [r211(k*L+1 : k*L+L), ~, ~] = ola(x2((k-1)*L+1: (k-1)*L+L), ...
                                 x2(k*L+1 : k*L+L), c11, L);
                             
    [r212(k*L+1 : k*L+L), ~, ~] = ola(x2((k-1)*L+1: (k-1)*L+L), ...
                                 x2(k*L+1 : k*L+L), c12, L);
                             
    [r222(k*L+1 : k*L+L), X2_k, ~] = ola(x2((k-1)*L+1: (k-1)*L+L), ...
                                 x2(k*L+1 : k*L+L), c22, L);
    
    [r221(k*L+1 : k*L+L), ~, ~] = ola(x2((k-1)*L+1: (k-1)*L+L), ...
                                 x2(k*L+1 : k*L+L), c21, L);
                             
    [r122(k*L+1 : k*L+L), ~, ~] = ola(x1((k-1)*L+1: (k-1)*L+L), ...
                                 x1(k*L+1 : k*L+L), c22, L);
                             
    [r121(k*L+1 : k*L+L), ~, ~] = ola(x1((k-1)*L+1: (k-1)*L+L), ...
                                 x1(k*L+1 : k*L+L), c21, L);
                             
    [y1_k, R111_k, H11] = ola(r111((k-1)*L+1: (k-1)*L+L), ...
                                        r111(k*L+1 : k*L+L), h11, L);
                                        
    y1(k*L+1 : k*L+L) = y1(k*L+1 : k*L+L) + y1_k;
    
    [y1_k, R112_k, H21] = ola(r112((k-1)*L+1: (k-1)*L+L), ...
                               r112(k*L+1 : k*L+L), h21, L);
                           
    y1(k*L+1 : k*L+L) = y1(k*L+1 : k*L+L) + y1_k;  
    
    [y1_k, R211_k, H12] = ola(r211((k-1)*L+1: (k-1)*L+L), ...
                               r211(k*L+1 : k*L+L), h12, L);
                           
    y1(k*L+1 : k*L+L) = y1(k*L+1 : k*L+L) + y1_k;  
    
    [y1_k, R212_k, H22] = ola(r212((k-1)*L+1: (k-1)*L+L), ...
                               r212(k*L+1 : k*L+L), h22, L);
                           
    y1(k*L+1 : k*L+L) = y1(k*L+1 : k*L+L) + y1_k;
                           
    [y2_k, R222_k, ~] = ola(r222((k-1)*L+1: (k-1)*L+L), ...
                                    r222(k*L+1 : k*L+L), h22, L);
                                
    y2(k*L+1 : k*L+L) = y2(k*L+1 : k*L+L) + y2_k;                       
    
    [y2_k, R221_k, ~] = ola(r221((k-1)*L+1: (k-1)*L+L), ...
                               r221(k*L+1 : k*L+L), h12, L);
                           
    y2(k*L+1 : k*L+L) = y2(k*L+1 : k*L+L) + y2_k;  
    
    [y2_k, R122_k, ~] = ola(r122((k-1)*L+1: (k-1)*L+L), ...
                               r122(k*L+1 : k*L+L), h21, L);
                           
    y2(k*L+1 : k*L+L) = y2(k*L+1 : k*L+L) + y2_k;  
    
    [y2_k, R121_k, ~] = ola(r121((k-1)*L+1: (k-1)*L+L), ...
                               r121(k*L+1 : k*L+L), h11, L);
                           
    y2(k*L+1 : k*L+L) = y2(k*L+1 : k*L+L) + y2_k;                        
    
    % update for output 1
    d1_k = d1(k*L+1 : k*L+L);
    y1_k = y1(k*L+1 : k*L+L);
    
    [h11, H11] = update_filter(d1_k, y1_k, R111_k, H11, mu1, L);
    [h21, H21] = update_filter(d1_k, y1_k, R112_k, H21, mu1, L);
    [h12, H12] = update_filter(d1_k, y1_k, R211_k, H12, mu1, L);
    [h22, H22] = update_filter(d1_k, y1_k, R212_k, H22, mu1, L);
    
    % update for output 2 
    d2_k = d2(k*L+1 : k*L+L);
    y2_k = y2(k*L+1 : k*L+L);
    
    [h11, H11] = update_filter(d2_k, y2_k, R121_k, H11, mu2, L);
    [h21, H21] = update_filter(d2_k, y2_k, R122_k, H21, mu2, L);
    [h12, H12] = update_filter(d2_k, y2_k, R221_k, H12, mu2, L);
    [h22, H22] = update_filter(d2_k, y2_k, R222_k, H22, mu2, L);
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

% costruzione dell'asse delle frequenze
f = Fs/2*(0:L)/L;

C11 = fft(c11,2*L);
C12 = fft(c12,2*L);
C21 = fft(c21,2*L);
C22 = fft(c22,2*L);

% Left channel separation
JL_num = C11.*H11+C12.*H21;
JL_den = C21.*H11+C22.*H21;
% spettro monolaterale
JL_num = JL_num(1:end/2+1);
JL_den = JL_den(1:end/2+1);
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
W = ones(2*L,1);  % finestra rettangolare in frequenza
JL_after = (C11.*H11+C12.*H21)./(C21.*H11+C22.*H21);
JL_before = (C11.*W+C12.*W)./(C21.*W+C22.*W);
% spettro monolaterale
JL_after = JL_after(1:end/2+1);
JL_before = JL_before(1:end/2+1);
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
JR_num = JR_num(1:end/2+1);
JR_den = JR_den(1:end/2+1);
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
W = ones(2*L,1);  % finestra rettangolare in frequenza
JR_after = (C22.*H22+C21.*H12)./(C12.*H22+C11.*H12);
JR_before = (C22.*W+C21.*W)./(C12.*W+C11.*W);
% spettro monolaterale
JR_after = JR_after(1:end/2+1);
JR_before = JR_before(1:end/2+1);
figure('Name','Right channel separation','NumberTitle','off');
plot(f, 20*log10(abs(JR_after)));
hold on
plot(f, 20*log10(abs(JR_before)));
title({'Confronto right channel separation con finestra rettangolare','e con filtro di cancellazione del xtalk'});
xlabel('Frequenza [Hz]');
ylabel('Ampiezza [dB]');
legend('JR Cancellazione xtalk', 'JR Finestra rettangolare')