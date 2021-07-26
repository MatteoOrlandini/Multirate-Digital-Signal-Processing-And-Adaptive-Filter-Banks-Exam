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
	ytmp = 0;

	x1 = 0;
	x2 = 0;

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
}

int __stdcall PlugIn::LEPlugin_Process(PinType **Input,PinType **Output,LPVOID ExtraInfo)
{ /*
	double* InputData1 = ((double*)Input[0]->DataBuffer);
	double* OutputData1 = ((double*)Output[0]->DataBuffer);
	double* InputData2 = ((double*)Input[1]->DataBuffer);
	double* OutputData2 = ((double*)Output[1]->DataBuffer);
	
	// d1[0:tau] = x1[end - tau:end];
	ippsCopy_64f(x1 + FrameSize - tau - 1, d1, tau);
	// copy InputData1 to x1
	ippsCopy_64f(InputData1, x1, FrameSize);
	//d1[tau + 1:end] = x1[0:FrameSize - tau];	
	ippsCopy_64f(x1, d1 + tau, FrameSize - tau);
	// d2[0:tau] = x2[end - tau:end];
	ippsCopy_64f(x2 + FrameSize - tau - 1, d2, tau);
	// copy InputData2 to x2
	ippsCopy_64f(InputData2, x2, FrameSize);
	//d2[tau + 1:end] = x2[0:FrameSize - tau];	
	ippsCopy_64f(x2, d2 + tau, FrameSize - tau);
	
	// initialize y1 and y2 with zeros
	ippsZero_64f(y1, FrameSize);
	ippsZero_64f(y2, FrameSize);

	// filter x1 with c11 and store the result in r111
	ippsFIRSR_64f(x1, r111, FrameSize, pSpecc11, dlyc11, dlyc11, bufc11);
	// filter x1 with c12 and store the result in r112
	ippsFIRSR_64f(x1, r112, FrameSize, pSpecc12, dlyc12, dlyc12, bufc12);
	// filter x2 with c11 and store the result in r211
	ippsFIRSR_64f(x2, r211, FrameSize, pSpecc11, dlyc11, dlyc11, bufc11);
	// filter x2 with c12 and store the result in r212
	ippsFIRSR_64f(x2, r212, FrameSize, pSpecc12, dlyc12, dlyc12, bufc12);

	// filter x2 with c22 and store the result in r222
	ippsFIRSR_64f(x2, r222, FrameSize, pSpecc22, dlyc22, dlyc22, bufc22);
	// filter x2 with c21 and store the result in r221
	ippsFIRSR_64f(x2, r221, FrameSize, pSpecc21, dlyc21, dlyc21, bufc21);
	// filter x1 with c22 and store the result in r122
	ippsFIRSR_64f(x1, r122, FrameSize, pSpecc22, dlyc22, dlyc22, bufc22);
	// filter x1 with c21 and store the result in r121
	ippsFIRSR_64f(x1, r121, FrameSize, pSpecc21, dlyc21, dlyc21, bufc21);

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
	// copy y1 to OutputData1
	ippsCopy_64f(y1, OutputData1, FrameSize);
	// copy y2 to OutputData2
	ippsCopy_64f(y2, OutputData2, FrameSize);
	*/
	// copy y1 to OutputData1
	ippsCopy_64f((double*)Input[0]->DataBuffer, (double*)Output[0]->DataBuffer, FrameSize);
	// copy y2 to OutputData2
	ippsCopy_64f((double*)Input[0]->DataBuffer, (double*)Output[0]->DataBuffer, FrameSize);
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

	if (ytmp == 0)
	{
		ytmp = ippsMalloc_64f(FrameSize);
		ippsZero_64f(ytmp, FrameSize);
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
	memset(fileName, 0, MAX_FILE_NAME_LENGTH * sizeof(char));
	strcpy(fileName, "../c11.dat");
	read_dat(fileName, c11, M);
	memset(fileName, 0, MAX_FILE_NAME_LENGTH * sizeof(char));
	strcpy(fileName, "../c12.dat");
	read_dat(fileName, c12, M); 
	memset(fileName, 0, MAX_FILE_NAME_LENGTH * sizeof(char));
	strcpy(fileName, "../c21.dat");
	read_dat(fileName, c21, M);
	memset(fileName, 0, MAX_FILE_NAME_LENGTH * sizeof(char));
	strcpy(fileName, "../c22.dat");
	read_dat(fileName, c22, M);

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
	// initialization filtering structure
	ippsFIRSRGetSize(M, ipp64f, &specSizeh11, &bufSizeh11);
	ippsFIRSRGetSize(M, ipp64f, &specSizeh12, &bufSizeh12);
	ippsFIRSRGetSize(M, ipp64f, &specSizeh21, &bufSizeh21);
	ippsFIRSRGetSize(M, ipp64f, &specSizeh22, &bufSizeh22);
	ippsFIRSRGetSize(M, ipp64f, &specSizec11, &bufSizec11);
	ippsFIRSRGetSize(M, ipp64f, &specSizec12, &bufSizec12);
	ippsFIRSRGetSize(M, ipp64f, &specSizec21, &bufSizec21);
	ippsFIRSRGetSize(M, ipp64f, &specSizec22, &bufSizec22);
	
	dlyh11 = ippsMalloc_64f(M - 1);
	dlyh12 = ippsMalloc_64f(M - 1);
	dlyh21 = ippsMalloc_64f(M - 1);
	dlyh22 = ippsMalloc_64f(M - 1);
	dlyc11 = ippsMalloc_64f(M - 1);
	dlyc12 = ippsMalloc_64f(M - 1);
	dlyc21 = ippsMalloc_64f(M - 1);
	dlyc22 = ippsMalloc_64f(M - 1);

	ippsZero_64f(dlyh11, M - 1);
	ippsZero_64f(dlyh12, M - 1);
	ippsZero_64f(dlyh21, M - 1);
	ippsZero_64f(dlyh22, M - 1);
	ippsZero_64f(dlyc11, M - 1);
	ippsZero_64f(dlyc12, M - 1);
	ippsZero_64f(dlyc21, M - 1);
	ippsZero_64f(dlyc22, M - 1);

	pSpech11 = (IppsFIRSpec_64f*)ippsMalloc_8u(specSizeh11);
	pSpech12 = (IppsFIRSpec_64f*)ippsMalloc_8u(specSizeh12);
	pSpech21 = (IppsFIRSpec_64f*)ippsMalloc_8u(specSizeh21);
	pSpech22 = (IppsFIRSpec_64f*)ippsMalloc_8u(specSizeh22);
	pSpecc11 = (IppsFIRSpec_64f*)ippsMalloc_8u(specSizec11);
	pSpecc12 = (IppsFIRSpec_64f*)ippsMalloc_8u(specSizec12);
	pSpecc21 = (IppsFIRSpec_64f*)ippsMalloc_8u(specSizec21);
	pSpecc22 = (IppsFIRSpec_64f*)ippsMalloc_8u(specSizec22);

	bufh11 = ippsMalloc_8u(bufSizeh11);
	bufh12 = ippsMalloc_8u(bufSizeh12);
	bufh21 = ippsMalloc_8u(bufSizeh21);
	bufh22 = ippsMalloc_8u(bufSizeh22);
	bufc11 = ippsMalloc_8u(bufSizec11);
	bufc12 = ippsMalloc_8u(bufSizec12);
	bufc21 = ippsMalloc_8u(bufSizec21);
	bufc22 = ippsMalloc_8u(bufSizec22);

	ippsFIRSRInit_64f(h11, M, ippAlgDirect, pSpech11);
	ippsFIRSRInit_64f(h12, M, ippAlgDirect, pSpech12);
	ippsFIRSRInit_64f(h21, M, ippAlgDirect, pSpech21);
	ippsFIRSRInit_64f(h22, M, ippAlgDirect, pSpech22);
	ippsFIRSRInit_64f(c11, M, ippAlgDirect, pSpecc11);
	ippsFIRSRInit_64f(c12, M, ippAlgDirect, pSpecc12);
	ippsFIRSRInit_64f(c21, M, ippAlgDirect, pSpecc21);
	ippsFIRSRInit_64f(c22, M, ippAlgDirect, pSpecc22);
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

	if (ytmp != 0)
	{
		ippsFree(ytmp);
		ytmp = 0;
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

	ippsFree(dlyh11);
	ippsFree(dlyh12);
	ippsFree(dlyh21);
	ippsFree(dlyh22);
	ippsFree(dlyc11);
	ippsFree(dlyc12);
	ippsFree(dlyc21);
	ippsFree(dlyc22);

	ippsFree(bufh11);
	ippsFree(bufh12);
	ippsFree(bufh21);
	ippsFree(bufh22);
	ippsFree(bufc11);
	ippsFree(bufc12);
	ippsFree(bufc21);
	ippsFree(bufc22);
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
