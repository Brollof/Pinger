#include "Main.h"
#include <wx/display.h>
#include <iostream>

#include "Ping.h"

#define APP_WIDTH 200
#define APP_HEIGHT 100
#define BORDER 4

enum IDS
{
	ID_BTN_START = 500,
	ID_BTN_STOP,
};

wxBEGIN_EVENT_TABLE(Main, wxFrame)
	EVT_BUTTON(ID_BTN_START, StartButtonClicked)
	EVT_BUTTON(ID_BTN_STOP, StopButtonClicked)
wxEND_EVENT_TABLE()

Main::Main() : wxFrame(nullptr, wxID_ANY, "lol")
{
#ifdef _DEBUG
	AllocConsole();
	FILE* fDummy;
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
#endif
	std::cout << "Init" << std::endl;

	// Init window
	wxDisplay display(wxDisplay::GetFromWindow(this));
	wxRect screen = display.GetClientArea();
	this->SetInitialSize(wxSize(APP_WIDTH, APP_HEIGHT));
	this->SetPosition(wxPoint(screen.width / 2 - APP_WIDTH / 2, screen.height / 2 - APP_HEIGHT / 2));

	// Init widgets
	wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);

	m_btnStart = new wxButton(this, ID_BTN_START, "Ping!");
	m_btnStop = new wxButton(this, ID_BTN_STOP, "Stop");
	m_btnStop->Enable(false);
	m_txtTarget = new wxTextCtrl(this, wxID_ANY);

	hbox1->Add(new wxStaticText(this, wxID_ANY, "Target:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, BORDER);
	hbox1->Add(m_txtTarget, 0, wxALL, BORDER);
	hbox1->Add(m_btnStart, 0, wxALL, BORDER);
	hbox1->Add(m_btnStop, 0, wxALL, BORDER);

	m_labPacketLoss = new wxStaticText(this, wxID_ANY, "0");
	hbox2->Add(new wxStaticText(this, wxID_ANY, "Packet loss:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, BORDER);
	hbox2->Add(m_labPacketLoss, 0, wxALL | wxALIGN_CENTER_VERTICAL, BORDER);

	vbox->Add(hbox1);
	vbox->Add(hbox2);

	SetSizerAndFit(vbox);
}

Main::~Main()
{

}

void Main::StartButtonClicked(wxCommandEvent& event)
{
	ping(m_txtTarget->GetValue().ToStdString());
	//m_isStarted = true;
	//m_btnStart->Enable(false);
	//m_btnStop->Enable(true);
	event.Skip();
}

void Main::StopButtonClicked(wxCommandEvent& event)
{
	m_isStarted = false;
	m_btnStart->Enable(true);
	m_btnStop->Enable(false);
	event.Skip();
}