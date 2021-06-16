clear all;
close all;

L=1000; %lunghezza ingresso e uscita
M=10; %lunghezza filtro

c=fir1(M,0.5); %filtro inventato
x=0.2*randn(L,1); %ingresso a caso
y=zeros(L,1); %inizializzazione uscita
e=zeros(L,1); %inizializzazione errore
b=zeros(M,1); %inizializzazione tappi bk del filtro che deve approssimare c
d=filter(c,1,x); %segnale desiderato = ingresso filtrato con c
%%%%    d=filter(c,1,x) is equal to the code below except for the first M samples of d1  %%%%
% d1 = zeros(L,1);
% for n = M:L
%     for k = 1:M
%         d1(n) = d1(n)+c(k)*x(n-k+1);
%     end
% end

mu=2/trace(x*x'); %step size

for n=M:L %parto dal M-esimo campione dell'ingresso in modo che il filtro può operare su M campioni
    for k=1:M %per ogni campione di ingresso il filtro agisce sui M campioni precedenti
        y(n)=y(n)+b(k)*x(n-k+1);
    end
    e(n)=d(n)-y(n); %errore
    for k=1:M
        b(k)=b(k)+mu*e(n)*x(n-k+1); %aggiorno i k tappi del filtro
    end
end

%y = filter(b, 1, x);
%e = d-y;

figure;
plot(d);
hold on;
plot(y);
legend('d(n)', 'y(n');