% comparison between H11, H12, H21, H22 filters of the fast deconvolution
% and the lms algorithm as shown in Fig. 5 of "Comprehensive Comparison of the Least Mean Square Algorithm and the
% Fast Deconvolution Algorithm for Crosstalk Cancellation"
clc
close all 
clear all

load ("fast_deconvolution_filters.mat");
H11_fd = H11;
H12_fd = H12;
H21_fd = H21;
H22_fd = H22;

load ("lms_filters.mat");
H11_lms = H11;
H12_lms = H12;
H21_lms = H21;
H22_lms = H22;


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