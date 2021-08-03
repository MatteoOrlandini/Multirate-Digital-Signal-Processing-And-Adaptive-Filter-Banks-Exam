#include "StdAfx.h"
#include ".\plugin.h"


PlugIn::PlugIn(InterfaceType _CBFunction,void * _PlugRef,HWND ParentDlg): LEEffect(_CBFunction,_PlugRef,ParentDlg)
{
	FrameSize = CBFunction(this,NUTS_GET_FS_SR,0,(LPVOID)AUDIOPROC);
	SampleRate = CBFunction(this,NUTS_GET_FS_SR,1,(LPVOID)AUDIOPROC);

	LESetNumInput(2);
	LESetNumOutput(2);
	
	M = 512;	// filter length
	tau = M;
	mu = 1e-4;

	y1 = 0;
	y2 = 0;

	x1 = 0;
	x2 = 0;

	x1buff = 0;
	x2buff = 0;
	
	e1 = 0;
	e2 = 0;

	d1 = 0;
	d2 = 0;

	c11 = 0;
	c12 = 0;
	c21 = 0;
	c22 = 0;

	h11 = 0;
	h12 = 0;
	h21 = 0;
	h22 = 0;
	
	r111 = 0;
	r112 = 0;
	r211 = 0;
	r212 = 0;
	r222 = 0;
	r221 = 0;
	r122 = 0;
	r121 = 0;

	r111buff = 0;
	r112buff = 0;
	r121buff = 0;
	r122buff = 0;
	r211buff = 0;
	r212buff = 0;
	r221buff = 0;
	r222buff = 0;

	bufferNumber = 0;
}

