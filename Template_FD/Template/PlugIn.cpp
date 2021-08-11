#include "StdAfx.h"
#include ".\plugin.h"
#include "ipp.h"

PlugIn::PlugIn(InterfaceType _CBFunction,void * _PlugRef,HWND ParentDlg): LEEffect(_CBFunction,_PlugRef,ParentDlg)
{
	FrameSize = CBFunction(this,NUTS_GET_FS_SR,0,(LPVOID)AUDIOPROC);
	SampleRate = CBFunction(this,NUTS_GET_FS_SR,1,(LPVOID)AUDIOPROC);

	LESetNumInput(2);
	LESetNumOutput(2);
	
	isRunning = false;

	//M = 512;	// filter length
	L = FrameSize;
	M = int(L / 2);
	fs = L + M - 1;

	beta = 0.1;

	x1buff = 0;
	x2buff = 0;

	X1BUFF = 0;
	X2BUFF = 0;

	y1buff = 0;
	y2buff = 0;

	Y1BUFF = 0;
	Y2BUFF = 0;
	
	temp = 0;
	temp2 = 0;
	temp3 = 0;

	hrir = 0;
	c11 = 0;
	c12 = 0;
	c21 = 0;
	c22 = 0;

	C11 = 0;
	C12 = 0;
	C21 = 0;
	C22 = 0;

	H11 = 0;
	H12 = 0;
	H21 = 0;
	H22 = 0;

	sizeSpec = 0;
	sizeInit = 0;
	sizeBuffer = 0;
	/*
	sizeBuf = 0;
	pSpecMem = 0;
	pBuffer = 0;
	pmemInit = 0;
	*/

	//fftLen = round(2^ceil(log2(double(fs))));
	fftLen = 8192;
	fftOrd = (int)(log10((double)fftLen) / log10(2.0));
}

