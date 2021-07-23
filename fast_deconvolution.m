clc
clear all;
close all;

%L = 10240;    % lunghezza input x1 e x2 e output y1 e y2
%M = 512;      % lunghezza dei filtri c11, c12, c21, c22
%N = 1024;     % lunghezza dei filtri da calcolare h11, h12, h21, h22

L = 1024;
M = L/2;

fs = L + M - 1; % frame size

%x1 = 0.1*randn(10*L,1);  % input 1 (left)
%x2 = 0.1*randn(10*L,1);  % input 2 (right)
[x, Fsample] = audioread('Daft Punk - Get Lucky_cut.wav');
x1 = x(:,1);
x2 = x(:,2);
nPoints = length(x1);

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

fftLen = 2.^nextpow2(fs);

H11 = zeros(fftLen,1);     % filtro di cancellazione del crosstalk input 1 output 1
H12 = zeros(fftLen,1);     % filtro di cancellazione del crosstalk input 2 output 1
H21 = zeros(fftLen,1);     % filtro di cancellazione del crosstalk input 1 output 2
H22 = zeros(fftLen,1);     % filtro di cancellazione del crosstalk input 2 output 2

C11 = fft(c11, fftLen);     % HRTF left loudspeaker - left ear
C12 = fft(c12, fftLen);     % HRTF right loudspeaker - left ear
C21 = fft(c21, fftLen);     % HRTF left loudspeaker - right ear
C22 = fft(c22, fftLen);     % HRTF right loudspeaker - right ear

C_prev = [C11(1) C12(1); C21(1) C22(1)];
beta = 0.1;
B = [1 0; 0 1];

for n = 2:length(H11)
    C = [C11(n) C12(n); C21(n) C22(n)];
    H = (C_prev'*C+beta*(B)'*B)^(-1)*C_prev'; 
    H11(n) = H(1, 1);
    H12(n) = H(1, 2);
    H21(n) = H(2, 1);
    H22(n) = H(2, 2);
    C_prev = C;
end

x1Buff = zeros(L, 1);
x2Buff = zeros(L, 1);

for i = 1 : floor(nPoints/L)
    % analysis
    % copy L values of x vector from M - 1 onwards
    x1Buff(fs - L + 1 : fs) = x1((i - 1) * L + 1 : i * L); 
    x2Buff(fs - L + 1 : fs) = x2((i - 1) * L + 1 : i * L); 
    X1BUFF = fft(x1Buff, fftLen);
    X2BUFF = fft(x2Buff, fftLen);
    
    % processing
    %%%{
    Y1BUFF = (C11.*H11+C12.*H21).*X1BUFF+(C11.*H12+C12.*H22).*X2BUFF;
    Y2BUFF = (C21.*H11+C22.*H21).*X1BUFF+(C21.*H12+C22.*H22).*X2BUFF;
    %}
    %{
    for n = 1:fftLen
        Y1BUFF(n) = (C11(n)*H11(n)+C12(n)*H21(n))*X1BUFF(n)+(C11(n)*H12(n)+C12(n)*H22(n))*X2BUFF(n);
        Y2BUFF(n) = (C21(n)*H11(n)+C22(n)*H21(n))*X1BUFF(n)+(C21(n)*H12(n)+C22(n)*H22(n))*X2BUFF(n);
    end
    %}
    % synthesis
    y1Buff = real(ifft(Y1BUFF));
    y2Buff = real(ifft(Y2BUFF));
    
    % discard first M - 1 values
    y1((i-1) * L + 1 : i * L) = y1Buff(fs - L + 1 : fs); 
    y2((i-1) * L + 1 : i * L) = y2Buff(fs - L + 1 : fs); 
    
    % first M - 1 values of the new vector must be the last ones of the previous array
    x1Buff(1 : M - 1) = x1Buff(fs - M + 2 : fs); 
    x2Buff(1 : M - 1) = x2Buff(fs - M + 2 : fs); 
end

figure('Name','Confronto tra x1 e y1','NumberTitle','off');
plot(x1); 
hold on; 
plot(y1);
xlim([0, length(y1)])
title('Confronto tra x_1 e y_1');
xlabel('Campioni')
ylabel('Ampiezza')
legend('x_1(n)', 'y_1(n)');

figure('Name','Confronto tra x2 e y2','NumberTitle','off');
plot(x2); 
hold on; 
plot(y2);
xlim([0, length(y1)])
title('Confronto tra x_2 e y_2');
xlabel('Campioni')
ylabel('Ampiezza')
legend('x_2(n)', 'y_2(n)'); 

% costruzione dell'asse delle frequenze
f = Fs*(0:(fftLen/2))/fftLen;

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
%{
% Confronto left channel separation con finestra rettangolare e con 
% filtro di cancellazione del crosstalk
W = ones(length(C11),1);  % finestra rettangolare in frequenza
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
%}
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

%{
% Confronto right channel separation con finestra rettangolare e con 
% filtro di cancellazione del crosstalk
W = ones(length(C11),1);  % finestra rettangolare in frequenza
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
%}

save("fast_deconvolution_filters.mat", "H11", "H12", "H21", "H22")

audioin = audioplayer([x1,x2],Fsample);
%play(audioin)
%pause(length([x1,x2])/Fsample + 1);

z1 = filter(c11, 1, x1) + filter(c12, 1, x2);
z2 = filter(c22, 1, x2) + filter(c21, 1, x1);
audioz = audioplayer([z1,z2],Fsample);
%play(audioz);
%pause(length([z1,z2])/Fsample + 1);

audioout = audioplayer([y1,y2],Fsample);
%play(audioout)
%pause(length([y1,y2])/Fsample + 1);