int __stdcall PlugIn::LEPlugin_Process(PinType **Input,PinType **Output,LPVOID ExtraInfo)
{ 
	double* InputData1 = ((double*)Input[0]->DataBuffer);
	double* OutputData1 = ((double*)Output[0]->DataBuffer);
	double* InputData2 = ((double*)Input[1]->DataBuffer);
	double* OutputData2 = ((double*)Output[1]->DataBuffer);
	
	bufferNumber++;

	// d1[0:tau] = x1[end - tau:end];
	ippsCopy_64f(x1 + FrameSize - tau, d1, tau);
	// copy InputData1 to x1
	ippsCopy_64f(InputData1, x1, FrameSize);
	// divide each element of the vector x1 by 32768 and store the result in x1
	ippsDivC_64f_I(32768.0, x1, FrameSize);
	//d1[tau + 1:end] = x1[0:FrameSize - tau];	
	ippsCopy_64f(x1, d1 + tau, FrameSize - tau);
	// d2[0:tau] = x2[end - tau:end];
	ippsCopy_64f(x2 + FrameSize - tau, d2, tau);
	// copy InputData2 to x2
	ippsCopy_64f(InputData2, x2, FrameSize);
	// divide each element of the vector x2 by 32768 and store the result in x2
	ippsDivC_64f_I(32768.0, x2, FrameSize);
	//d2[tau + 1:end] = x2[0:FrameSize - tau];	
	ippsCopy_64f(x2, d2 + tau, FrameSize - tau);
	
	// initialize y1 and y2 with zeros
	ippsZero_64f(y1, FrameSize);
	ippsZero_64f(y2, FrameSize);
	/*
	// filter x1 with c11 and store the result in r111
	ippsFIRSR_64f(x1, r111, FrameSize, pSpecc11, dlyc11, dlyc11, bufc11);
	ippsZero_64f(dlyc11, M - 1);
	// filter x1 with c12 and store the result in r112
	ippsFIRSR_64f(x1, r112, FrameSize, pSpecc12, dlyc12, dlyc12, bufc12);
	ippsZero_64f(dlyc12, M - 1);
	// filter x2 with c11 and store the result in r211
	ippsFIRSR_64f(x2, r211, FrameSize, pSpecc11, dlyc11, dlyc11, bufc11);
	ippsZero_64f(dlyc11, M - 1);
	// filter x2 with c12 and store the result in r212
	ippsFIRSR_64f(x2, r212, FrameSize, pSpecc12, dlyc12, dlyc12, bufc12);
	ippsZero_64f(dlyc12, M - 1);

	// filter x2 with c22 and store the result in r222
	ippsFIRSR_64f(x2, r222, FrameSize, pSpecc22, dlyc22, dlyc22, bufc22);
	ippsZero_64f(dlyc22, M - 1);
	// filter x2 with c21 and store the result in r221
	ippsFIRSR_64f(x2, r221, FrameSize, pSpecc21, dlyc21, dlyc21, bufc21);
	ippsZero_64f(dlyc21, M - 1);
	// filter x1 with c22 and store the result in r122
	ippsFIRSR_64f(x1, r122, FrameSize, pSpecc22, dlyc22, dlyc22, bufc22);
	ippsZero_64f(dlyc22, M - 1);
	// filter x1 with c21 and store the result in r121
	ippsFIRSR_64f(x1, r121, FrameSize, pSpecc21, dlyc21, dlyc21, bufc21);
	ippsZero_64f(dlyc21, M - 1);
	*/
	for (int i = 0; i < FrameSize; i++)
	{
		ippsMove_64f(x1buff, x1buff + 1, M - 1);
		x1buff[0] = x1[i];

		ippsMove_64f(x2buff, x2buff + 1, M - 1);
		x2buff[0] = x2[i];

		ippsDotProd_64f(c11, x1buff, M, &(r111[i]));
		ippsDotProd_64f(c12, x1buff, M, &(r112[i]));
		ippsDotProd_64f(c11, x2buff, M, &(r211[i]));
		ippsDotProd_64f(c12, x2buff, M, &(r212[i]));
		ippsDotProd_64f(c22, x2buff, M, &(r222[i]));
		ippsDotProd_64f(c21, x2buff, M, &(r221[i]));
		ippsDotProd_64f(c22, x1buff, M, &(r122[i]));
		ippsDotProd_64f(c21, x1buff, M, &(r121[i]));

		ippsMove_64f(r111buff, r111buff + 1, M - 1);
		r111buff[0] = r111[i];

		ippsMove_64f(r112buff, r112buff + 1, M - 1);
		r112buff[0] = r112[i];

		ippsMove_64f(r121buff, r121buff + 1, M - 1);
		r121buff[0] = r121[i];

		ippsMove_64f(r122buff, r122buff + 1, M - 1);
		r122buff[0] = r122[i];

		ippsMove_64f(r211buff, r211buff + 1, M - 1);
		r211buff[0] = r211[i];

		ippsMove_64f(r212buff, r212buff + 1, M - 1);
		r212buff[0] = r212[i];

		ippsMove_64f(r221buff, r221buff + 1, M - 1);
		r221buff[0] = r221[i];

		ippsMove_64f(r222buff, r222buff + 1, M - 1);
		r222buff[0] = r222[i];

		double ytmp = 0.0;
		ippsDotProd_64f(h11, r111buff, M, y1 + i);
		// y1[i] = y1[i] + ytmp;

		ippsDotProd_64f(h21, r112buff, M, &ytmp);
		y1[i] = y1[i] + ytmp;
		//ippsAddC_64f_I(ytmp, y1 + i, 1);

		ippsDotProd_64f(h12, r211buff, M, &ytmp);
		y1[i] = y1[i] + ytmp;
		//ippsAddC_64f_I(ytmp, y1 + i, 1);

		ippsDotProd_64f(h22, r212buff, M, &ytmp);
		y1[i] = y1[i] + ytmp;
		//ippsAddC_64f_I(ytmp, y1 + i, 1);

		ippsDotProd_64f(h11, r121buff, M, y2 + i);
		//y2[i] = y2[i] + ytmp;

		ippsDotProd_64f(h21, r122buff, M, &ytmp);
		y2[i] = y2[i] + ytmp;
		//ippsAddC_64f_I(ytmp, y2 + i, 1);

		ippsDotProd_64f(h12, r221buff, M, &ytmp);
		y2[i] = y2[i] + ytmp;
		//ippsAddC_64f_I(ytmp, y2 + i, 1);

		ippsDotProd_64f(h22, r222buff, M, &ytmp);
		y2[i] = y2[i] + ytmp;
		//ippsAddC_64f_I(ytmp, y2 + i, 1);

		// e1 = d1 - y1
		//ippsSub_64f(y1 + i, d1 + i, e1 + i, 1);
		// e2 = d2 - y2
		//ippsSub_64f(y2 + i, d2 + i, e2 + i, 1);
		e1[i] = d1[i] - y1[i];
		e2[i] = d2[i] - y2[i];

		if (y1[i] > 1.0)
			printf("\n"); // MATLAB y1(40*4096+297)

		if (y2[i] > 1.0)
			printf("\n");

		for (int j = 0; j < M; j++)
		{
			h11[j] = h11[j] + mu * (e1[i] * r111buff[j] + e2[i] * r121buff[j]);
			h12[j] = h12[j] + mu * (e1[i] * r211buff[j] + e2[i] * r221buff[j]);
			h21[j] = h21[j] + mu * (e1[i] * r112buff[j] + e2[i] * r122buff[j]);
			h22[j] = h22[j] + mu * (e1[i] * r212buff[j] + e2[i] * r222buff[j]);
		}	
	}
	// multiply each element of the vector y1 by a 32768 and store the result in y1
	ippsMulC_64f_I(32768.0, y1, FrameSize);
	// copy y1 to OutputData1
	ippsCopy_64f(y1, OutputData1, FrameSize);
	// multiply each element of the vector y2 by a 32768 and store the result in y2
	ippsMulC_64f_I(32768.0, y2, FrameSize);
	// copy y2 to OutputData2
	ippsCopy_64f(y2, OutputData2, FrameSize);

	/*
	// filter r111 with h11 and store the result in ytmp
	ippsFIRSR_64f(r111, ytmp, FrameSize, pSpech11, dlyh11, dlyh11, bufh11);
	// y1 = y1 + ytmp
	ippsAdd_64f_I(ytmp, y1, FrameSize);
	// filter r112 with h21 and store the result in ytmp
	ippsFIRSR_64f(r112, ytmp, FrameSize, pSpech21, dlyh21, dlyh21, bufh21);
	// y1 = y1 + ytmp
	ippsAdd_64f_I(ytmp, y1, FrameSize);
	// filter r211 with h12 and store the result in ytmp
	ippsFIRSR_64f(r211, ytmp, FrameSize, pSpech12, dlyh12, dlyh12, bufh12);
	// y1 = y1 + ytmp
	ippsAdd_64f_I(ytmp, y1, FrameSize);
	// filter r212 with h22 and store the result in ytmp
	ippsFIRSR_64f(r212, ytmp, FrameSize, pSpech22, dlyh22, dlyh22, bufh22);
	// y1 = y1 + ytmp
	ippsAdd_64f_I(ytmp, y1, FrameSize);

	// filter r222 with h22 and store the result in ytmp
	ippsFIRSR_64f(r222, ytmp, FrameSize, pSpech22, dlyh22, dlyh22, bufh22);
	// y1 = y1 + ytmp
	ippsAdd_64f_I(ytmp, y2, FrameSize);
	// filter r221 with h12 and store the result in ytmp
	ippsFIRSR_64f(r221, ytmp, FrameSize, pSpech12, dlyh12, dlyh12, bufh12);
	// y1 = y1 + ytmp
	ippsAdd_64f_I(ytmp, y2, FrameSize);
	// filter r122 with h21 and store the result in ytmp
	ippsFIRSR_64f(r122, ytmp, FrameSize, pSpech21, dlyh21, dlyh21, bufh21);
	// y1 = y1 + ytmp
	ippsAdd_64f_I(ytmp, y2, FrameSize);
	// filter r121 with h11 and store the result in ytmp
	ippsFIRSR_64f(r121, ytmp, FrameSize, pSpech11, dlyh11, dlyh11, bufh11);
	// y1 = y1 + ytmp
	ippsAdd_64f_I(ytmp, y2, FrameSize);

	// e1 = d1 - y1
	ippsSub_64f(y1, d1, e1, FrameSize);
	// e2 = d2 - y2
	ippsSub_64f(y2, d2, e2, FrameSize);

	// update h11, h12, h21, h22 filters
	for (int i = 0; i < FrameSize; i++)
	{
		for (int j = 0; j < M; j++)
		{
			h11[j] = h11[j] + mu * (e1[i] * r111[j] + e2[i] * r121[j]);
			h12[j] = h12[j] + mu * (e1[i] * r211[j] + e2[i] * r221[j]);
			h21[j] = h21[j] + mu * (e1[i] * r112[j] + e2[i] * r122[j]);
			h22[j] = h22[j] + mu * (e1[i] * r212[j] + e2[i] * r222[j]);
		}
	}
	ippsFIRSRInit_64f(h11, M, ippAlgDirect, pSpech11);
	ippsFIRSRInit_64f(h12, M, ippAlgDirect, pSpech12);
	ippsFIRSRInit_64f(h21, M, ippAlgDirect, pSpech21);
	ippsFIRSRInit_64f(h22, M, ippAlgDirect, pSpech22);
	*/	
	return COMPLETED;
}