int __stdcall PlugIn::LEPlugin_Process(PinType** Input, PinType** Output, LPVOID ExtraInfo)
{
	double* InputData1 = ((double*)Input[0]->DataBuffer);
	double* OutputData1 = ((double*)Output[0]->DataBuffer);
	double* InputData2 = ((double*)Input[1]->DataBuffer);
	double* OutputData2 = ((double*)Output[1]->DataBuffer);

	//ippsCopy_64f(InputData1, x1buff + M - 1, L);
	//ippsCopy_64f(InputData2, x2buff + M - 1, L);
	for (int i = 0; i < L; i++)
	{
		x1buff[M - 1 + i].re = InputData1[i];
		x2buff[M - 1 + i].re = InputData2[i];
	}
	// divide L elements of the vector x1buff by 32768 and store the result in x1buff
	ippsDivC_64fc_I({ 32768.0, 0.0 }, x1buff + M - 1, L);
	// divide L elements of the vector x2buff by 32768 and store the result in x2buff
	ippsDivC_64fc_I({ 32768.0, 0.0 }, x2buff + M - 1, L);
	/****************** CCS FORMAT ********************/
	/*
	ippsFFTFwd_RToCCS_64f(x1buff, X1BUFF, fftState, pBuffer);
	ippsFFTFwd_RToCCS_64f(x2buff, X2BUFF, fftState, pBuffer);
	*/

	//ippsFFTFwd_RToPack_64f(x1buff, X1BUFF, fftState, pBuffer);
	//ippsFFTFwd_RToPack_64f(x2buff, X2BUFF, fftState, pBuffer);

	ippsFFTFwd_CToC_64fc(x1buff, X1BUFF, pSpec, pMemBuffer);
	ippsFFTFwd_CToC_64fc(x2buff, X2BUFF, pSpec, pMemBuffer);

	/****************** CCS FORMAT ********************/
	/*
	for (int n = 0; n < fftLen; n = n + 2)
	{
		temp = ((C11[n] + 1i * C11[n + 1]) * (H11[n] + 1i * H11[n + 1]) + (C12[n] + 1i * C12[n + 1]) * (H21[n] + 1i * H21[n + 1])) * (X1BUFF[n] + 1i * X1BUFF[n + 1]) + ((C11[n] + 1i * C11[n + 1]) * (H12[n] + 1i * H12[n + 1]) + (C12[n] + 1i * C12[n + 1]) * (H22[n] + 1i * H22[n + 1])) * (X2BUFF[n] + 1i * X2BUFF[n + 1]);
		Y1BUFF[n] = temp.real();
		Y1BUFF[n + 1] = temp.imag();
		temp = ((C21[n] + 1i * C21[n + 1]) * (H11[n] + 1i * H11[n + 1]) + (C22[n] + 1i * C22[n + 1]) * (H21[n] + 1i * H21[n + 1])) * (X1BUFF[n] + 1i * X1BUFF[n + 1]) + ((C21[n] + 1i * C21[n + 1]) * (H12[n] + 1i * H12[n + 1]) + (C22[n] + 1i * C22[n + 1]) * (H22[n] + 1i * H22[n + 1])) * (X2BUFF[n] + 1i * X2BUFF[n + 1]);
		Y2BUFF[n] = temp.real();
		Y2BUFF[n + 1] = temp.imag();
	}
	ippsFFTInv_CCSToR_64f(Y1BUFF, y1buff, fftState, pBuffer);
	ippsFFTInv_CCSToR_64f(Y2BUFF, y2buff, fftState, pBuffer);
	// get only the real values
	for (int i = 1; i < fftLen/2; i++)
	{
		y1buff[i] = y1buff[int(i * 2)];
		y2buff[i] = y2buff[int(i * 2)];
	}
	*/

	/************************ PACK FORMAT ***********************/
	/*
	// temp = C11.*H11
	ippsMulPack_64f(C11, H11, temp, fftLen);
	// temp2 = C12.*H21
	ippsMulPack_64f(C12, H21, temp2, fftLen);
	// temp2 = C11.*H11+C12.*H21
	ippsAdd_64f_I(temp, temp2, fftLen);
	// temp3 = (C11.*H11 + C12.*H21).*X1BUFF
	ippsMulPack_64f(temp2, X1BUFF, temp3, fftLen);
	// temp = C11.*H12
	ippsMulPack_64f(C11, H12, temp, fftLen);
	// temp2 = C12.*H22
	ippsMulPack_64f(C12, H22, temp2, fftLen);
	// temp2 = C11.*H12+C12.*H22
	ippsAdd_64f_I(temp, temp2, fftLen);
	// temp = (C11.*H12+C12.*H22).*X2BUFF
	ippsMulPack_64f(temp2, X2BUFF, temp, fftLen);
	// Y1BUFF = (C11.*H11 + C12.*H21).*X1BUFF + (C11.*H12+C12.*H22).*X2BUFF
	ippsAdd_64f(temp3, temp, Y1BUFF, fftLen);

	// temp = C21.*H11
	ippsMulPack_64f(C21, H11, temp, fftLen);
	// temp2 = C22.*H21
	ippsMulPack_64f(C22, H21, temp2, fftLen);
	// temp2 = C21.*H11+C22.*H21
	ippsAdd_64f_I(temp, temp2, fftLen);
	// temp3 = (C21.*H11+C22.*H21).*X1BUFF
	ippsMulPack_64f(temp2, X1BUFF, temp3, fftLen);
	// temp = C21.*H12
	ippsMulPack_64f(C21, H12, temp, fftLen);
	// temp2 = C22.*H22
	ippsMulPack_64f(C22, H22, temp2, fftLen);
	// temp2 = C21.*H12+C22.*H22
	ippsAdd_64f_I(temp, temp2, fftLen);
	// temp = (C21.*H12+C22.*H22).*X2BUFF
	ippsMulPack_64f(temp2, X2BUFF, temp, fftLen);
	// Y2BUFF = (C21.*H11+C22.*H21).*X1BUFF + (C21.*H12+C22.*H22).*X2BUFF
	ippsAdd_64f(temp3, temp, Y2BUFF, fftLen);

	ippsFFTInv_PackToR_64f(Y1BUFF, y1buff, fftState, pBuffer);
	ippsFFTInv_PackToR_64f(Y2BUFF, y2buff, fftState, pBuffer);
	// get only the real values
	for (int i = 1; i < fftLen / 2; i++)
	{
		y1buff[i] = y1buff[int(i * 2)];
		y2buff[i] = y2buff[int(i * 2)];
	}
	*/
	// temp = C11.*H11
	ippsMul_64fc(C11, H11, temp, fftLen);
	// temp2 = C12.*H21
	ippsMul_64fc(C12, H21, temp2, fftLen);
	// temp2 = C11.*H11+C12.*H21
	ippsAdd_64fc_I(temp, temp2, fftLen);
	// temp3 = (C11.*H11 + C12.*H21).*X1BUFF
	ippsMul_64fc(temp2, X1BUFF, temp3, fftLen);
	// temp = C11.*H12
	ippsMul_64fc(C11, H12, temp, fftLen);
	// temp2 = C12.*H22
	ippsMul_64fc(C12, H22, temp2, fftLen);
	// temp2 = C11.*H12+C12.*H22
	ippsAdd_64fc_I(temp, temp2, fftLen);
	// temp = (C11.*H12+C12.*H22).*X2BUFF
	ippsMul_64fc(temp2, X2BUFF, temp, fftLen);
	// Y1BUFF = (C11.*H11 + C12.*H21).*X1BUFF + (C11.*H12+C12.*H22).*X2BUFF
	ippsAdd_64fc(temp3, temp, Y1BUFF, fftLen);

	// temp = C21.*H11
	ippsMul_64fc(C21, H11, temp, fftLen);
	// temp2 = C22.*H21
	ippsMul_64fc(C22, H21, temp2, fftLen);
	// temp2 = C21.*H11+C22.*H21
	ippsAdd_64fc_I(temp, temp2, fftLen);
	// temp3 = (C21.*H11+C22.*H21).*X1BUFF
	ippsMul_64fc(temp2, X1BUFF, temp3, fftLen);
	// temp = C21.*H12
	ippsMul_64fc(C21, H12, temp, fftLen);
	// temp2 = C22.*H22
	ippsMul_64fc(C22, H22, temp2, fftLen);
	// temp2 = C21.*H12+C22.*H22
	ippsAdd_64fc_I(temp, temp2, fftLen);
	// temp = (C21.*H12+C22.*H22).*X2BUFF
	ippsMul_64fc(temp2, X2BUFF, temp, fftLen);
	// Y2BUFF = (C21.*H11+C22.*H21).*X1BUFF + (C21.*H12+C22.*H22).*X2BUFF
	ippsAdd_64fc(temp3, temp, Y2BUFF, fftLen);

	//ippsFFTInv_PackToR_64f(Y1BUFF, y1buff, fftState, pBuffer);
	//ippsFFTInv_PackToR_64f(Y2BUFF, y2buff, fftState, pBuffer);

	ippsFFTInv_CToC_64fc(Y1BUFF, y1buff, pSpec, pMemBuffer);
	ippsFFTInv_CToC_64fc(Y2BUFF, y2buff, pSpec, pMemBuffer);
	// get only the real values
	/*
	for (int i = 1; i < fftLen / 2; i++)
	{
		y1buff[i] = y1buff[int(i * 2)];
		y2buff[i] = y2buff[int(i * 2)];
	}
	*/
	// output update
	// multiply each element of the vector y1buff by 32768 and store the result in y1buff
	ippsMulC_64fc_I({ 32768.0, 0.0 }, y1buff, fs);
	// multiply each element of the vector y2buff by 32768 and store the result in y2buff
	ippsMulC_64fc_I({ 32768.0, 0.0 }, y2buff, fs);
	//ippsCopy_64f(y1buff + M - 1, OutputData1, L);
	//ippsCopy_64f(y2buff + M - 1, OutputData2, L);
	for (int i = 0; i < L; i++)
	{
		OutputData1[i] = y1buff[M - 1 + i].re;
		OutputData2[i] = y2buff[M - 1 + i].re;
	}
	ippsMove_64fc(x1buff + L, x1buff, M - 1);
	ippsMove_64fc(x2buff + L, x2buff, M - 1);

	return COMPLETED;
}

