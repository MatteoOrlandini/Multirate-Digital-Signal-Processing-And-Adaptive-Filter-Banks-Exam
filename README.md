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

![](https://github.com/MatteoOrlandini/Multirate-Digital-Signal-Processing-And-Adaptive-Filter-Banks-Exam/blob/main/Relazione/Immagini/left_channel_separation_LMS.jpg)

![](https://github.com/MatteoOrlandini/Multirate-Digital-Signal-Processing-And-Adaptive-Filter-Banks-Exam/blob/main/Relazione/Immagini/right_channel_separation_LMS.jpg)

![](https://github.com/MatteoOrlandini/Multirate-Digital-Signal-Processing-And-Adaptive-Filter-Banks-Exam/blob/main/Relazione/Immagini/left_channel_separation_FD.jpg)

![](https://github.com/MatteoOrlandini/Multirate-Digital-Signal-Processing-And-Adaptive-Filter-Banks-Exam/blob/main/Relazione/Immagini/right_channel_separation_FD.jpg)


For more information please see [Relazione.pdf](https://github.com/MatteoOrlandini/Multirate-Digital-Signal-Processing-And-Adaptive-Filter-Banks-Exam/blob/main/Relazione/Relazione.pdf).

# Bibliography
[1] Mingsian R. Bai e Chih-Chung Lee. «Objective and subjective analysis of effects of listening angle on crosstalk cancellation in spatial sound reproduction». In: The Journal of the Acoustical Society of America 120.4 (2006), pp. 1976–1989. doi: 10.1121/1.2257986. 

[2] E. Ferrara. «Fast implementations of LMS adaptive filters». In: IEEE Transactions on Acoustics, Speech, and Signal Processing 28 (1980), pp. 474–475. 

[3] Bill Gardner e Keith Martin. HRTF Measurements of a KEMAR Dummy- Head Microphone. Rapp. tecn. 280. (update on October 13, 2006). [Online]. Available: http://www.media.mit.edu. MIT Media Lab Perceptual Computing, 1994. 

[4] A. Gonzalez e J.J. Lopez. «Time domain recursive deconvolution in sound reproduction». In: 2000 IEEE International Conference on Acoustics, Speech, and Signal Processing. Proceedings (Cat. No.00CH37100). Vol. 2. 2000, II833–II836 vol.2. doi: 10.1109/ICASSP.2000.859089. 

[5] Emmanuel C. Ifeachor e Barrie W. Jervis. Digital Signal Processing: A Practical Approach. 2nd. Pearson Education, 2002. isbn: 0201596199. 

[6] Intel®. Intel® Integrated Performance Primitives. Dic. 2020. 

[7] Jun Seong Kim, Sang Gyun Kim e Chang D. Yoo. «A Novel Adaptive Crosstalk Cancellation using Psychoacoustic Model for 3D Audio». In: 2007 IEEE International Conference on Acoustics, Speech and Signal Processing - ICASSP ’07. Vol. 1. 2007, pp. I-185-I–188. doi: 10.1109/ ICASSP.2007.366647. 

[8] O. Kirkeby, P. Rubak e Angelo Farina. «Analysis of ill-conditioning of multi-channel deconvolution problems». In: feb. 1999, pp. 155–158. isbn: 0-7803-5612-8. 

[9] O. Kirkeby et al. «Fast deconvolution of multichannel systems using regularization». In: IEEE Transactions on Speech and Audio Processing 6.2 (1998), pp. 189–194. doi: 10.1109/89.661479. 

[10] Ole Kirkeby et al. «Design of Cross-Talk Cancellation Networks by Using Fast Deconvolution». In: (nov. 2000). 

[11] Chris Kyriakakis. «Fundamental and Technological Limitations of Immersive Audio Systems». In: Proceedings of the IEEE 86 (giu. 1998), pp. 941–951. doi: 10.1109/5.664281.

[12] Dan Li et al. «Comprehensive comparison of the least mean square algorithm and the fast deconvolution algorithm for crosstalk cancellation ». In: 2012 International Conference on Audio, Language and Image Processing. 2012, pp. 224–229. doi: 10.1109/ICALIP.2012. 6376616. 

[13] A. Mouchtaris, P. Reveliotis e C. Kyriakakis. «Inverse filter design for immersive audio rendering over loudspeakers». In: IEEE Transactions on Multimedia 2.2 (2000), pp. 77–87. doi: 10.1109/6046.845012. 

[14] P.A. Nelson, H. Hamada e S.J. Elliott. «Adaptive inverse filters for stereophonic sound reproduction». In: IEEE Transactions on Signal Processing 40.7 (1992), pp. 1621–1632. doi: 10.1109/78.143434. 

[15] Lin Wang e Yin Fuliang. «A Stereo Crosstalk Cancellation System Based on the Common-Acoustical Pole/Zero Model». In: EURASIP Journal on Advances in Signal Processing 2010 (gen. 2010). doi: 10. 1155/2010/719197.