void __stdcall PlugIn::LEPlugin_Init()
{
	if (y1 == 0)
	{
		y1 = ippsMalloc_64f(FrameSize);
		ippsZero_64f(y1, FrameSize);
	}

	if (y2 == 0)
	{
		y2 = ippsMalloc_64f(FrameSize);
		ippsZero_64f(y2, FrameSize);
	}

	if (x1 == 0)
	{
		x1 = ippsMalloc_64f(FrameSize);
		ippsZero_64f(x1, FrameSize);
	}

	if (x2 == 0)
	{
		x2 = ippsMalloc_64f(FrameSize);
		ippsZero_64f(x2, FrameSize);
	}

	if (x1buff == 0)
	{
		x1buff = ippsMalloc_64f(M);
		ippsZero_64f(x1buff, M);
	}

	if (x2buff == 0)
	{
		x2buff = ippsMalloc_64f(M);
		ippsZero_64f(x2buff, M);
	}

	if (e1 == 0)
	{
		e1 = ippsMalloc_64f(FrameSize);
		ippsZero_64f(e1, FrameSize);
	}

	if (e2 == 0)
	{
		e2 = ippsMalloc_64f(FrameSize);
		ippsZero_64f(e2, FrameSize);
	}
	if (d1 == 0)
	{
		d1 = ippsMalloc_64f(FrameSize);
		ippsZero_64f(d1, FrameSize);
	}

	if (d2 == 0)
	{
		d2 = ippsMalloc_64f(FrameSize);
		ippsZero_64f(d2, FrameSize);
	}
	
	if (c11 == 0)
	{
		c11 = ippsMalloc_64f(M);
		ippsZero_64f(c11, M);
	}

	if (c12 == 0)
	{
		c12 = ippsMalloc_64f(M);
		ippsZero_64f(c12, M);
	}
	
	if (c21 == 0)
	{
		c21 = ippsMalloc_64f(M);
		ippsZero_64f(c21, M);
	}

	if (c22 == 0)
	{
		c22 = ippsMalloc_64f(M);
		ippsZero_64f(c22, M);
	}
	// load filter taps
	read_dat("c11.dat", c11, M);

	read_dat("c12.dat", c12, M);

	read_dat("c21.dat", c21, M);

	read_dat("c22.dat", c22, M);

	if (h11 == 0)
	{
		h11 = ippsMalloc_64f(M);
		ippsZero_64f(h11, M);
	}
	
	if (h12 == 0)
	{
		h12 = ippsMalloc_64f(M);
		ippsZero_64f(h12, M);
	}
	
	if (h21 == 0)
	{
		h21 = ippsMalloc_64f(M);
		ippsZero_64f(h21, M);
	}
	
	if (h22 == 0)
	{
		h22 = ippsMalloc_64f(M);
		ippsZero_64f(h22, M);
	}

	if (r111 == 0)
	{
		r111 = ippsMalloc_64f(FrameSize);
		ippsZero_64f(r111, FrameSize);
	}

	if (r112 == 0)
	{
		r112 = ippsMalloc_64f(FrameSize);
		ippsZero_64f(r112, FrameSize);
	}

	if (r211 == 0)
	{
		r211 = ippsMalloc_64f(FrameSize);
		ippsZero_64f(r211, FrameSize);
	}

	if (r212 == 0)
	{
		r212 = ippsMalloc_64f(FrameSize);
		ippsZero_64f(r212, FrameSize);
	}

	if (r222 == 0)
	{
		r222 = ippsMalloc_64f(FrameSize);
		ippsZero_64f(r222, FrameSize);
	}

	if (r221 == 0)
	{
		r221 = ippsMalloc_64f(FrameSize);
		ippsZero_64f(r221, FrameSize);
	}

	if (r122 == 0)
	{
		r122 = ippsMalloc_64f(FrameSize);
		ippsZero_64f(r122, FrameSize);
	}

	if (r121 == 0)
	{
		r121 = ippsMalloc_64f(FrameSize);
		ippsZero_64f(r121, FrameSize);
	}

	if (r111buff == 0)
	{
		r111buff = ippsMalloc_64f(M);
		ippsZero_64f(r111buff, M);
	}

	if (r112buff == 0)
	{
		r112buff = ippsMalloc_64f(M);
		ippsZero_64f(r112buff, M);
	}

	if (r211buff == 0)
	{
		r211buff = ippsMalloc_64f(M);
		ippsZero_64f(r211buff, M);
	}

	if (r212buff == 0)
	{
		r212buff = ippsMalloc_64f(M);
		ippsZero_64f(r212buff, M);
	}

	if (r222buff == 0)
	{
		r222buff = ippsMalloc_64f(M);
		ippsZero_64f(r222buff, M);
	}

	if (r221buff == 0)
	{
		r221buff = ippsMalloc_64f(M);
		ippsZero_64f(r221buff, M);
	}

	if (r122buff == 0)
	{
		r122buff = ippsMalloc_64f(M);
		ippsZero_64f(r122buff, M);
	}

	if (r121buff == 0)
	{
		r121buff = ippsMalloc_64f(M);
		ippsZero_64f(r121buff, M);
	}
	// initialization filtering structure
	/*
	ippsFIRSRGetSize(M, ipp64f, &specSizeh11, &bufSizeh11);
	ippsFIRSRGetSize(M, ipp64f, &specSizeh12, &bufSizeh12);
	ippsFIRSRGetSize(M, ipp64f, &specSizeh21, &bufSizeh21);
	ippsFIRSRGetSize(M, ipp64f, &specSizeh22, &bufSizeh22);
	*/
	/*
	ippsFIRSRGetSize(M, ipp64f, &specSizec11, &bufSizec11);
	ippsFIRSRGetSize(M, ipp64f, &specSizec12, &bufSizec12);
	ippsFIRSRGetSize(M, ipp64f, &specSizec21, &bufSizec21);
	ippsFIRSRGetSize(M, ipp64f, &specSizec22, &bufSizec22);
	*/
	/*
	dlyh11 = ippsMalloc_64f(M - 1);
	dlyh12 = ippsMalloc_64f(M - 1);
	dlyh21 = ippsMalloc_64f(M - 1);
	dlyh22 = ippsMalloc_64f(M - 1);
	*/
	/*
	dlyc11 = ippsMalloc_64f(M - 1);
	dlyc12 = ippsMalloc_64f(M - 1);
	dlyc21 = ippsMalloc_64f(M - 1);
	dlyc22 = ippsMalloc_64f(M - 1);
	*/
	/*
	ippsZero_64f(dlyh11, M - 1);
	ippsZero_64f(dlyh12, M - 1);
	ippsZero_64f(dlyh21, M - 1);
	ippsZero_64f(dlyh22, M - 1);
	*/
	/*
	ippsZero_64f(dlyc11, M - 1);
	ippsZero_64f(dlyc12, M - 1);
	ippsZero_64f(dlyc21, M - 1);
	ippsZero_64f(dlyc22, M - 1);
	*/
	/*
	pSpech11 = (IppsFIRSpec_64f*)ippsMalloc_8u(specSizeh11);
	pSpech12 = (IppsFIRSpec_64f*)ippsMalloc_8u(specSizeh12);
	pSpech21 = (IppsFIRSpec_64f*)ippsMalloc_8u(specSizeh21);
	pSpech22 = (IppsFIRSpec_64f*)ippsMalloc_8u(specSizeh22);
	*/
	/*
	pSpecc11 = (IppsFIRSpec_64f*)ippsMalloc_8u(specSizec11);
	pSpecc12 = (IppsFIRSpec_64f*)ippsMalloc_8u(specSizec12);
	pSpecc21 = (IppsFIRSpec_64f*)ippsMalloc_8u(specSizec21);
	pSpecc22 = (IppsFIRSpec_64f*)ippsMalloc_8u(specSizec22);
	*/
	/*
	bufh11 = ippsMalloc_8u(bufSizeh11);
	bufh12 = ippsMalloc_8u(bufSizeh12);
	bufh21 = ippsMalloc_8u(bufSizeh21);
	bufh22 = ippsMalloc_8u(bufSizeh22);
	*/
	/*
	bufc11 = ippsMalloc_8u(bufSizec11);
	bufc12 = ippsMalloc_8u(bufSizec12);
	bufc21 = ippsMalloc_8u(bufSizec21);
	bufc22 = ippsMalloc_8u(bufSizec22);
	*/
	/*
	ippsFIRSRInit_64f(h11, M, ippAlgDirect, pSpech11);
	ippsFIRSRInit_64f(h12, M, ippAlgDirect, pSpech12);
	ippsFIRSRInit_64f(h21, M, ippAlgDirect, pSpech21);
	ippsFIRSRInit_64f(h22, M, ippAlgDirect, pSpech22);
	*/
	/*
	ippsFIRSRInit_64f(c11, M, ippAlgDirect, pSpecc11);
	ippsFIRSRInit_64f(c12, M, ippAlgDirect, pSpecc12);
	ippsFIRSRInit_64f(c21, M, ippAlgDirect, pSpecc21);
	ippsFIRSRInit_64f(c22, M, ippAlgDirect, pSpecc22);
	*/
}

