#  Multirate-Digital-Signal-Processing-And-Adaptive-Filter-Banks-Exam

# Introduction
This paper aims to reproduce the results obtained in the paper [12], which shows a comparison between the Least Mean Square and Fast algorithm Deconvolution for the cancellation of crosstalk. Unlike classic stereo systems, a 3D sound system allows to position the sounds around a listener so that they are perceived as coming from arbitrary points in space. So, 3D audio can enhance the sense of realism in music or movies, and it can be of great benefit in virtual reality, augmented reality, in remote video conferencing or for home entertainment.

## LMS (Least Mean Square) Algorithm
For the cancellation of the crosstalk the most common algorithm is that of the LMS which, despite being simple and accurate, has a fast convergence [14], [7]. Figure 1 shows the crosstalk cancellation diagram using LMS, x indicates the input, y the output, d the desired signal and e the error. Lowercase variables represent the signal in the domain of time and represents the delay of the system. For causal systems, the output y_i[n] should ideally be a delayed version of input x_i[n]. The r_{ilm} sequences are the input signals x_i[n] filtered by the functions clm che they have M taps in the time-domain.

![](https://github.com/MatteoOrlandini/Multirate-Digital-Signal-Processing-And-Adaptive-Filter-Banks-Exam/blob/main/Relazione/Immagini/lms.png)

Figure 1: Block diagram of crosstalk cancellation using LMS

## Fast Deconvolution
The deconvolution [9] [8] [10], in its most basic form, can be described as the task of calculating the input of a discrete time system knowing its output. It is usually assumed that the system is linear and that the input-output relationship is known precisely. Let's consider a function cost of type: J = E + V (f) where E is a measure of the sound pressure error: E = |Y1 - X1|^2 + |Y2 - X2|^2 and V(f) is a function of the frequency indicating the computational cost. In the practical implementation it is not possible to calculate all the output with only one matrix operation, the overlap and save technique must be used to filter input X with filters C and H. Overlap and save is useful for perform real-time filtering with a finite impulse response filter. This technique is used to do block convolution between a signal very long input x[n] and a FIR filter h[n].

![](https://github.com/MatteoOrlandini/Multirate-Digital-Signal-Processing-And-Adaptive-Filter-Banks-Exam/blob/main/Relazione/Immagini/ols.jpg)

Figure 2: Overlap and Save method

![](https://github.com/MatteoOrlandini/Multirate-Digital-Signal-Processing-And-Adaptive-Filter-Banks-Exam/blob/main/Relazione/Immagini/ols_diagram.png)

Figure 3: Overlap and Save implementation diagram

## Prereqs (at least)
* [Matlab](https://it.mathworks.com/products/matlab.html) 
* [Visual Studio 2019 v. 16.10.4](https://visualstudio.microsoft.com/it/)
* [NU-Tech 2.0](http://www.nu-tech-dsp.com/content.php?id=2&sid=1)

## How to run 
The repository contains a LMS filter and a Fast Deconvolution algorithm implemented both using Matlab and Visual Studio. The implementation are equivalent: it's used Matlab for its semplicity and Visual Studio to perform a real time filtering process with the support of NU-Tech.

### How to run the Matlab scripts
1. Open Matlab
2. Open [lms_4.m](https://github.com/MatteoOrlandini/Multirate-Digital-Signal-Processing-And-Adaptive-Filter-Banks-Exam/blob/main/lms_4.m) and click run to perferm the LMS algorithm.
3. Open [fast_deconvolution.m](https://github.com/MatteoOrlandini/Multirate-Digital-Signal-Processing-And-Adaptive-Filter-Banks-Exam/blob/main/fast_deconvolution.m) and click run to perferm the FD algorithm.

### How to run the NU-Tech files
1. Open NU-Tech
2. Select "Direct Sound" mode 
3. Right click -> Load Setup -> open lms_nutech.xml to run the LMS algorithm in real time.
4. Right click -> Load Setup -> open fd_nutech.xml to run the FD algorithm in real time.

# Results

![]()

For more information please see [Relazione.pdf](https://github.com/MatteoOrlandini/Multirate-Digital-Signal-Processing-And-Adaptive-Filter-Banks-Exam/blob/main/Relazione/Relazione.pdf).
