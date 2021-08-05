clc
clear all;
close all;

format long
%digits(32)

%L = 10000;    % lunghezza input x1 e x2 e output y1 e y2
M = 512;      % lunghezza dei filtri c11, c12, c21, c22
%N = 1024;     % lunghezza dei filtri da calcolare h11, h12, h21, h22

%x1 = 0.1*randn(L,1);  % random input 1 (left)
%x2 = 0.1*randn(L,1);  % random input 2 (right)
[x, Fsample] = audioread('Daft Punk - Get Lucky_cut.wav');
x1 = x(:,1);
x2 = x(:,2);
L = length(x1);

% il segnale desiderato è x ritardato di tau campioni
tau = M;   % ritardo temporale
d1 = [zeros(tau,1); x1(1:end-tau)]; % segnale desiderato 1 (left)
d2 = [zeros(tau,1); x2(1:end-tau)]; % segnale desiderato 2 (right)

y1 = zeros(L,1);  % output 1 (left)
y2 = zeros(L,1);  % output 2 (right)

e1 = zeros(L,1);  % errore 1 (left)
e2 = zeros(L,1);  % errore 2 (right)

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

h11 = zeros(M,1);     % filtro da calcolare input 1 output 1
h12 = zeros(M,1);     % filtro da calcolare input 2 output 1
h21 = zeros(M,1);     % filtro da calcolare input 1 output 2
h22 = zeros(M,1);     % filtro da calcolare input 2 output 2

r111 = zeros(L,1);    % uscita di x1 filtrato da c11 per output y1
r112 = zeros(L,1);    % uscita di x1 filtrato da c12 per output y1
r211 = zeros(L,1);    % uscita di x2 filtrato da c11 per output y1
r212 = zeros(L,1);    % uscita di x2 filtrato da c12 per output y1
r222 = zeros(L,1);    % uscita di x2 filtrato da c22 per output y2
r221 = zeros(L,1);    % uscita di x2 filtrato da c21 per output y2
r122 = zeros(L,1);    % uscita di x1 filtrato da c22 per output y2
r121 = zeros(L,1);    % uscita di x1 filtrato da c21 per output y2

mu = 1e-4; 

x1buff = zeros(M, 1);
x2buff = zeros(M, 1);

r111buff = zeros(M,1);    
r112buff = zeros(M,1);    
r211buff = zeros(M,1);  
r212buff = zeros(M,1);    
r222buff = zeros(M,1);    
r221buff = zeros(M,1);    
r122buff = zeros(M,1);   
r121buff = zeros(M,1);  

for n = 1:L
    x1buff = [x1(n); x1buff(1:end-1)];
    x2buff = [x2(n); x2buff(1:end-1)];
   
    r111(n) = c11'*x1buff;
    r112(n) = c12'*x1buff;
    r211(n) = c11'*x2buff;
    r212(n) = c12'*x2buff;
    r222(n) = c22'*x2buff;
    r221(n) = c21'*x2buff;
    r122(n) = c22'*x1buff;
    r121(n) = c21'*x1buff;

    r111buff = [r111(n); r111buff(1:end-1)];
    r112buff = [r112(n); r112buff(1:end-1)];
    r211buff = [r211(n); r211buff(1:end-1)];
    r212buff = [r212(n); r212buff(1:end-1)];
    r222buff = [r222(n); r222buff(1:end-1)];
    r221buff = [r221(n); r221buff(1:end-1)];
    r122buff = [r122(n); r122buff(1:end-1)];
    r121buff = [r121(n); r121buff(1:end-1)];
    
    y1(n) = h11'*r111buff+h21'*r112buff+h12'*r211buff+h22'*r212buff;
    y2(n) = h11'*r121buff+h21'*r122buff+h12'*r221buff+h22'*r222buff;
    
    e1(n) = d1(n)-y1(n);
    e2(n) = d2(n)-y2(n);
    %{
    h11 = h11 + mu.*(e1(n).*r111buff + e2(n).*r121buff);
    h12 = h12 + mu.*(e1(n).*r211buff + e2(n).*r221buff);
    h21 = h21 + mu.*(e1(n).*r112buff + e2(n).*r122buff);
    h22 = h22 + mu.*(e1(n).*r212buff + e2(n).*r222buff);
    %}
    
    for k = 1:M
        h11(k) = h11(k)+mu*(e1(n)*r111buff(k)+e2(n)*r121buff(k));
        h12(k) = h12(k)+mu*(e1(n)*r211buff(k)+e2(n)*r221buff(k));
        h21(k) = h21(k)+mu*(e1(n)*r112buff(k)+e2(n)*r122buff(k));
        h22(k) = h22(k)+mu*(e1(n)*r212buff(k)+e2(n)*r222buff(k));
    end
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

C11 = fft(c11);
C12 = fft(c12);
C21 = fft(c21);
C22 = fft(c22);

H11 = fft(h11, M);
H12 = fft(h12, M);
H21 = fft(h21, M);
H22 = fft(h22, M);

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

%{
% Confronto left channel separation con finestra rettangolare e con 
% filtro di cancellazione del crosstalk
W = ones(M,1);  % finestra rettangolare in frequenza
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
%}

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

%{
% Confronto right channel separation con finestra rettangolare e con 
% filtro di cancellazione del crosstalk
W = ones(M,1);  % finestra rettangolare in frequenza
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
%}
%}

save("lms_data.mat", "h11", "h12", "h21", "h22", "y1", "y2", "e1", "e2")

audioin = audioplayer([x1,x2],Fsample);
%play(audioin);
%pause(10);
%stop(audioin);

z1 = filter(c11, 1, x1) + filter(c12, 1, x2);
z2 = filter(c22, 1, x2) + filter(c21, 1, x1);
audioz = audioplayer([z1,z2],Fsample);
%play(audioz);
%pause(10);
%stop(audioz);

audioout = audioplayer([y1,y2],Fsample);
%play(audioout);
%pause(10);
%stop(audioout);