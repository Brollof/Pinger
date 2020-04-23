#include <wx/display.h>
#include <iostream>
#include "Main.h"
#include "Ping.h"

#define APP_NAME "Pinger"
#define APP_WIDTH 200
#define APP_HEIGHT 100
#define BORDER_WIDTH 4
#define PING_PERIOD 1000 // ms
#define DEFAULT_TARGET "wp.pl"
#define STATS_SAMPLES_NUM 10
#define ID_BTN_START 500
#define FORMAT_PLOSS(val) FormatVal(val, " %")
#define FORMAT_PING(val) FormatVal(val, " ms")

wxBEGIN_EVENT_TABLE(Main, wxFrame)
	EVT_BUTTON(ID_BTN_START, Main::StartStopButtonClicked)
	EVT_CLOSE(Main::OnClose)
wxEND_EVENT_TABLE()

Main::Main() : wxFrame(nullptr, wxID_ANY, APP_NAME)
{
#ifdef _DEBUG
	AllocConsole();
	FILE* fDummy;
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
#endif

	// Init window
	wxDisplay display(wxDisplay::GetFromWindow(this));
	wxRect screen = display.GetClientArea();
	this->SetInitialSize(wxSize(APP_WIDTH, APP_HEIGHT));
	this->SetPosition(wxPoint(screen.width / 2 - APP_WIDTH / 2, screen.height / 2 - APP_HEIGHT / 2));
	this->SetWindowStyle(this->GetWindowStyle() & ~wxMAXIMIZE_BOX); // Disable maximize button

	// Init widgets
	m_taskBarIcon = new TaskBarIcon(this);
	wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);

	m_btnStartStop = new wxButton(this, ID_BTN_START, "Ping!");
	m_txtTarget = new wxTextCtrl(this, wxID_ANY, DEFAULT_TARGET);

	hbox1->Add(new wxStaticText(this, wxID_ANY, "Target:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, BORDER_WIDTH);
	hbox1->Add(m_txtTarget, 0, wxALL, BORDER_WIDTH);
	hbox1->Add(m_btnStartStop, 0, wxALL, BORDER_WIDTH);

	m_labPacketLoss = new wxStaticText(this, wxID_ANY, FORMAT_PLOSS(0));
	m_labPing = new wxStaticText(this, wxID_ANY, FORMAT_PING(0));
	hbox2->Add(new wxStaticText(this, wxID_ANY, "Packet loss:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, BORDER_WIDTH);
	hbox2->Add(m_labPacketLoss, 0, wxALL | wxALIGN_CENTER_VERTICAL, BORDER_WIDTH);
	hbox2->Add(new wxStaticText(this, wxID_ANY, "Ping:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, BORDER_WIDTH);
	hbox2->Add(m_labPing, 0, wxALL | wxALIGN_CENTER_VERTICAL, BORDER_WIDTH);

	vbox->Add(hbox1);
	vbox->Add(hbox2);

	SetSizerAndFit(vbox);

	// Init other stuff
	m_timer.Bind(wxEVT_TIMER, &Main::OnTimer, this);

	m_ploss = new Stats(STATS_SAMPLES_NUM);
	m_latency = new Stats(STATS_SAMPLES_NUM);
}

Main::~Main()
{
	delete m_btnStartStop, m_txtTarget, m_txtTarget;
	delete m_labPacketLoss, m_labPing, m_ploss;
	delete m_taskBarIcon;
}

void Main::OnTimer(wxTimerEvent& event)
{
	PingResult_t result = ping(m_txtTarget->GetValue().ToStdString());

	if (result.status)
	{
		m_latency->Update(result.latency);
		m_ploss->Update(0);
	}
	else
	{
		m_ploss->Update(1);
	}

	int pl = m_ploss->GetAverage() * 100;
	int ping = m_latency->GetAverage();
	m_labPacketLoss->SetLabel(FORMAT_PLOSS(pl));
	m_labPing->SetLabel(FORMAT_PING(ping));
	// constrain value to range <0;99> for optimal text size on icon
	m_taskBarIcon->SetIconFromNumber(std::clamp(pl, 0, 99));
}

void Main::StartStopButtonClicked(wxCommandEvent& event)
{
	if (m_timer.IsRunning())
	{
		m_timer.Stop();
		m_btnStartStop->SetLabel("Ping!");
		m_txtTarget->Enable(true);
	}
	else
	{
		m_timer.Start(PING_PERIOD);
		m_btnStartStop->SetLabel("Stop");
		m_txtTarget->Enable(false);
	}

	event.Skip();
}

std::string Main::FormatVal(float avg, std::string suffix)
{
	return std::to_string((int)avg) + suffix;
}

void Main::OnClose(wxCloseEvent& event)
{
	Show(false);
}