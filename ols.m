function [y_k, X_k, H_k] = ols(x_k_prev, x_k, h, L)
% OLA  Overlap and save of a signal x passing through a filter h.
%     [y_k, X_k, H_k] = OLS(x_k_prev, x_k, h, L) computes the overlap and save of
%     x_k_prev, the (k-1)-th block of the input signal, and x_k, the k-th block of 
%     the input signal, with the filter h. x_k, x_k_prev and h are column
%     arrays of size (L x 1).
%
% Reference: E. R. Ferrara - "Fast Implementation of LMS Adaptive Filters"

    % fft of the filter h padded with L zeros at the end
    H_k = fft([h; zeros(L, 1)], 2*L);       % (9)
    % fft of the (k-1)-th and k-th block 
    X_k = fft([x_k_prev; x_k], 2*L);        % (10)
    % apply the filter to X_k
    Y_k = X_k.*H_k;    
    % inverse Fourier transform of Y_k
    y_k = real(ifft(Y_k, 2*L));             % (11)
    % taking only the last L values of the inverse Fourier transform of Y_k
    y_k = y_k(L+1:2*L);
end