void __stdcall PlugIn::LEPlugin_Init()
{
	isRunning = true;

	if (x1buff == 0)
	{
		x1buff = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(x1buff, fftLen);
	}

	if (x2buff == 0)
	{
		x2buff = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(x2buff, fftLen);
	}

	if (X1BUFF == 0)
	{
		X1BUFF = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(X1BUFF, fftLen);
	}

	if (X2BUFF == 0)
	{
		X2BUFF = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(X2BUFF, fftLen);
	}

	if (y1buff == 0)
	{
		y1buff = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(y1buff, fftLen);
	}

	if (y2buff == 0)
	{
		y2buff = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(y2buff, fftLen);
	}

	if (Y1BUFF == 0)
	{
		Y1BUFF = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(Y1BUFF, fftLen);
	}

	if (Y2BUFF == 0)
	{
		Y2BUFF = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(Y2BUFF, fftLen);
	}

	if (temp == 0)
	{
		temp = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(temp, fftLen);
	}

	if (temp2 == 0)
	{
		temp2 = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(temp2, fftLen);
	}

	if (temp3 == 0)
	{
		temp3 = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(temp3, fftLen);
	}

	if (c11 == 0)
	{
		c11 = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(c11, fftLen);
	}

	if (c12 == 0)
	{
		c12 = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(c12, fftLen);
	}

	if (c21 == 0)
	{
		c21 = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(c21, fftLen);
	}

	if (c22 == 0)
	{
		c22 = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(c22, fftLen);
	}

	if (C11 == 0)
	{
		C11 = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(C11, fftLen);
	}

	if (C12 == 0)
	{
		C12 = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(C12, fftLen);
	}

	if (C21 == 0)
	{
		C21 = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(C21, fftLen);
	}

	if (C22 == 0)
	{
		C22 = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(C22, fftLen);
	}

	if (H11 == 0)
	{
		H11 = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(H11, fftLen);
	}

	if (H12 == 0)
	{
		H12 = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(H12, fftLen);
	}

	if (H21 == 0)
	{
		H21 = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(H21, fftLen);
	}

	if (H22 == 0)
	{
		H22 = ippsMalloc_64fc(fftLen);
		ippsZero_64fc(H22, fftLen);
	}

	if (hrir == 0)
	{
		hrir = ippsMalloc_64f(M);
		ippsZero_64f(hrir, M);
	}
	// load filter taps
	read_dat("c11.dat", hrir, M);
	for (int i = 0; i < M; i++)
		c11[i].re = hrir[i];

	read_dat("c12.dat", hrir, M);
	for (int i = 0; i < M; i++)
		c12[i].re = hrir[i];

	read_dat("c21.dat", hrir, M);
	for (int i = 0; i < M; i++)
		c21[i].re = hrir[i];

	read_dat("c22.dat", hrir, M);
	for (int i = 0; i < M; i++)
		c22[i].re = hrir[i];

	// allocation of variables for FFT PACK or CCS FORMAT
	/*
	status = ippsFFTGetSize_R_64f(fftOrd, IPP_FFT_DIV_INV_BY_N, ippAlgHintNone, &sizeSpec, &sizeInit, &sizeBuf);
	pSpecMem = ippsMalloc_8u(sizeSpec);
	pBuffer = ippsMalloc_8u(sizeBuf);
	pmemInit = ippsMalloc_8u(sizeInit);
	ippsFFTInit_R_64f(&fftState, fftOrd, IPP_FFT_DIV_INV_BY_N, ippAlgHintNone, pSpecMem, pmemInit);
	*/

	/* FFT WITH CCS -> MALLOC fftLen + 2!
	ippsFFTFwd_RToCCS_64f(c11, C11, fftState, pBuffer);
	ippsFFTFwd_RToCCS_64f(c12, C12, fftState, pBuffer);
	ippsFFTFwd_RToCCS_64f(c21, C21, fftState, pBuffer);
	ippsFFTFwd_RToCCS_64f(c22, C22, fftState, pBuffer);
	*/

	/// get sizes for required buffers
	ippsFFTGetSize_C_64fc(fftOrd, IPP_FFT_DIV_INV_BY_N, ippAlgHintNone, &sizeSpec, &sizeInit, &sizeBuffer);

	/// allocate memory for required buffers
	pMemSpec = (Ipp8u*)ippMalloc(sizeSpec);

	//if (sizeInit > 0)
	//{
	pMemInit = (Ipp8u*)ippMalloc(sizeInit);
	//}

	//if (sizeBuffer > 0)
	//{
	pMemBuffer = (Ipp8u*)ippMalloc(sizeBuffer);
	//}

	ippsFFTInit_C_64fc(&pSpec, fftOrd, IPP_FFT_DIV_INV_BY_N, ippAlgHintNone, pMemSpec, pMemInit);

	ippsFFTFwd_CToC_64fc(c11, C11, pSpec, pMemBuffer);
	ippsFFTFwd_CToC_64fc(c12, C12, pSpec, pMemBuffer);
	ippsFFTFwd_CToC_64fc(c21, C21, pSpec, pMemBuffer);
	ippsFFTFwd_CToC_64fc(c22, C22, pSpec, pMemBuffer);

	/*
	ippsFFTFwd_RToPack_64f(c11, C11, fftState, pBuffer);
	ippsFFTFwd_RToPack_64f(c12, C12, fftState, pBuffer);
	ippsFFTFwd_RToPack_64f(c21, C21, fftState, pBuffer);
	ippsFFTFwd_RToPack_64f(c22, C22, fftState, pBuffer);
	
	Ipp64fc* C11ext, *C12ext, *C21ext, *C22ext;
	C11ext = ippsMalloc_64fc(fftLen);
	C12ext = ippsMalloc_64fc(fftLen);
	C21ext = ippsMalloc_64fc(fftLen);
	C22ext = ippsMalloc_64fc(fftLen);
	ippsZero_64fc(C11ext, fftLen);
	ippsZero_64fc(C12ext, fftLen);
	ippsZero_64fc(C21ext, fftLen);
	ippsZero_64fc(C22ext, fftLen);
	ippsConjPack_64fc(C11, C11ext, fftLen);
	ippsConjPack_64fc(C12, C12ext, fftLen);
	ippsConjPack_64fc(C21, C21ext, fftLen);
	ippsConjPack_64fc(C22, C22ext, fftLen);
	*/
	
	/* with CCS format
	Cconj[0][0] = C11[0] + C11[1] * 1i;
	Cconj[0][1] = C12[0] + C12[1] * 1i;
	Cconj[1][0] = C21[0] + C21[1] * 1i;
	Cconj[1][1] = C22[0] + C22[1] * 1i;
	*/


	/*
	B[0][0].re = 1.0, B[0][0].im = 0.0;
	B[0][1].re = 0.0, B[0][1].im = 0.0;
	B[1][0].re = 0.0, B[1][0].im = 0.0;
	B[1][1].re = 1.0, B[1][1].im = 0.0;
	*/

	/*
	Ipp64fc* H11ext, * H12ext, * H21ext, * H22ext;
	H11ext = ippsMalloc_64fc(fftLen);
	H12ext = ippsMalloc_64fc(fftLen);
	H21ext = ippsMalloc_64fc(fftLen);
	H22ext = ippsMalloc_64fc(fftLen);
	ippsZero_64fc(H11ext, fftLen);
	ippsZero_64fc(H12ext, fftLen);
	ippsZero_64fc(H21ext, fftLen);
	ippsZero_64fc(H22ext, fftLen);
	*/

	for (int n = 0; n < fftLen; n++)
	{
		C[0][0] = C11[n];
		C[0][1] = C12[n];
		C[1][0] = C21[n];
		C[1][1] = C22[n];

		ippsConj_64fc(&(C[0][0]), &(Cconj[0][0]), 1);
		ippsConj_64fc(&(C[0][1]), &(Cconj[0][1]), 1);
		ippsConj_64fc(&(C[1][0]), &(Cconj[1][0]), 1);
		ippsConj_64fc(&(C[1][1]), &(Cconj[1][1]), 1);

		//Ctemp[0][0] = Cconj[0][0] * C[0][0] + Cconj[1][0] * C[1][0] + beta;
		ippsMul_64fc(&(Cconj[0][0]), &(C[0][0]), &(Ctemp[0][0]), 1);
		ippsMul_64fc(&(Cconj[1][0]), &(C[1][0]), &(Ctemp2[0][0]), 1);
		ippsAdd_64fc_I(&(Ctemp2[0][0]), &(Ctemp[0][0]), 1);
		ippsAddC_64fc_I({ beta, 0 }, &(Ctemp[0][0]), 1);
		//Ctemp[0][1] = Cconj[0][0] * C[0][1] + Cconj[1][0] * C[1][1];
		ippsMul_64fc(&(Cconj[0][0]), &(C[0][1]), &(Ctemp[0][1]), 1);
		ippsMul_64fc(&(Cconj[1][0]), &(C[1][1]), &(Ctemp2[0][1]), 1);
		ippsAdd_64fc_I(&(Ctemp2[0][1]), &(Ctemp[0][1]), 1);
		//Ctemp[1][0] = Cconj[0][1] * C[0][0] + Cconj[1][1] * C[1][0];
		ippsMul_64fc(&(Cconj[0][1]), &(C[0][0]), &(Ctemp[1][0]), 1);
		ippsMul_64fc(&(Cconj[1][1]), &(C[1][0]), &(Ctemp2[1][0]), 1);
		ippsAdd_64fc_I(&(Ctemp2[1][0]), &(Ctemp[1][0]), 1);
		//Ctemp[1][1] = Cconj[0][1] * C[0][1] + Cconj[1][1] * C[1][1] + beta;
		ippsMul_64fc(&(Cconj[0][1]), &(C[0][1]), &(Ctemp[1][1]), 1);
		ippsMul_64fc(&(Cconj[1][1]), &(C[1][1]), &(Ctemp2[1][1]), 1);
		ippsAdd_64fc_I(&(Ctemp2[1][1]), &(Ctemp[1][1]), 1);
		ippsAddC_64fc_I({ beta, 0 }, &(Ctemp[1][1]), 1);

		// det = Ctemp[0][0] * Ctemp[1][1] - Ctemp[0][1] * Ctemp[1][0];
		ippsMul_64fc(&(Ctemp[0][0]), &(Ctemp[1][1]), &det, 1);
		ippsMul_64fc(&(Ctemp[0][1]), &(Ctemp[1][0]), &invtemp, 1);
		ippsSub_64fc_I(&invtemp, &det, 1);

		invtemp = Ctemp[0][0];
		//Ctemp[0][0] = Ctemp[1][1] / det;
		ippsDivC_64fc(&(Ctemp[1][1]), det, &(Ctemp[0][0]), 1);
		//Ctemp[1][1] = invtemp / det;
		ippsDivC_64fc(&invtemp, det, &(Ctemp[1][1]), 1);
		//Ctemp[0][1] = -Ctemp[0][1] / det;
		ippsMulC_64fc_I({ -1.0, 0.0 }, &det, 1);
		ippsDivC_64fc_I(det, &(Ctemp[0][1]), 1);
		//Ctemp[1][0] = -Ctemp[1][0] / det;
		ippsDivC_64fc_I(det, &(Ctemp[1][0]), 1);

		//invtemp = Ctemp[0][0] * Cconj[0][0] + Ctemp[0][1] * Cconj[0][1];
		ippsMul_64fc(&(Ctemp[0][0]), &(Cconj[0][0]), &invtemp2, 1);
		ippsMul_64fc(&(Ctemp[0][1]), &(Cconj[0][1]), &invtemp, 1);
		ippsAdd_64fc_I(&invtemp2, &invtemp, 1);
		H11[n].re = invtemp.re;
		H11[n].im = invtemp.im;
		//invtemp = Ctemp[0][0] * Cconj[1][0] + Ctemp[0][1] * Cconj[1][1];
		ippsMul_64fc(&(Ctemp[0][0]), &(Cconj[1][0]), &invtemp2, 1);
		ippsMul_64fc(&(Ctemp[0][1]), &(Cconj[1][1]), &invtemp, 1);
		ippsAdd_64fc_I(&invtemp2, &invtemp, 1);
		H12[n].re = invtemp.re;
		H12[n].im = invtemp.im;
		//invtemp = Ctemp[1][0] * Cconj[0][0] + Ctemp[1][1] * Cconj[0][1];
		ippsMul_64fc(&(Ctemp[1][0]), &(Cconj[0][0]), &invtemp2, 1);
		ippsMul_64fc(&(Ctemp[1][1]), &(Cconj[0][1]), &invtemp, 1);
		ippsAdd_64fc_I(&invtemp2, &invtemp, 1);
		H21[n].re = invtemp.re;
		H21[n].im = invtemp.im;
		//invtemp = Ctemp[1][0] * Cconj[1][0] + Ctemp[1][1] * Cconj[1][1];
		ippsMul_64fc(&(Ctemp[1][0]), &(Cconj[1][0]), &invtemp2, 1);
		ippsMul_64fc(&(Ctemp[1][1]), &(Cconj[1][1]), &invtemp, 1);
		ippsAdd_64fc_I(&invtemp2, &invtemp, 1);
		H22[n].re = invtemp.re;
		H22[n].im = invtemp.im;
	}

	// extended format to pack format
	/*
	H11[0] = H11ext[0].re;
	H12[0] = H12ext[0].re;
	H21[0] = H21ext[0].re;
	H22[0] = H22ext[0].re;

	int n = 1;
	for (int i = 1; i <= int(fftLen/2); i ++)
	{
		H11[n] = H11ext[i].re;
		H12[n] = H12ext[i].re;
		H21[n] = H21ext[i].re;
		H22[n] = H22ext[i].re;
		H11[n + 1] = H11ext[i].im;
		H12[n + 1] = H12ext[i].im;
		H21[n + 1] = H21ext[i].im;
		H22[n + 1] = H22ext[i].im;
		n = n + 2;
	}
	*/

	/*********************+ CCS FORMAT ****************************/
	/*
	for (int n = 2; n < fftLen; n = n + 2) 
	{
		C[0][0] = C11[n] + C11[n + 1] * 1i;
		C[0][1] = C12[n] + C12[n + 1] * 1i;
		C[1][0] = C21[n] + C21[n + 1] * 1i;
		C[1][1] = C22[n] + C22[n + 1] * 1i;

		Ctemp[0][0] = Cconj[0][0] * C[0][0] + Cconj[1][0] * C[1][0] + beta;
		Ctemp[0][1] = Cconj[0][0] * C[0][1] + Cconj[1][0] * C[1][1];
		Ctemp[1][0] = Cconj[0][1] * C[0][0] + Cconj[1][1] * C[1][0];
		Ctemp[1][1] = Cconj[0][1] * C[0][1] + Cconj[1][1] * C[1][1] + beta;

		det = Ctemp[0][0] * Ctemp[1][1] - Ctemp[0][1] * Ctemp[1][0];

		invtemp = Ctemp[0][0];
		Ctemp[0][0] = Ctemp[1][1] / det;
		Ctemp[1][1] = invtemp / det;
		Ctemp[0][1] = -Ctemp[0][1] / det;
		Ctemp[1][0] = -Ctemp[1][0] / det;

		invtemp = Ctemp[0][0] * Cconj[0][0] + Ctemp[0][1] * Cconj[0][1];
		H11[n] = invtemp.real();
		H11[n + 1] = invtemp.imag();
		invtemp = Ctemp[0][0] * Cconj[1][0] + Ctemp[0][1] * Cconj[1][1];
		H12[n] = invtemp.real();
		H12[n + 1] = invtemp.imag();
		invtemp = Ctemp[1][0] * Cconj[0][0] + Ctemp[1][1] * Cconj[0][1];
		H21[n] = invtemp.real();
		H21[n + 1] = invtemp.imag();
		invtemp = Ctemp[1][0] * Cconj[1][0] + Ctemp[1][1] * Cconj[1][1];
		H22[n] = invtemp.real();
		H22[n + 1] = invtemp.imag();

		Cconj[0][0] = C[0][0];
		Cconj[0][1] = C[0][1];
		Cconj[1][0] = C[1][0];
		Cconj[1][1] = C[1][1];
	}
	*/

	// CCS format to Pack format
	/*
	ippsMove_64f(C11 + 2, C11 + 1, fftLen);
	ippsMove_64f(C12 + 2, C12 + 1, fftLen);
	ippsMove_64f(C21 + 2, C21 + 1, fftLen);
	ippsMove_64f(C22 + 2, C22 + 1, fftLen);

	ippsMove_64f(H11 + 2, H11 + 1, fftLen);
	ippsMove_64f(H12 + 2, H12 + 1, fftLen);
	ippsMove_64f(H21 + 2, H21 + 1, fftLen);
	ippsMove_64f(H22 + 2, H22 + 1, fftLen);
	*/

	/*
	ippsFree(C11ext);
	ippsFree(C12ext);
	ippsFree(C21ext);
	ippsFree(C22ext);


	ippsFree(H11ext);
	ippsFree(H12ext);
	ippsFree(H21ext);
	ippsFree(H22ext);
	*/
}

