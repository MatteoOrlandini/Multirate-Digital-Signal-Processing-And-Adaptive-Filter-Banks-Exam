clear all;
close all;

L = 10000;    % lunghezza input x1 e x2 e output y1 e y2
M = 256;      % lunghezza dei filtri c11, c12, c21, c22
N = 1024;     % lunghezza dei filtri da calcolare h11, h12, h21, h22

t = 0:L-1;
x1 = sin(t)';
x2 = cos(t)';

c11 = fir1(M-1,0.3);    % HRIR input 1 output 1
c12 = fir1(M-1,0.4);    % HRIR input 2 output 1
c21 = fir1(M-1,0.3);    % HRIR input 1 output 2
c22 = fir1(M-1,0.4);    % HRIR input 2 output 2

H11 = zeros(M,1);     % filtro da calcolare input 1 output 1
H12 = zeros(M,1);     % filtro da calcolare input 2 output 1
H21 = zeros(M,1);     % filtro da calcolare input 1 output 2
H22 = zeros(M,1);     % filtro da calcolare input 2 output 2

C11 = fft(c11);
C12 = fft(c12);
C21 = fft(c21);
C22 = fft(c22);

X1 = fft(x1, M);
X2 = fft(x2, M);

C_prev = [C11(1) C12(1); C21(1) C22(1)];
beta = 1;
B = [1 0; 0 1];

for n = 2:M
    C = [C11(n) C12(n); C21(n) C22(n)];
    H = (C_prev'*C+beta*B'*B)^(-1)*C_prev'; 
    H11(n) = H(1, 1);
    H12(n) = H(1, 2);
    H21(n) = H(2, 1);
    H22(n) = H(2, 2);
    C_prev = C;
end

h11 = ifft(H11);
h12 = ifft(H12);
h21 = ifft(H21);
h22 = ifft(H22);

for n = 1:M
    Y(:,n) = [C11(n) C12(n); C21(n) C22(n)]*[H11(n) H12(n); H21(n) H22(n)]*[X1(n); X2(n)];
end

y1 = abs(ifft(Y(1,:)));
y2 = abs(ifft(Y(2,:)));

figure;
plot(abs(X1)); hold on; plot(abs(Y(1,:)));
legend('X_1(n)', 'Y_1(n)');
figure;
plot(abs(X2)); hold on; plot(abs(Y(2,:)));
legend('X_2(n)', 'Y_2(n)'); 

figure;
plot(ifft(X1)); hold on; plot(y1);
legend('x_1(n)', 'y_1(n)');
figure;
plot(ifft(X2)); hold on; plot(y2);
legend('x_2(n)', 'y_2(n)'); 