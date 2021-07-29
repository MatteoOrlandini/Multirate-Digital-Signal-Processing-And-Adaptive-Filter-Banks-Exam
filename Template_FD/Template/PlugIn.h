#pragma once
#include "LEEffect.h"
#include "ipp.h"
#include "myLib.h"

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
	int M, tau;
	double beta;
	double* x1, * x2, * d1, * d2, * y1, * y2, * e1, * e2, * x1buff, * x2buff;
	double* C11, * C12, * C21, * C22, * c11, * c12, * c21, * c22;
	double* H11, * H12, * H21, * H22;
	double* r111, * r112, * r211, * r212, * r222, * r221, * r122, * r121;
	double* r111buff, * r112buff, * r211buff, * r212buff, * r222buff, * r221buff, * r122buff, * r121buff;
	double Cprev[2][2], C[2][2], B[2][2];
	
	char fileName[MAX_FILE_NAME_LENGTH];

	// variables declaration for FFT
	int sizeSpec, sizeInit, sizeBuf;
	Ipp8u* pSpecMem;
	Ipp8u* pBuffer;
	Ipp8u* pmemInit;
	IppStatus status;

	int fftLen, fftOrd;
	IppsFFTSpec_R_64f* fftState;

};