void __stdcall PlugIn::LEPlugin_Delete()
{
	isRunning = false;

	if (x1buff != 0)
	{
		ippsFree(x1buff);
		x1buff = 0;
	}

	if (x2buff != 0)
	{
		ippsFree(x2buff);
		x2buff = 0;
	}

	if (X1BUFF != 0)
	{
		ippsFree(X1BUFF);
		X1BUFF = 0;
	}

	if (X2BUFF != 0)
	{
		ippsFree(X2BUFF);
		X2BUFF = 0;
	}

	if (y1buff != 0)
	{
		ippsFree(y1buff);
		y1buff = 0;
	}

	if (y2buff != 0)
	{
		ippsFree(y2buff);
		y2buff = 0;
	}

	if (Y1BUFF != 0)
	{
		ippsFree(Y1BUFF);
		Y1BUFF = 0;
	}

	if (Y2BUFF != 0)
	{
		ippsFree(Y2BUFF);
		Y2BUFF = 0;
	}

	if (temp != 0)
	{
		ippsFree(temp);
		temp = 0;
	}

	if (temp2 != 0)
	{
		ippsFree(temp2);
		temp2 = 0;
	}

	if (temp3 != 0)
	{
		ippsFree(temp3);
		temp3 = 0;
	}

	if (c11 != 0)
	{
		ippsFree(c11);
		c11 = 0;
	}

	if (c12 != 0)
	{
		ippsFree(c12);
		c12 = 0;
	}

	if (c21 != 0)
	{
		ippsFree(c21);
		c21 = 0;
	}

	if (c22 != 0)
	{
		ippsFree(c22);
		c22 = 0;
	}

	if (H11 != 0)
	{
		ippsFree(H11);
		H11 = 0;
	}

	if (H12 != 0)
	{
		ippsFree(H12);
		H12 = 0;
	}

	if (H21 != 0)
	{
		ippsFree(H21);
		H21 = 0;
	}

	if (H22 != 0)
	{
		ippsFree(H22);
		H22 = 0;
	}

	if (C11 != 0)
	{
		ippsFree(C11);
		C11 = 0;
	}

	if (C12 != 0)
	{
		ippsFree(C12);
		C12 = 0;
	}

	if (C21 != 0)
	{
		ippsFree(C21);
		C21 = 0;
	}

	if (C22 != 0)
	{
		ippsFree(C22);
		C22 = 0;
	}

	if (pMemSpec != 0)
	{
		ippsFree(pMemSpec);
		pMemSpec = 0;
	}
	
	if (pMemInit != 0)
	{
		ippsFree(pMemInit);
		pMemInit = 0;
	}

	if (pMemBuffer != 0)
	{
		ippsFree(pMemBuffer);
		pMemBuffer = 0;
	}
	/*
	if (pSpecMem != 0)
	{
		ippsFree(pSpecMem);
		pSpecMem = 0;
	}

	if (pBuffer != 0)
	{
		ippsFree(pBuffer);
		pBuffer = 0;
	}

	if (pmemInit != 0)
	{
		ippsFree(pmemInit);
		pmemInit = 0;
	}
	*/
}

