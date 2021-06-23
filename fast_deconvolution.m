clc
clear all;
close all;

L = 10000;    % lunghezza input x1 e x2 e output y1 e y2
M = 512;      % lunghezza dei filtri c11, c12, c21, c22
N = 1024;     % lunghezza dei filtri da calcolare h11, h12, h21, h22

x1 = 0.1*randn(L,1);  % input 1 (left)
x2 = 0.1*randn(L,1);  % input 2 (right)
% t = 0:L-1;
% x1 = sin(t)';
% x2 = cos(t)';

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

H11 = zeros(M,1);     % filtro da calcolare input 1 output 1
H12 = zeros(M,1);     % filtro da calcolare input 2 output 1
H21 = zeros(M,1);     % filtro da calcolare input 1 output 2
H22 = zeros(M,1);     % filtro da calcolare input 2 output 2

C11 = fft(c11);     % HRTF left loudspeaker - left ear
C12 = fft(c12);     % HRTF right loudspeaker - left ear
C21 = fft(c21);     % HRTF left loudspeaker - right ear
C22 = fft(c22);     % HRTF right loudspeaker - right ear

X1 = fft(x1, M);    % fft input 1 (left)
X2 = fft(x2, M);    % fft input 2 (right)

C_prev = [C11(1) C12(1); C21(1) C22(1)];
beta = 1;
B = [1 0; 0 1];

for n = 2:M
    C = [C11(n) C12(n); C21(n) C22(n)];
    H = (C_prev'*C+beta*(B)'*B)^(-1)*C_prev'; 
    H11(n) = H(1, 1);
    H12(n) = H(1, 2);
    H21(n) = H(2, 1);
    H22(n) = H(2, 2);
    C_prev = C;
end

h11 = real(ifft(H11, M));
h12 = real(ifft(H12, M));
h21 = real(ifft(H21, M));
h22 = real(ifft(H22, M));

%Y = zeros(2, M);
Y1 = zeros(1, M);
Y2 = zeros(1, M);

% for n = 1:M
%     Y(:,n) = [C11(n) C12(n); C21(n) C22(n)]*[H11(n) H12(n); H21(n) H22(n)]*[X1(n); X2(n)];
% end

for n = 1:M
    Y1(n) = (C11(n)*H11(n)+C12(n)*H21(n))*X1(n)+(C11(n)*H12(n)+C12(n)*H22(n))*X2(n);
    Y2(n) = (C21(n)*H11(n)+C22(n)*H21(n))*X1(n)+(C21(n)*H12(n)+C22(n)*H22(n))*X2(n);
end

y1 = real(ifft(Y1, M));
y2 = real(ifft(Y2, M));

figure('Name','Confronto tra x1 e y1','NumberTitle','off');
plot(ifft(X1)); 
hold on; 
plot(y1);
title('Confronto tra x_1 e y_1');
xlabel('Campioni')
ylabel('Ampiezza')
legend('x_1(n)', 'y_1(n)');

figure('Name','Confronto tra x2 e y2','NumberTitle','off');
plot(ifft(X2)); 
hold on; 
plot(y2);
title('Confronto tra x_2 e y_2');
xlabel('Campioni')
ylabel('Ampiezza')
legend('x_2(n)', 'y_2(n)'); 

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