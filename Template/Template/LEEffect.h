#ifndef __LEEffect_H__
#define __LEEffect_H__

#define SCALAR			256
#define VECTOR			1
#define BUFFSTREAMING	2
#define VARLEN			8192
#define VIDEOFRAME		4096

#define DATAINT32		4
#define DATAINT16		8
#define DATAINT8		16
#define DATAFLOAT32		32
#define DATADOUBLE64	64
#define CUSTOM			128

#define PLAYBUFFER		BUFFSTREAMING
	
#define NONETYPE		 16384

#define NUTS_ALERT			 1
#define NUTS_CHANGEINPUT		 2
#define NUTS_CHANGEOUTPUT	 3
#define NUTS_SETEXCLUSIVEIN	 4
#define NUTS_SETEXCLUSIVEOUT	 5
#define NUTS_SETTYPEIN		 6
#define NUTS_SETTYPEOUT		 7
#define NUTS_GETTYPEIN		 8
#define NUTS_GETTYPEOUT		 9
#define NUTS_GETLATENCY		10
#define NUTS_GETFRAME_RATE	11
#define NUTS_GET_FS_SR		NUTS_GETFRAME_RATE
#define NUTS_GETFRAMECOUNT	12
#define NUTS_WRITEFILE		13
#define NUTS_READFILE		14

#define NUTS_GETIDEFF_PLG	15
#define NUTS_DEBUGOUTPUT		16
#define NUTS_GETENGINETYPE	17
#define NUTS_INSERTINPUT		18
#define NUTS_INSERTOUTPUT	19
#define NUTS_DELETEOUTPUT	20
#define NUTS_DELETEINPUT		21

#define NUTS_GETSTREAMINGSTATUS	22
#define NUTS_GETINEXTRAINFO		23
#define NUTS_GETOUTEXTRAINFO		24
#define NUTS_SETINEXTRAINFO		25
#define NUTS_SETOUTEXTRAINFO		26

#define NUTS_ADDRTWATCH			27
#define NUTS_REMOVERTWATCH		28
#define NUTS_UPDATERTWATCH		29
#define NUTS_CHANGERTWATCH		30


#define NUTS_ENABLEWIN		128

#define NUTS_SETNUMINPUT		NUTS_CHANGEINPUT
#define NUTS_SETNUMOUTPUT	NUTS_CHANGEOUTPUT
#define NUTS_GETNUMINTPUT	129
#define NUTS_GETNUMOUTPUT	130	

#define NUTS_GETSECURETIME		131
#define NUTS_RELEASESECURETIME	132
#define NUTS_BROADCASTPARAM1		133
#define NUTS_BROADCASTPARAM2		134
#define NUTS_BROADCASTBOARD     NUTS_BROADCASTPARAM1
#define NUTS_BROADCASTSEL	    NUTS_BROADCASTPARAM2

#define NUTS_REGISTERTRIG		135
#define NUTS_UNREGISTERTRIG		136
#define NUTS_SETFRAMESIZETRIG	137
#define NUTS_SETSAMPLERATETRIG	138
#define NUTS_GETFRAMESIZETRIG	139	
#define NUTS_GETSAMPLERATETRIG	140	

#define NUTS_TRIGGEREVENT		141
#define NUTS_GETDRIVERTYPE		142
#define NUTS_TRIGGEREVENT_SYNC	143

#define NUTS_CREATEMEMORY		144
#define NUTS_REMOVEMEMORY		145
#define NUTS_READMEMORY			146
#define NUTS_WRITEMEMORY		147
#define NUTS_ADDNOTIFYMEMORY	148
#define NUTS_REMOVENOTIFYMEMORY	149
#define NUTS_EXISTMEMORY		150
#define NUTS_GETSIZEMEMORY		151

#define NUTS_NUMTRIGGERCALL		152

#define NUTSSECURE		0
#define GLOBALSECURE	1

#define INPUT	0
#define OUTPUT	1

#define MAXNAME				12

#define MAXCARDEBUGPLUGIN	MAX_PATH+20 
#define NOTELEN				MAXCARDEBUGPLUGIN
#define MAXSTRING			MAXCARDEBUGPLUGIN

#define ENGINEInt16LSB		1
#define ENGINEInt32LSB		2


#define DRVSTRMSTATUS		-2
#define BOARDSTRMSTATUS		-1
#define NUTSSTRMSTATUS		0

