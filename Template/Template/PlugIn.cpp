#include "StdAfx.h"
#include ".\plugin.h"


PlugIn::PlugIn(InterfaceType _CBFunction,void * _PlugRef,HWND ParentDlg): LEEffect(_CBFunction,_PlugRef,ParentDlg)
{
	FrameSize = CBFunction(this,NUTS_GET_FS_SR,0,(LPVOID)AUDIOPROC);
	SampleRate = CBFunction(this,NUTS_GET_FS_SR,1,(LPVOID)AUDIOPROC);
	M = 512;
	tau = M;
	mu = 1e-4;
}

int __stdcall PlugIn::LEPlugin_Process(PinType **Input,PinType **Output,LPVOID ExtraInfo)
{ 
	double* InputData1 = ((double*)Input[0]->DataBuffer);
	double* OutputData1 = ((double*)Output[0]->DataBuffer);
	double* InputData2 = ((double*)Input[1]->DataBuffer);
	double* OutputData2 = ((double*)Output[1]->DataBuffer);

	ippsCopy_64f(InputData1, x1, FrameSize);
	ippsCopy_64f(InputData2, x2, FrameSize);
	ippsZero_64f(y1, FrameSize);
	ippsZero_64f(y2, FrameSize);

	ippsFIRSR_64f(x1, r111, FrameSize / M, pSpec3, dly3, dly3, buf3);
	ippsFIRSR_64f(w00, u00, FrameSize / M, pSpec3, dly3, dly3, buf3);
	ippsFIRSR_64f(w00, u00, FrameSize / M, pSpec3, dly3, dly3, buf3);
	ippsFIRSR_64f(w00, u00, FrameSize / M, pSpec3, dly3, dly3, buf3);

	ippsFIRSR_64f(w00, u00, FrameSize / M, pSpec3, dly3, dly3, buf3);
	ippsFIRSR_64f(w00, u00, FrameSize / M, pSpec3, dly3, dly3, buf3);
	ippsFIRSR_64f(w00, u00, FrameSize / M, pSpec3, dly3, dly3, buf3);
	ippsFIRSR_64f(w00, u00, FrameSize / M, pSpec3, dly3, dly3, buf3);
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
	ippsFIRSRGetSize(M, ipp64f, &specSize1, &bufSize1);
	ippsFIRSRGetSize(M, ipp64f, &specSize2, &bufSize2);

	dlyh11 = ippsMalloc_64f(M - 1);
	dlyh12 = ippsMalloc_64f(M - 1);
	dlyh21 = ippsMalloc_64f(M - 1);
	dlyh22 = ippsMalloc_64f(M - 1);
	dlyc11 = ippsMalloc_64f(M - 1);
	dlyc12 = ippsMalloc_64f(M - 1);
	dlyc21 = ippsMalloc_64f(M - 1);
	dlyc22 = ippsMalloc_64f(M - 1);

	ippsZero_64f(dly1, M - 1);
	ippsZero_64f(dly2, M - 1);

	pSpec1 = (IppsFIRSpec_64f*)ippsMalloc_8u(specSize1);
	pSpec2 = (IppsFIRSpec_64f*)ippsMalloc_8u(specSize2);

	buf1 = ippsMalloc_8u(bufSize1);
	buf2 = ippsMalloc_8u(bufSize2);

	ippsFIRSRInit_64f(h11, M, ippAlgDirect, pSpec1);
	ippsFIRSRInit_64f(h12, M, ippAlgDirect, pSpec1);
	ippsFIRSRInit_64f(h21, M, ippAlgDirect, pSpec1);
	ippsFIRSRInit_64f(h22, M, ippAlgDirect, pSpec1);

	ippsFIRSRInit_64f(c11, M, ippAlgDirect, pSpec2);
	ippsFIRSRInit_64f(c12, M, ippAlgDirect, pSpec2);
	ippsFIRSRInit_64f(c21, M, ippAlgDirect, pSpec2);
	ippsFIRSRInit_64f(c22, M, ippAlgDirect, pSpec2);


}

void __stdcall PlugIn::LEPlugin_Delete()
{
	
}

PlugIn::~PlugIn(void)
{

}


bool __stdcall PlugIn::LEInfoIO(int index,int type, char *StrInfo)
{
	if(type==INPUT) sprintf(StrInfo,"In[%d]",index);
	if(type==OUTPUT) sprintf(StrInfo,"Out[%d]",index);
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
		Info->NumInStartUp=1;
		Info->NumOutStartUp=1;
		Info->BitMaskProc = AUDIOPROC;
		Info->BitMaskDriver = OFFLINEDRIVER | DIRECTDRIVER | ASIODRIVER;
	}
}
