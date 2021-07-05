function [h, H] = update_filter(d, y, X, H, mu, L)
% UPDATE_FILTER  Update the filter H_k with the stepest descent method.
% [h, H] = update_filter(d, y, X, H, mu, L) updates the frequency response 
% H of the filter of size (L x 1) computing the error between the k-th block 
% of the desidered signal d of size (L x 1) and the k-th block block of the
% output y of size (L x 1). The gradient is computed using the error and the
% fft of two blocks X of size (2L x 1) of the input signal. mu is the step
% size of the LMS algorithm. h and H are the impulse response and the
% frequency response of the updated filter, respectively.

    % the error E is the fft of the difference between the k-th block of 
    % the desidered signal d and the output signal y with zero padding
    % before
    E = fft([zeros(L,1); d - y]);                      
    % the gradient are the first L elements of the inverse Fourier
    % transform of the element-by-element multiplication of the error E 
    % with the input X of the filter to update
    grad = real(ifft(E.*conj(X)));
    grad = grad(1:L);
    % updating the filter H
    H = H + 2*mu*fft([grad; zeros(L,1)]);
    % h, the impulse response, is the inverse Fourier transform of H, the
    % frequency response
    h = real(ifft(H, L));
end

