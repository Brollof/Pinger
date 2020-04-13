#pragma once

#include "wx/wx.h"

class Main : public wxFrame
{
public:
	Main();
	~Main();

private:
	bool m_isStarted = false;
	wxButton* m_btnStart = nullptr;
	wxButton *m_btnStop = nullptr;
	wxTextCtrl *m_txtTarget = nullptr;
	wxStaticText *m_labPacketLoss = nullptr;

	void StartButtonClicked(wxCommandEvent& event);
	void StopButtonClicked(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
};