PlugIn::~PlugIn(void)
{

}


bool __stdcall PlugIn::LEInfoIO(int index,int type, char *StrInfo)
{
	if ((type == INPUT)) sprintf(StrInfo, "FilterIn[%d]", index);
	if ((type == OUTPUT)) sprintf(StrInfo, "FilterOut[%d]", index);
	return true;
}

int __stdcall PlugIn::LESetDefPin(int index,int type, PinType *Info)
{
	if (type==OUTPUT) 
	{
		Info->DataType=PLAYBUFFER;
		Info->Exclusive=true; 
		Info->DataLen=FrameSize;
		Info->MaxDataLen=FrameSize;
		return OUTPUT; 
	}

	if (type==INPUT) 
	{
		Info->DataType=PLAYBUFFER;
		Info->Exclusive=true; 
		Info->DataLen=FrameSize;
		Info->MaxDataLen=FrameSize;
		return INPUT; 
	}

	return -1;
}

int  __stdcall PlugIn::LEConnectionRequest(int IOType,int Index,PinType *NewType)
{
	return 0;
}

LPVOID __stdcall PlugIn::LEOnNUTechMessage(int MessageType,int MessageID,WPARAM wParam,LPARAM lParam)
{
	return 0;
}


void __stdcall PlugIn::LESetName(char *Name)
{
	strncpy(Name, NUTS_NAME, MAXNAME);
}

