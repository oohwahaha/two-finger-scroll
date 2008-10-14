//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "SynComDefs.h"
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "SYNCTRLLib_OCX"
#pragma link "SYNCOMLib_OCX"
#pragma resource "*.dfm"

static const UnicodeString regKey = "Software\\TwoFingerScroll";
struct TTapInfo {
	DWORD eventDown, eventUp, x;
};

static const TTapInfo tapInfo[] = {
	{MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP, 0},
	{MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP, 0},
	{MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP, 0},
	{MOUSEEVENTF_XDOWN, MOUSEEVENTF_XUP, XBUTTON1},
	{MOUSEEVENTF_XDOWN, MOUSEEVENTF_XUP, XBUTTON2}
};

TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
	synAPI = new TSynAPI(this);
	synTouchPad = new TSynDevice(this);

	synAPI->Initialize();

	long devHandle = -1;

	synAPI->FindDevice(SE_ConnectionAny,
		SE_DeviceTouchPad,
		&devHandle);

	if (devHandle < 0) {
		Application->MessageBox(L"No Synaptics TouchPad found!",
			L"TwoFingerScroll");
		Application->Terminate();
	}

	synTouchPad->Select(devHandle);
	synTouchPad->CreatePacket(&synPacket);

	long multi;

	synTouchPad->GetProperty(SP_IsMultiFingerCapable, &multi);
	if (!multi) {
		TRegistry *reg = new TRegistry();
		reg->RootKey = HKEY_LOCAL_MACHINE;
		reg->Access = KEY_ALL_ACCESS;
		if (!reg->OpenKey("System\\CurrentControlSet\\Services\\SynTP\\Parameters", false)) {
			Application->MessageBox(L"Synaptics driver registry key missing. Reinstall drivers.",
				L"TwoFingerScroll");
			Application->Terminate();
		}
		unsigned int mask = 0;
		if (reg->ValueExists("CapabilitiesMask")) {
			mask = reg->ReadInteger("CapabilitiesMask");
			if (mask == 0xFFFFFFFF) {
				Application->MessageBox(L"This TouchPad doesn't support multiple fingers.",
					L"TwoFingerScroll");
				Application->Terminate();
			}
		}
		if (mask != 0xFFFFFFFF) {
			reg->WriteInteger("CapabilitiesMask", 0xFFFFFFFF);
			reg->CloseKey();
			if (Application->MessageBox(L"Driver support for multiple fingers has been enabled.\n"
				"System restart is required for the changes to take effect.\n"
				"\n"
				"Restart now?",
				L"TwoFingerScroll", MB_YESNO) == IDYES)
			{
				ExitWindowsEx(EWX_REBOOT, SHTDN_REASON_MAJOR_APPLICATION);
			}
			Application->Terminate();
		}
		reg->CloseKey();
	}

	// activate the OnPacket event
	synTouchPad->SetSynchronousNotification(this);

	settings = new TRegistry();

	SettingsLoad();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SettingsLoad(bool defaults)
{
	settings->RootKey = HKEY_CURRENT_USER;
	if (!defaults)
		settings->OpenKeyReadOnly(regKey);

	if (settings->ValueExists("scroll")) {
		int t = settings->ReadInteger("scroll");
		switch (t) {
		case 1:
			scrollLinear->Checked = true;
			break;
		//case 2:
		//	scrollCircular->Checked = true;
		//	break;
		default:
			scrollOff->Checked = true;
		}
	}
	else scrollLinear->Checked = true;
	scrollLinearClick(NULL);

	if (settings->ValueExists("scrollLinearEdge")) {
		scrollLinearEdge->Checked =
			settings->ReadInteger("scrollLinearEdge");
	}
	else scrollLinearEdge->Checked = true;

	if (settings->ValueExists("scrollLinearAcc")) {
		scrollLinearAcc->Position =
			settings->ReadInteger("scrollLinearAcc");
	}
	else scrollLinearAcc->Position = 75;

	if (settings->ValueExists("tapActive")) {
		tapActive->Checked =
			settings->ReadInteger("tapActive");
	}
	else tapActive->Checked = false;
	tapActiveClick(NULL);

	if (settings->ValueExists("tapFunction")) {
		tapFunction->ItemIndex =
			settings->ReadInteger("tapFunction");
	}
	else tapFunction->ItemIndex = 1;

	settings->CloseKey();

	TRegistry *reg = new TRegistry();
	reg->RootKey = HKEY_CURRENT_USER;
	if (!defaults)
		reg->OpenKeyReadOnly("Software\\Microsoft\\Windows\\CurrentVersion\\Run");

	if (reg->ValueExists("TwoFingerScroll")) {
		 startWithWindows->Checked = (reg->ReadString("TwoFingerScroll") ==
			Application->ExeName);
	}
	else startWithWindows->Checked = false;

	reg->CloseKey();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SettingsSave()
{
	settings->RootKey = HKEY_CURRENT_USER;
	settings->Access = KEY_ALL_ACCESS;
	if (!settings->OpenKey(regKey, true))
		return;

	int t = scrollLinear->Checked ? 1 :
		(scrollCircular->Checked ? 2 : 0);
	settings->WriteInteger("scroll", t);
	settings->WriteInteger("scrollLinearEdge",
		scrollLinearEdge->Checked);
	settings->WriteInteger("scrollLinearAcc",
		scrollLinearAcc->Position);
	settings->WriteInteger("tapActive",
		tapActive->Checked);
	settings->WriteInteger("tapFunction",
		tapFunction->ItemIndex);

	settings->CloseKey();

	TRegistry *reg = new TRegistry();
	reg->RootKey = HKEY_CURRENT_USER;
	reg->Access = KEY_ALL_ACCESS;
	reg->OpenKey("Software\\Microsoft\\Windows\\CurrentVersion\\Run", false);

	if (startWithWindows->Checked) {
		reg->WriteString("TwoFingerScroll", Application->ExeName);
	}
	else if (reg->ValueExists("TwoFingerScroll")) {
		reg->DeleteValue("TwoFingerScroll");
	}

	reg->CloseKey();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::AcquirePad(bool acquire)
{
	if (acquire && !IsPadAcquired) {
		synTouchPad->Acquire(0);
	}
	else if (!acquire && IsPadAcquired) {
		synTouchPad->Unacquire();
	}
	IsPadAcquired = acquire;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LockDeviceTap(bool lock)
{
	if (lock) {
		if (deviceTapLockLevel == 0) {
			synTouchPad->GetProperty(SP_Gestures,
				&synTouchPadGestures);
			if (synTouchPadGestures & SF_GestureTap) {
				synTouchPad->SetProperty(SP_Gestures,
					synTouchPadGestures & ~SF_GestureTap);
			}
		}
		deviceTapLockLevel++;
	}
	else if (deviceTapLockLevel != 0) {
		if (deviceTapLockLevel == 1) {
			long g = synTouchPadGestures;
			synTouchPad->GetProperty(SP_Gestures,
				&synTouchPadGestures);
			if (g & SF_GestureTap != synTouchPadGestures & SF_GestureTap) {
				synTouchPad->SetProperty(SP_Gestures,
					synTouchPadGestures & ~SF_GestureTap | (g & SF_GestureTap));
			}
		}
		deviceTapLockLevel--;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Settings1Click(TObject *Sender)
{
	pages->ActivePageIndex = 0;
	Show();
	ok->SetFocus();
	SetForegroundWindow(Handle);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
	Hide();
	SettingsLoad();
	Action = caNone;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::scrollLinearClick(TObject *Sender)
{
	bool e = scrollLinear->Checked;

	scrollLinearEdge->Enabled = e;
	scrollLinearAccLabel->Enabled = e;
	scrollLinearAcc->Enabled = e;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::tapActiveClick(TObject *Sender)
{
	bool e = tapActive->Checked;

	tapFunctionLabel->Enabled = e;
	tapFunction->Enabled = e;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Exit1Click(TObject *Sender)
{
	Application->Terminate();
}
//---------------------------------------------------------------------------

HRESULT STDMETHODCALLTYPE TForm1::OnSynDevicePacket(long seqNum)
{
	long nof, fstate, xd, yd;

	// get the pointing data packet
	synTouchPad->LoadPacket(synPacket);

	if (!globalActive->Checked)
		return 0;

	// extract relevant data
	synPacket->GetProperty(SP_ExtraFingerState, &nof);
	nof &= 3;
	synPacket->GetProperty(SP_FingerState, &fstate);
	synPacket->GetProperty(SP_XDelta, &xd);
	synPacket->GetProperty(SP_YDelta, &yd);

	// handle tapping
	if (tapActive->Checked) {
		if (nof == 2) {
			if (!tapInProgress) {
				tapInProgress = true;
				synPacket->GetProperty(SP_TimeStamp,
					&tapStartTime);
				tapDistance = 0;
				GetCursorPos(&tapTouchPos);
				LockDeviceTap(true);
			}
			if (abs(xd) < 800) tapDistance += abs(xd);
			if (abs(yd) < 800) tapDistance += abs(yd);
		}
		else if (tapInProgress) {
			tapInProgress = false;
			LockDeviceTap(false);
			long tstamp;
			synPacket->GetProperty(SP_TimeStamp, &tstamp);
			if ((tstamp - tapStartTime < 175) &&
				(tapDistance < 100))
			{
				SetCursorPos(tapTouchPos.x,
					tapTouchPos.y);

				DoTap();
				return 0;
			}
		}
	}

	// handle scrolling
	if (scrollLinear->Checked) {
		if (fstate & SF_FingerPresent) {
			if (scrollTouchTime == 0) {
				GetCursorPos(&scrollTouchPos);
				synPacket->GetProperty(SP_TimeStamp,
						&scrollTouchTime);
			}
			if (nof == 2) {
				long y, ylo, yhi;
				synPacket->GetProperty(SP_Y, &y);
				synTouchPad->GetProperty(SP_YLoBorder, &ylo);
				synTouchPad->GetProperty(SP_YHiBorder, &yhi);
				if (IsPadAcquired && scrollLinearEdge->Checked) {
					if (ylo <= y && y <= yhi) {
						scrollNotEdge = true;
					}
					else if (scrollNotEdge) {
						DoScroll(scrollLastXDelta, scrollLastYDelta);
						return 0;
					}
				}
				if (fstate & SF_FingerMotion) {
					if (!IsPadAcquired) {
						AcquirePad(true);
						long tstamp;
						synPacket->GetProperty(SP_TimeStamp, &tstamp);
						if (tstamp - scrollTouchTime < 200) {
							SetCursorPos(scrollTouchPos.x,
								scrollTouchPos.y);
						}
					}
					if (IsPadAcquired) {
						DoScroll(xd, yd);
						scrollLastXDelta = xd;
						scrollLastYDelta = yd;
					}
				}
			}
			else {
				scrollLastXDelta = scrollLastYDelta = 0;
				AcquirePad(false);
				scrollBuffer = 0;
				scrollNotEdge = false;
			}
		}
		else {
			scrollTouchTime = 0;
			scrollLastXDelta = scrollLastYDelta = 0;
			AcquirePad(false);
			scrollBuffer = 0;
			scrollNotEdge = false;
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DoTap()
{
	INPUT i[2];
	const TTapInfo *info = &tapInfo[tapFunction->ItemIndex];

	ZeroMemory(i, sizeof(INPUT)*2);
	i[0].type = INPUT_MOUSE;
	i[0].mi.dwFlags = info->eventDown;
	i[0].mi.mouseData = info->x;
	i[1].type = INPUT_MOUSE;
	i[1].mi.dwFlags = info->eventUp;
	i[1].mi.mouseData = info->x;
	SendInput(2, i, sizeof(INPUT));
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DoScroll(long dx, long dy)
{
	long d;

	if (abs(dy) > 800)
		return;

	d = dy * dy / (scrollLinearAcc->Max - scrollLinearAcc->Position +
		scrollLinearAcc->Min);
	if (d < 10)
		d = 10;
	if (dy < 0)
		d = -d;
	scrollBuffer += d;
	d = scrollBuffer - scrollBuffer % WHEEL_DELTA;

	if (d != 0) {
		INPUT i;

		ZeroMemory(&i, sizeof(INPUT));
		i.type = INPUT_MOUSE;
		i.mi.dwFlags = MOUSEEVENTF_WHEEL;
		i.mi.mouseData = d;
		SendInput(1, &i, sizeof(INPUT));

		scrollBuffer -= d;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::okClick(TObject *Sender)
{
	Hide();
	SettingsSave();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::cancelClick(TObject *Sender)
{
	Hide();
	SettingsLoad();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::About1Click(TObject *Sender)
{
	Application->MessageBox(L"TwoFingerScroll 1.0.1\n"
		"\n"
		"Copyright (c) 2008 Arkadiusz Wahlig\n"
		"<arkadiusz.wahlig@gmail.com>",
		L"About...");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::defaultsClick(TObject *Sender)
{
	SettingsLoad(true);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Dispatch(void *Message)
{
	// the Synaptics API seems to have a problem with
	// power state changes; in particullar, the events
	// get deactivated after standby or hibernation;
	// here we ensure that they stay activated

	if (((TMessage *)Message)->Msg == WM_POWERBROADCAST) {
		reactivateTimer->Tag = 20;
		reactivateTimer->Enabled = true;
	}

	TForm::Dispatch(Message);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::reactivateTimerTimer(TObject *Sender)
{
	synTouchPad->SetSynchronousNotification(this);
	reactivateTimer->Tag--;
	if (!reactivateTimer->Tag) {
		reactivateTimer->Enabled = false;
	}
}
//---------------------------------------------------------------------------

