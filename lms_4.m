clear all;
close all;

L = 10000;    % lunghezza input x1 e x2 e output y1 e y2
M = 256;      % lunghezza dei filtri c11, c12, c21, c22
N = 1024;     % lunghezza dei filtri da calcolare h11, h12, h21, h22

%x1 = 0.1*randn(L,1);  % input 1
%x2 = 0.1*randn(L,1);  % input 2
t = 0:L-1;
x1 = sin(t)';
x2 = cos(t)';

% il segnale desiderato è x ritardato di tau campioni
tau = 5;   % ritardo temporale
d1 = [zeros(tau,1); x1(1:end-tau)]; % segnale desiderato 1
d2 = [zeros(tau,1); x2(1:end-tau)]; % segnale desiderato 2

y1 = zeros(L,1);  % output 1
y2 = zeros(L,1);  % output 2

e1 = zeros(L,1);  % errore 1
e2 = zeros(L,1);  % errore 2

c11 = fir1(M-1,0.3);    % HRIR input 1 output 1
c12 = fir1(M-1,0.4);    % HRIR input 2 output 1
c21 = fir1(M-1,0.3);    % HRIR input 1 output 2
c22 = fir1(M-1,0.4);    % HRIR input 2 output 2

h11 = zeros(N,1);     % filtro da calcolare input 1 output 1
h12 = zeros(N,1);     % filtro da calcolare input 2 output 1
h21 = zeros(N,1);     % filtro da calcolare input 1 output 2
h22 = zeros(N,1);     % filtro da calcolare input 2 output 2

r111 = zeros(L,1);    % uscita di x1 filtrato da c11 per output y1
r112 = zeros(L,1);    % uscita di x1 filtrato da c12 per output y1
r211 = zeros(L,1);    % uscita di x2 filtrato da c11 per output y1
r212 = zeros(L,1);    % uscita di x2 filtrato da c12 per output y1
r222 = zeros(L,1);    % uscita di x2 filtrato da c22 per output y2
r221 = zeros(L,1);    % uscita di x2 filtrato da c21 per output y2
r122 = zeros(L,1);    % uscita di x1 filtrato da c22 per output y2
r121 = zeros(L,1);    % uscita di x1 filtrato da c21 per output y2

mu1 = 1/trace(x1*x1');    % prima era 1/trace(x1*x1'); 
mu2 = 1/trace(x2*x2');    % prima era 1/trace(x2*x2'); 

for n = N:L
    for j = 1:M % dubbio su questo for, metterlo fuori dal for n = N:L?
        r111(n) = r111(n)+c11(j)*x1(n-j+1);
        r112(n) = r112(n)+c12(j)*x1(n-j+1);
        r211(n) = r211(n)+c11(j)*x2(n-j+1);
        r212(n) = r212(n)+c12(j)*x2(n-j+1);
        r222(n) = r222(n)+c22(j)*x2(n-j+1);
        r221(n) = r221(n)+c21(j)*x2(n-j+1);
        r122(n) = r122(n)+c22(j)*x1(n-j+1);
        r121(n) = r121(n)+c21(j)+x1(n-j+1);
    end

    for p = 1:N
        y1(n) = y1(n)+r111(n-p+1)*h11(p)+r112(n-p+1)*h21(p)+r211(n-p+1)*h12(p)+r212(n-p+1)*h22(p);
        y2(n) = y2(n)+r121(n-p+1)*h11(p)+r122(n-p+1)*h21(p)+r221(n-p+1)*h12(p)+r222(n-p+1)*h22(p);
    end
    
    e1(n) = d1(n)-y1(n);
    e2(n) = d2(n)-y2(n);
    
    for k = 1:N
        h11(k) = h11(k)+mu1*e1(n)*r111(n-k+1)+mu2*e2(n)*r121(n-k+1);
        h12(k) = h12(k)+mu1*e1(n)*r112(n-k+1)+mu2*e2(n)*r221(n-k+1);
        h21(k) = h21(k)+mu1*e1(n)*r211(n-k+1)+mu2*e2(n)*r122(n-k+1);
        h22(k) = h22(k)+mu1*e1(n)*r212(n-k+1)+mu2*e2(n)*r222(n-k+1);
    end
end
    
figure;
plot(d1); hold on; plot(y1);
legend('d_1(n)', 'y_1(n)');
figure;
plot(d2); hold on; plot(y2);
legend('d_2(n)', 'y_2(n)');    