void __stdcall PlugIn::LESetParameter(int Index,void *Data,LPVOID bBroadCastInfo)
{

	CBFunction(this, NUTS_GETSECURETIME, NUTSSECURE, 0);

	if(Index == ID_BETA)
	{
		if (!isRunning)
		{
			memcpy(&beta, (double*)Data, sizeof(double));
			CBFunction(this, NUTS_UPDATERTWATCH, ID_BETA, 0);
		}
		
	}

	CBFunction(this, NUTS_RELEASESECURETIME, NUTSSECURE, 0);
}

int  __stdcall PlugIn::LEGetParameter(int Index,void *Data)
{

	CBFunction(this, NUTS_GETSECURETIME, NUTSSECURE, 0);

	if (Index == ID_BETA)
	{
		memcpy((double*)Data, &beta, sizeof(double));
	}

	CBFunction(this, NUTS_RELEASESECURETIME, NUTSSECURE, 0);
	return 0;
}

void __stdcall PlugIn::LESaveSetUp()
{
	
}

void __stdcall PlugIn::LELoadSetUp()
{

}

void __stdcall PlugIn::LERTWatchInit()
{
	WatchType NewWatch;

	memset(&NewWatch, 0, sizeof(WatchType));
	NewWatch.EnableWrite = true;
	NewWatch.LenByte = sizeof(double);
	NewWatch.TypeVar = WTC_DOUBLE;
	NewWatch.IDVar = ID_BETA;
	sprintf(NewWatch.VarName, "Beta\0");
	CBFunction(this, NUTS_ADDRTWATCH, 0, &NewWatch);
}