#define WSS_OPEN	1
#define WSS_MAX		2
#define WSS_CLOSE	4
#define WSS_MIN		8

#define CHANGERTWATCH_VARNAME		1
#define CHANGERTWATCH_ENABLEWRITE	2
#define CHANGERTWATCH_AUTOUPDATE	4
#define CHANGERTWATCH_TYPEVAR		8
#define CHANGERTWATCH_EXTRAINFO		16

class LEEffect;

#define ASIODRIVER			1
#define DIRECTDRIVER		2
#define OFFLINEDRIVER		4 //audio
#define NUTSDRIVER			8
#define REMOTECONTROL		1024

#define OFFLINEAUDIO		OFFLINEDRIVER

#define NOTCOMPLETED	0
#define COMPLETED		1

#define AUDIOPROC		1
#define VIDEOPROC		2
#define MIDIPROC		4
#define ALLPROC		0xFFFFFFFF
#define OFFLINEPROC		ALLPROC

#define AUDIOTRIG		1
#define VIDEOTRIG		2
#define MIDITRIG		4

#define DEFAULT_DATAREADY	-1

#define IDXTRIGENGINETYPE	-1	// reserved
#define IDXTRIGLATENCY_IN	-2  // reserved
#define IDXTRIGLATENCY_OUT	-3  // reserved

#define	LICENSE_FREEWARE	1
#define	LICENSE_SINGLE		2
#define	LICENSE_MULTI		3
#define	LICENSE_SITE		4
#define	LICENSE_CORPORATE	5

#define MSG_TRIGGER			1
#define MSG_RTWATCH			2
#define MSG_MEMORY			3
///////// TRIGGER mode messages //////////
#define TRG_STARTSCHEDULER	1
#define TRG_STOPSCHEDULER	2
///////// RTWATCH messages //////////
#define RTW_MOUSEWHEEL		1
///////// MEMORY messages //////////
#define MEM_REMOVE			1
#define MEM_WRITE			2

#define TESTPROC(tipo,test)  ((tipo & test) == test)

struct StartUpNUTSProps
{
	int NumInStartUp;
	int NumOutStartUp;

	int BitMaskLicense;	// NUTS license type [0xFFFFFFFF : all]
	int BitMaskDriver;	// Driver model/s for the current NUTS [0xFFFFFFFF : all drivers]
	int BitMaskProc;	// Processing type for the current NUTS

	bool ManualReadyPin;	// use functions GetReadyToExecute e GetReadyOutPin
};

struct ProcessExtraInfoType
{
	int IDProcessRequest;	// type of requested processing [VIDEOPROC - AUDIOPROC]
	double TimeStamp;		// timestamp when scheduling starts
};

struct ExtraVInfo
{
	bool ValidInfo;

	///// Video Frame Info  //////
	int Width;			// Frame width
	int Height;			// Frame  Heigth
	int BitPerPixel;	// bits per pixel
	int BytePerPixel;	// bytes per pixel
	int ByteFrameLen;	// Frame length in bytes (W x H x BytePerPixel)

	bool Coded;			//If the frame is coded or not [true = coded]
	int  FourCC;		//Codec id  [if Coded=TRUE].
	int	 CodedValidByte;//DataBuffer Valid data when Coded=TRUE.
	//////////////////////////////
};

struct PinExtraInfoType
{
	ExtraVInfo	VideoInfo;
};

struct PinType
{
	void	*DataBuffer;	// poiter to data to be used inside the PlugIn
	int		DataType;		// 
	bool	Exclusive;		// true: pin must be connected in order for the PlugIn to work
	int		DataLen;		// valid data
	int		MaxDataLen;		// Maximum length of data (differs from DataLen only for VARLEN datatype)
	bool	UpdatedOut;		// To indicate if output pin has been updated or not.. must be set from the NUTS responsible for generating the output.
	bool    AutoUpdate;		// To forward the request of datatype change to subsequent connected pin

	PinExtraInfoType	ExtraInfo;		
};

