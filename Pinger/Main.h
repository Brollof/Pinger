#pragma once

#include "wx/wx.h"
#include "wx/time.h"
#include "Stats.h"
#include "TaskBarIcon.h"

class Main : public wxFrame
{
public:
	Main();
	~Main();

private:
	bool m_isStarted = false;
	wxButton* m_btnStartStop = nullptr;
	wxTextCtrl* m_txtTarget = nullptr;
	wxStaticText* m_labPacketLoss = nullptr;
	wxStaticText* m_labPing = nullptr;
	TaskBarIcon* m_taskBarIcon = nullptr;

	wxTimer m_timer;
	Stats* m_ploss;
	Stats* m_latency;

	void StartStopButtonClicked(wxCommandEvent& event);
	void OnTimer(wxTimerEvent& event);
	void OnClose(wxCloseEvent& event);
	std::string FormatVal(float avg, std::string suffix);

	wxDECLARE_EVENT_TABLE();
};

