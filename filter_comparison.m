% comparison between H11, H12, H21, H22 filters of the fast deconvolution
% and the lms algorithm as shown in Fig. 5 of "Comprehensive Comparison of the Least Mean Square Algorithm and the
% Fast Deconvolution Algorithm for Crosstalk Cancellation"
clc
close all 
clear all

load ("fast_deconvolution_data.mat");
H11_fd = H11;
H12_fd = H12;
H21_fd = H21;
H22_fd = H22;

load ("lms_data.mat");
h11_lms = h11;
h12_lms = h12;
h21_lms = h21;
h22_lms = h22;

H11_lms = fft(h11, length(H11_fd));
H12_lms = fft(h12, length(H11_fd));
H21_lms = fft(h21, length(H11_fd));
H22_lms = fft(h22, length(H11_fd));

%frequency axes
f_df = linspace(0, 1, length(H11_fd(1:end/2)));
f_lms = linspace(0, 1, length(H11_lms(1:end/2)));

figure('Name','H11 comparison','NumberTitle','off');
% monolateral spectrum
plot(f_df, 20*log10(abs(H11_fd(1:end/2))));
hold on
plot(f_lms, 20*log10(abs(H11_lms(1:end/2))));
title('H11 comparison');
xlabel('Normalized Frequency (x\pi rad/sample)');
ylabel('Magnitude [dB]');
legend('FD', 'LMS')

figure('Name','H12 comparison','NumberTitle','off');
% monolateral spectrum
plot(f_df, 20*log10(abs(H12_fd(1:end/2))));
hold on
plot(f_lms, 20*log10(abs(H12_lms(1:end/2))));
title('H12 comparison');
xlabel('Normalized Frequency (x\pi rad/sample)');
ylabel('Magnitude [dB]');
legend('FD', 'LMS')

figure('Name','H21 comparison','NumberTitle','off');
% monolateral spectrum
plot(f_df, 20*log10(abs(H21_fd(1:end/2))));
hold on
plot(f_lms, 20*log10(abs(H21_lms(1:end/2))));
title('H21 comparison');
xlabel('Normalized Frequency (x\pi rad/sample)');
ylabel('Magnitude [dB]');
legend('FD', 'LMS')

figure('Name','H22 comparison','NumberTitle','off');
% monolateral spectrum
plot(f_df, 20*log10(abs(H22_fd(1:end/2))));
hold on
plot(f_lms, 20*log10(abs(H22_lms(1:end/2))));
title('H22 comparison');
xlabel('Normalized Frequency (x\pi rad/sample)');
ylabel('Magnitude [dB]');
legend('FD', 'LMS')

fileID = fopen('e1.dat','rb');
e1_NuTech = fread(fileID, Inf, 'float64');
fclose(fileID);

fileID = fopen('e2.dat','rb');
e2_NuTech = fread(fileID, Inf, 'float64');
fclose(fileID);

figure('Name','LMS error e1','NumberTitle','off');
plot(e1);
title('LMS error e_1');
xlabel('Samples');
ylabel('Amplitude');

figure('Name','LMS error e2','NumberTitle','off');
plot(e2);
title('LMS error e_2');
xlabel('Samples');
ylabel('Amplitude');