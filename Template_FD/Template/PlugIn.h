#pragma once
#include "LEEffect.h"
#include "ipp.h"
#include "myLib.h"
#include "math.h"
#include <complex>

#define PLAYBUFFER_TXT "PlayBuffer"
#define	SCALAR_TXT "Scalar"
#define VECTOR_TXT "Vector"
#define VARLEN_TXT "VarLen"
			
#define INT16_TXT "Integer 16bit"
#define INT32_TXT "Integer 32bit"
#define FLT32_TXT "Float 32bit"
#define DBL64_TXT "Double 64bit"

#define DATATYPEBITMASK VECTOR|SCALAR|BUFFSTREAMING|VARLEN
#define DATALENBITMASK DATAINT32|DATAINT16|DATAINT8|DATAFLOAT32|DATADOUBLE64|CUSTOM

#define WIDTHDEF 75

#define NUTS_NAME	"Fast Deconvolution"


class PlugIn :	public LEEffect
{
public:
	PlugIn(InterfaceType _CBFunction,void * _PlugRef,HWND ParentDlg);

	~PlugIn(void);
		
	void __stdcall LESetName(char *Name);

	void __stdcall LEPlugin_Init();
	int  __stdcall LEPlugin_Process(PinType **Input,PinType **Output,LPVOID ExtraInfo);
	void __stdcall LEPlugin_Delete(void);

	bool __stdcall LEInfoIO(int index,int type, char *StrInfo);

	int __stdcall LEGetNumInput(){return LEEffect::LEGetNumInput();};  
	int __stdcall LEGetNumOutput(){return LEEffect::LEGetNumOutput();};
	//int __stdcall LESetNumInput(int Val,PinType *TypeNewIn=0){return LEGetNumInput();}; 
	//int __stdcall LESetNumOutput(int Val,PinType *TypeNewOut=0){return LEGetNumOutput();};
	int __stdcall LESetNumInput(int Val, PinType *TypeNewIn = 0) { return LEEffect::LESetNumInput(Val, TypeNewIn); };
	int __stdcall LESetNumOutput(int Val, PinType *TypeNewOut = 0) { return LEEffect::LESetNumOutput(Val, TypeNewOut); };

	void __stdcall LESetParameter(int Index,void *Data,LPVOID bBroadCastInfo);
	int  __stdcall LEGetParameter(int Index,void *Data);

	int __stdcall LESetDefPin(int index,int type, PinType *Info);

	HWND __stdcall LEGetWndSet(){return 0;};
	int __stdcall  LEWinSetStatusChange(int NewStatus){return 0;};		

	void __stdcall LESaveSetUp();
	void __stdcall LELoadSetUp();

	void __stdcall LESampleRateChange(int NewVal,int TrigType); 
	void __stdcall LEFrameSizeChange (int NewVal,int TrigType); 
	void __stdcall LEConnectionChange(int IOType,int Index,bool Connected){};
	int  __stdcall LEConnectionRequest(int IOType,int Index,PinType *NewType);
	int  __stdcall LEExtraInfoPinChange(int IOType,int Index,PinExtraInfoType ExInfo){return 0;};

	void __stdcall LERTWatchInit();
	LPVOID __stdcall LEOnNUTechMessage(int MessageType,int MessageID,WPARAM wParam,LPARAM lParam);

private:

	int FrameSize,SampleRate;
	int L, M, fs, tau;
	double beta, * hrir;
	Ipp64fc* x1buff, * x2buff, * X1BUFF, * X2BUFF, * y1buff, * y2buff, * Y1BUFF, * Y2BUFF;
	Ipp64fc* c11, * c12, * c21, * c22, * C11, * C12, * C21, * C22;
	Ipp64fc* H11, * H12, * H21, * H22;
	Ipp64fc det, invtemp, invtemp2, Cprev[2][2], C[2][2], B[2][2], Ctemp[2][2], Ctemp2[2][2];
	//double* temp, * temp2, * temp3;
	//std::complex<double> Cprev[2][2], C[2][2], B[2][2], Ctemp[2][2];
	/************** CCS FORMAT ****************/
	//std::complex<double> det, invtemp, temp;
	//std::complex<double> det, invtemp;
	/************** PACK FORMAT **************/
	//double* temp, * temp2, * temp3;
	Ipp64fc* temp, * temp2, * temp3;

	// variables declaration for FFT
	/*
	int sizeSpec, sizeInit, sizeBuf;
	Ipp8u* pSpecMem;
	Ipp8u* pBuffer;
	Ipp8u* pmemInit;
	IppStatus status;
	IppsFFTSpec_R_64f* fftState;
	*/
	IppsFFTSpec_C_64fc* pSpec;

	Ipp8u* pMemSpec;
	Ipp8u* pMemInit;
	Ipp8u* pMemBuffer;

	int sizeSpec, sizeInit, sizeBuffer;

	int fftLen, fftOrd;

};