typedef enum WatchDataType
{
	WTC_CHAR=0, 
	WTC_BYTE,
	WTC_SHORT,
	WTC_USHORT,
	WTC_INT,
	WTC_UINT,
	WTC_FLOAT,
	WTC_DOUBLE,
	WTC_LPCHAR,
	WTC_BOOLEAN,
	WTC_TYPE,
/// not implemented ///
	WTC_LPSHORT,
	WTC_LPINT,
	WTC_LPFLOAT,
	WTC_LPDOUBLE,
	WTC_LPBOOLEAN,
	WTC_LPVOID,
	WTC_LPBYTE,
	WTC_LPUSHORT,
	WTC_LPUINT
};

#define XUI_EDITBOX		1
#define XUI_CHECKBOX	4
#define XUI_BUTTON		3
#define XUI_COMBOBOX	2
#define XUI_SLIDER		5
#define XUI_LIST        6

struct ExtraInfoRTEdit
{
	int TypeExtraInfo;	// must be XUI_EDITBOX;
	int sizeExtraInfo;	// must be sizeof(ExtraInfoRTEdit)

	bool EnableWheel;
	bool Reserved;		
	double  MaxValue;  
	double  MinValue;  
};

struct ExtraInfoRTCheck
{
	int TypeExtraInfo;	// must be XUI_CHECKBOX;
	int sizeExtraInfo;	// must be sizeof(ExtraInfoRTCheck)

	char String_checked[MAXSTRING];		// string to be shown when checked
	char String_unchecked[MAXSTRING];	// string to be shown when unchecked
	int  StileExt;						// riserved
};

struct ExtraInfoRTButton
{
	int TypeExtraInfo;	// must be XUI_BUTTON;
	int sizeExtraInfo;	// must be sizeof(ExtraInfoRTButton)

	char BtnString[MAXSTRING];	// string to be shown when button is up
	int  Width;					// button width. use -1 for default dimension
	int  StileExt;				// riserved
};

struct ItemRTCombo 
{
	char	String[MAXSTRING];
	LPVOID  Item;	// note: Item must be handled inside NUTS [memory allocation and destruction]

	ItemRTCombo *Reserved; // Don't use this
};

struct ExtraInfoRTCombo
{
	int TypeExtraInfo;	// must be XUI_COMBOBOX;
	int sizeExtraInfo;	// must be sizeof(ExtraInfoRTCombo)

	int NumElements;
	ItemRTCombo *ListElement;
};

struct ExtraInfoRTSlider
{
	int TypeExtraInfo;	// must be XUI_SLIDER;
	int sizeExtraInfo;	// must be sizeof(ExtraInfoRTSlider)

	double  MaxValue;  
	double  MinValue;  
	bool	UpdateOnScroll;	// to update slider during scrolling
	LPVOID	ExtraStyle;
	double	ReservedV;
};

enum TextAlign : BYTE
{
	ALIGN_LEFT = 0,   // Left alignement
	ALIGN_RIGHT = 1,  // Right alignement
	ALIGN_CENTER = 2  // Center alignement
};

struct ExtraInfoCell
{
	char format[16]; // format of the number, if any. Default: %d for integral data, "%2.5g" for floating point.
	TextAlign align; // Default: ALIGN_LEFT
	UINT width;      // cell width in pixels. Default: 40
};

struct ExtraInfoRTList
{
	int TypeExtraInfo; // must be XUI_LIST
	int sizeExtraInfo; // must be sizeof(ExtraInfoRTList)

	BOOL showIndex;      // true if a little index is showed in the cell. Default: true;
	enum EditMode : BYTE
	{
		EM_INPLACE = 0,           // the cell is edited in place. Default.
		EM_WITHINDEXSELECTOR = 1  // the cell is showed in a index selector.
	} editMode;

	ExtraInfoCell cellInfo; // info about every cell. (All cells are equal in a list).
};

struct WatchType
{
	char			VarName[MAXCARDEBUGPLUGIN];	//name to be shown in RTWatch
	bool			EnableWrite;				//if parameter can be modified by GUI
	bool			AutoUpdate;					//if parameter must be automatically updated through polling 				
	WatchDataType	TypeVar;					//variable type
	int				LenByte;					//variable length in bytes
	int				IDVar;						//variable ID (see Set/GetParameter)
	void			*ExtraInfo;					//
};


struct SharedMemoryHelper
{
	char	*ItemName;
	int		DataSize;
	int		Offset;

	LPVOID	pData;
	BOOL	AutoRedim;
	LPVOID  pPlug;

	int		Reserved;
};

typedef int  (__stdcall *InterfaceType)(LEEffect *eff,int OpCode,int lParam,void *wParam);

