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

#define NUTS_NAME	"LMS Filter"

#define ID_MU 0 

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

	int FrameSize, SampleRate;
	int M, L, tau;
	double mu;
	double* x1, * x2, * d1, * d2, * y1, * y2, * e1, * e2, * e1tot, * e2tot, * x1buff, * x2buff;
	double* c11, * c12, * c21, * c22;
	double* h11, * h12, * h21, * h22;
	double* r111, * r112, * r211, * r212, * r222, * r221, * r122, * r121;
	double* r111buff, * r112buff, * r211buff, * r212buff, * r222buff, * r221buff, * r122buff, * r121buff;

	bool isRunning;
	/*
	int specSizeh11;
	int specSizeh12;
	int specSizeh21;
	int specSizeh22;
	*/
	/*
	int specSizec11;
	int specSizec12;
	int specSizec21;
	int specSizec22;
	*/
	/*
	int bufSizeh11;
	int bufSizeh12;
	int bufSizeh21;
	int bufSizeh22;
	*/
	/*
	int bufSizec11;
	int bufSizec12;
	int bufSizec21;
	int bufSizec22;
	*/
	/*
	double* dlyh11;
	double* dlyh12;
	double* dlyh21;
	double* dlyh22;
	*/
	/*
	double* dlyc11;
	double* dlyc12;
	double* dlyc21;
	double* dlyc22;
	*/
	/*
	IppsFIRSpec_64f* pSpech11;
	IppsFIRSpec_64f* pSpech12;
	IppsFIRSpec_64f* pSpech21;
	IppsFIRSpec_64f* pSpech22;
	*/
	/*
	IppsFIRSpec_64f* pSpecc11;
	IppsFIRSpec_64f* pSpecc12;
	IppsFIRSpec_64f* pSpecc21;
	IppsFIRSpec_64f* pSpecc22;
	*/
	/*
	Ipp8u* bufh11;
	Ipp8u* bufh12;
	Ipp8u* bufh21;
	Ipp8u* bufh22;
	*/
	/*
	Ipp8u* bufc11;
	Ipp8u* bufc12;
	Ipp8u* bufc21;
	Ipp8u* bufc22;
	*/
};