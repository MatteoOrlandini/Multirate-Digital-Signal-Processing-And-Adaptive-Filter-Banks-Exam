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
	beta = 0.1;

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
	sizeBuf = 0;
	pSpecMem = 0;
	pBuffer = 0;
	pmemInit = 0;

	fftLen = 512;		// NextPow2(fs + tapsLen - 1)
	fftOrd = (int)(log10((double)fftLen) / log10(2.0));
}

int __stdcall PlugIn::LEPlugin_Process(PinType **Input,PinType **Output,LPVOID ExtraInfo)
{ 
	double* InputData1 = ((double*)Input[0]->DataBuffer);
	double* OutputData1 = ((double*)Output[0]->DataBuffer);
	double* InputData2 = ((double*)Input[1]->DataBuffer);
	double* OutputData2 = ((double*)Output[1]->DataBuffer);

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

	if (C11 == 0)
	{
		C11 = ippsMalloc_64f(fftLen);
		ippsZero_64f(C11, fftLen);
	}

	if (C12 == 0)
	{
		C12 = ippsMalloc_64f(fftLen);
		ippsZero_64f(C12, fftLen);
	}

	if (C21 == 0)
	{
		C21 = ippsMalloc_64f(fftLen);
		ippsZero_64f(C21, fftLen);
	}

	if (C22 == 0)
	{
		C22 = ippsMalloc_64f(fftLen);
		ippsZero_64f(C22, fftLen);
	}
	// load filter taps
	memset(fileName, 0, MAX_FILE_NAME_LENGTH * sizeof(char));
	strcpy(fileName, "c11.dat");
	read_dat(fileName, c11, M);

	memset(fileName, 0, MAX_FILE_NAME_LENGTH * sizeof(char));
	strcpy(fileName, "c12.dat");
	read_dat(fileName, c12, M);

	memset(fileName, 0, MAX_FILE_NAME_LENGTH * sizeof(char));
	strcpy(fileName, "c21.dat");
	read_dat(fileName, c21, M);

	memset(fileName, 0, MAX_FILE_NAME_LENGTH * sizeof(char));
	strcpy(fileName, "c22.dat");
	read_dat(fileName, c22, M);

	// allocation of variables for FFT
	status = ippsFFTGetSize_R_64f(fftOrd, IPP_FFT_DIV_INV_BY_N, ippAlgHintNone, &sizeSpec, &sizeInit, &sizeBuf);
	pSpecMem = ippsMalloc_8u(sizeSpec);
	pBuffer = ippsMalloc_8u(sizeBuf);
	pmemInit = ippsMalloc_8u(sizeInit);
	ippsFFTInit_R_64f(&fftState, fftOrd, IPP_FFT_DIV_INV_BY_N, ippAlgHintNone, pSpecMem, pmemInit);

	ippsFFTFwd_RToPack_64f(c11, C11, fftState, pBuffer);
	ippsFFTFwd_RToPack_64f(c12, C12, fftState, pBuffer);
	ippsFFTFwd_RToPack_64f(c21, C21, fftState, pBuffer);
	ippsFFTFwd_RToPack_64f(c22, C22, fftState, pBuffer);


	if (H11 == 0)
	{
		H11 = ippsMalloc_64f(M);
		ippsZero_64f(H11, M);
	}
	
	if (H12 == 0)
	{
		H12 = ippsMalloc_64f(M);
		ippsZero_64f(H12, M);
	}
	
	if (H21 == 0)
	{
		H21 = ippsMalloc_64f(M);
		ippsZero_64f(H21, M);
	}
	
	if (H22 == 0)
	{
		H22 = ippsMalloc_64f(M);
		ippsZero_64f(H22, M);
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