class LEEffect
{
public:

	LEEffect(InterfaceType _CBFunction,void * _PlugRef,HWND ParentDlg)
	{
		PlugRef=_PlugRef;
		ByPass=false;
		CBFunction=_CBFunction;
		DriverRef=0;
		vers=2;
		debug=0;
		ParentWin=ParentDlg;
	};

	virtual ~LEEffect(void){};

	virtual void __stdcall LESetName(char *Name){};

	virtual void __stdcall LEPlugin_Init(void){};
	virtual int  __stdcall LEPlugin_Process(PinType **Input,PinType **Output,LPVOID ExtraInfo){return COMPLETED;};
	virtual void __stdcall LEPlugin_Delete(void){};

	virtual bool __stdcall LEInfoIO(int index,int type, char *StrInfo){return false;};
	virtual int __stdcall  LEGetNumInput(){return CBFunction(this,NUTS_GETNUMINTPUT,0,0);};  
	virtual int __stdcall  LEGetNumOutput(){return CBFunction(this,NUTS_GETNUMOUTPUT,0,0);};
	virtual int __stdcall  LESetNumInput(int Val,PinType *NewInType=0)
	{
		return CBFunction(this,NUTS_SETNUMINPUT,Val,NewInType);
	};  
	virtual int __stdcall LESetNumOutput(int Val,PinType *NewOutType=0)
	{
		return CBFunction(this,NUTS_SETNUMOUTPUT,Val,NewOutType);
	};

	virtual void __stdcall LESetParameter(int Index,void *Data,LPVOID bBroadCastInfo){};
	virtual int  __stdcall LEGetParameter(int Index,void *Data){return 0;};

	virtual int __stdcall LESetDefPin(int index,int type, PinType *Info){return 0;};

	virtual HWND __stdcall LEGetWndSet(){return 0;};		// settings window
	virtual int __stdcall  LEWinSetStatusChange(int NewStatus){return 0;};		

	virtual int __stdcall LEIsReadyForExecute(int TriggerType,int TypeIO,int IDPin,bool Linked,bool Exclusive, bool DataReady,bool LastChance){return 1;};
	virtual int __stdcall LEGetReadyOutPin(int TriggerType,int IDPin){return 1;};

	virtual void __stdcall LESaveSetUp(){ };			
	virtual void __stdcall LELoadSetUp(){ };			

	virtual void __stdcall LESampleRateChange(int NewVal, int TriggerType){}; 
	virtual void __stdcall LEFrameSizeChange (int NewVal, int TriggerType){};
	virtual void __stdcall LEConnectionChange(int IOType,int Index,bool Connected){};
	virtual int  __stdcall LEConnectionRequest(int IOType,int Index,PinType *NewType){return 0;};
	virtual int  __stdcall LEExtraInfoPinChange(int IOType,int Index,PinExtraInfoType ExInfo){return 0;};

	virtual LPVOID __stdcall LEOnNUTechMessage(int MessageType,int MessageID,WPARAM wParam,LPARAM lParam){return NULL;};
	virtual void __stdcall   LERTWatchInit(){ };

	InterfaceType CBFunction;  

	char NamePlugIn[MAXNAME];
	bool ByPass;
	
	////////////////////// DON'T TOUCH !!!!   /////////////////////////////////////

	void *PlugRef;		
	void *DriverRef;	
	void *debug;

	HWND ParentWin; 
	int vers;		
	int LEOutputDebugString(const char *Format, ...)
	{
		va_list ap ;
		int ret=1;
		char Txt[MAXCARDEBUGPLUGIN];
		va_start( ap, Format ) ; 

		int Len=_vscprintf(Format,ap)+1;

		if (Len<MAXCARDEBUGPLUGIN)
		{
			memset(Txt,0,MAXCARDEBUGPLUGIN);
			ret=vsprintf_s(Txt,Format,ap);
			CBFunction(this,NUTS_DEBUGOUTPUT,0,Txt);
		}

		va_end( ap ) ; 

		return ret;
	};
	
};

typedef LEEffect * (__stdcall *LoaderType)(InterfaceType CB, void *PlugRef,HWND ParentDlg);
typedef int  (__stdcall *UnLoaderType)(LEEffect *PlugToDelete);
typedef void (__stdcall *GetInfoType)(char *Name,int *W,void *ExtraInfo);

#endif