void __stdcall PlugIn::LESampleRateChange(int NewVal,int TrigType)
{
	if(TrigType==AUDIOPROC)
	{
		if(NewVal!=SampleRate)
		{
			SampleRate = NewVal;
		}
	}

} 

void __stdcall PlugIn::LEFrameSizeChange (int NewVal,int TrigType)
{
	if(TrigType==AUDIOPROC)
	{
		if(NewVal!=FrameSize)
		{
			FrameSize = NewVal;
		}
	}
} 

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////     LOADER      ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

LEEffect * __stdcall LoadEffect(InterfaceType _CBFunction,void *PlugRef,HWND ParentDlg)
{
	return new PlugIn(_CBFunction,PlugRef,ParentDlg);
}

int __stdcall UnLoadEffect(PlugIn *effect)
{
	delete effect;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////     GetStartUpInfo      //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void __stdcall LENUTSDefProps(char *NameEffect,int *Width, void *data)
{
	
	strncpy(NameEffect,NUTS_NAME,MAXNAME);
	*Width=WIDTHDEF;

	if (data!=0)
	{
		StartUpNUTSProps *Info=(StartUpNUTSProps *)data;
		Info->NumInStartUp=2;
		Info->NumOutStartUp=2;
		Info->BitMaskProc = AUDIOPROC;
		Info->BitMaskDriver = OFFLINEDRIVER | DIRECTDRIVER | ASIODRIVER;
	}
}