void __stdcall PlugIn::LEPlugin_Delete()
{
	if (y1 != 0)
	{
		ippsFree(y1);
		y1 = 0;
	}

	if (y2 != 0)
	{
		ippsFree(y2);
		y2 = 0;
	}

	if (x1 != 0)
	{
		ippsFree(x1);
		x1 = 0;
	}

	if (x2 != 0)
	{
		ippsFree(x2);
		x2 = 0;
	}

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

	if (e1 != 0)
	{
		ippsFree(e1);
		e1 = 0;
	}

	if (e2 != 0)
	{
		ippsFree(e2);
		e2 = 0;
	}

	if (d1 != 0)
	{
		ippsFree(d1);
		d1 = 0;
	}

	if (d2 != 0)
	{
		ippsFree(d2);
		d2 = 0;
	}

	if (h11 != 0)
	{
		ippsFree(h11);
		h11 = 0;
	}

	if (h12 != 0)
	{
		ippsFree(h12);
		h12 = 0;
	}

	if (h21 != 0)
	{
		ippsFree(h21);
		h21 = 0;
	}

	if (h22 != 0)
	{
		ippsFree(h22);
		h22 = 0;
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

	if (r111 != 0)
	{
		ippsFree(r111);
		r111 = 0;
	}

	if (r112 != 0)
	{
		ippsFree(r112);
		r112 = 0;
	}

	if (r121 != 0)
	{
		ippsFree(r121);
		r121 = 0;
	}

	if (r122 != 0)
	{
		ippsFree(r122);
		r122 = 0;
	}

	if (r211 != 0)
	{
		ippsFree(r211);
		r211 = 0;
	}

	if (r212 != 0)
	{
		ippsFree(r212);
		r212 = 0;
	}

	if (r221 != 0)
	{
		ippsFree(r221);
		r221 = 0;
	}

	if (r222 != 0)
	{
		ippsFree(r222);
		r222 = 0;
	}

	if (r111buff != 0)
	{
		ippsFree(r111buff);
		r111buff = 0;
	}

	if (r112buff != 0)
	{
		ippsFree(r112buff);
		r112buff = 0;
	}

	if (r121buff != 0)
	{
		ippsFree(r121buff);
		r121buff = 0;
	}

	if (r122buff != 0)
	{
		ippsFree(r122buff);
		r122buff = 0;
	}

	if (r211buff != 0)
	{
		ippsFree(r211buff);
		r211buff = 0;
	}

	if (r212buff != 0)
	{
		ippsFree(r212buff);
		r212buff = 0;
	}

	if (r221buff != 0)
	{
		ippsFree(r221buff);
		r221buff = 0;
	}

	if (r222buff != 0)
	{
		ippsFree(r222buff);
		r222buff = 0;
	}
	/*
	ippsFree(dlyh11);
	ippsFree(dlyh12);
	ippsFree(dlyh21);
	ippsFree(dlyh22);
	*/
	/*
	ippsFree(dlyc11);
	ippsFree(dlyc12);
	ippsFree(dlyc21);
	ippsFree(dlyc22);
	*/
	/*
	ippsFree(bufh11);
	ippsFree(bufh12);
	ippsFree(bufh21);
	ippsFree(bufh22);
	*/
	/*
	ippsFree(bufc11);
	ippsFree(bufc12);
	ippsFree(bufc21);
	ippsFree(bufc22);
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

}

int  __stdcall PlugIn::LEGetParameter(int Index,void *Data)
{
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
