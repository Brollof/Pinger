#include <wx/display.h>
#include <iostream>
#include "Main.h"
#include "Ping.h"
#include "Version.h"

#define APP_WIDTH 200
#define APP_HEIGHT 100
#define BORDER_WIDTH 4
#define DEFAULT_TARGET "wp.pl"
#define ID_BTN_START 500
#define FORMAT_PLOSS(val) FormatVal(val, " %")
#define FORMAT_PING(val) FormatVal(val, " ms")
#define PERIOD_MIN 1
#define PERIOD_MAX 60000
#define SAMPLES_MIN 1
#define SAMPLES_MAX 1000

wxBEGIN_EVENT_TABLE(Main, wxFrame)
  EVT_BUTTON(ID_BTN_START, Main::StartStopButtonClicked)
  EVT_CLOSE(Main::OnClose)
wxEND_EVENT_TABLE()

Main::Main(std::string appName) : wxFrame(nullptr, wxID_ANY, appName)
{
  // Init window
  wxDisplay display(wxDisplay::GetFromWindow(this));
  wxRect screen = display.GetClientArea();
  this->SetInitialSize(wxSize(APP_WIDTH, APP_HEIGHT));
  this->SetPosition(wxPoint(screen.width / 2 - APP_WIDTH / 2, screen.height / 2 - APP_HEIGHT / 2));
  this->SetWindowStyle(this->GetWindowStyle() & ~wxMAXIMIZE_BOX); // Disable maximize button

  // Init widgets
  m_taskBarIcon = new TaskBarIcon(this);
  wxBoxSizer* boxMain = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* boxTop = new wxBoxSizer(wxHORIZONTAL);
  wxFlexGridSizer* boxLeft = new wxFlexGridSizer(2, 0, 0);
  wxBoxSizer* boxRight = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* boxBottom = new wxBoxSizer(wxHORIZONTAL);

  boxMain->Add(boxTop);
  boxMain->Add(boxBottom);
  boxTop->Add(boxLeft);
  boxTop->Add(boxRight);

  m_btnStartStop = new wxButton(this, ID_BTN_START, "Ping!");
  m_txtTarget = new wxTextCtrl(this, wxID_ANY, DEFAULT_TARGET);
  m_txtSamples = new wxTextCtrl(this, wxID_ANY, "10", wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_DIGITS));
  m_txtPeriod = new wxTextCtrl(this, wxID_ANY, "1000", wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_DIGITS));

  boxLeft->Add(new wxStaticText(this, wxID_ANY, "Target:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, BORDER_WIDTH);
  boxLeft->Add(m_txtTarget, 0, wxALL, BORDER_WIDTH);

  boxLeft->Add(new wxStaticText(this, wxID_ANY, "Samples:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, BORDER_WIDTH);
  boxLeft->Add(m_txtSamples, 0, wxALL, BORDER_WIDTH);

  boxLeft->Add(new wxStaticText(this, wxID_ANY, "Period [ms]:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, BORDER_WIDTH);
  boxLeft->Add(m_txtPeriod, 0, wxALL, BORDER_WIDTH);

  boxRight->Add(m_btnStartStop, 0, wxALL, BORDER_WIDTH);

  m_labPacketLoss = new wxStaticText(this, wxID_ANY, FORMAT_PLOSS(0));
  m_labPing = new wxStaticText(this, wxID_ANY, FORMAT_PING(0));
  boxBottom->Add(new wxStaticText(this, wxID_ANY, "Packet loss:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, BORDER_WIDTH);
  boxBottom->Add(m_labPacketLoss, 0, wxALL | wxALIGN_CENTER_VERTICAL, BORDER_WIDTH);
  boxBottom->Add(new wxStaticText(this, wxID_ANY, "Ping:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, BORDER_WIDTH);
  boxBottom->Add(m_labPing, 0, wxALL | wxALIGN_CENTER_VERTICAL, BORDER_WIDTH);

  SetSizerAndFit(boxMain);

  // Init other stuff
  m_timer.Bind(wxEVT_TIMER, &Main::OnTimer, this);
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

bool Main::ValidateInput(int val, int min, int max)
{
  return (min <= val && val <= max);
}

void Main::StartStopButtonClicked(wxCommandEvent& event)
{
  event.Skip();

  if (m_timer.IsRunning())
  {
    m_timer.Stop();
    m_btnStartStop->SetLabel("Ping!");
    m_txtTarget->Enable(true);
    m_txtSamples->Enable(true);
    m_txtPeriod->Enable(true);
  }
  else
  {
    unsigned long samples = 0, period = 0;
    m_txtSamples->GetValue().ToULong(&samples);
    m_txtPeriod->GetValue().ToULong(&period);

    if (!ValidateInput(samples, SAMPLES_MIN, SAMPLES_MAX) || !ValidateInput(period, PERIOD_MIN, PERIOD_MAX))
    {
      wxString msg;
      msg.Printf(wxT("Samples number must be in range [%d; %d]\n"
        "Period must be in range [%d; %d]"),
        SAMPLES_MIN, SAMPLES_MAX, PERIOD_MIN, PERIOD_MAX);
      wxMessageBox(msg, "Invalid input", wxOK | wxICON_EXCLAMATION | wxCENTRE);
      return;
    }

    std::cout << "Starting timer with period: " << period << " ms" << std::endl;
    std::cout << "Samples for statistics: " << samples << std::endl;

    InitStats(samples);
    m_timer.Start(period);
    m_btnStartStop->SetLabel("Stop");
    m_txtTarget->Enable(false);
    m_txtSamples->Enable(false);
    m_txtPeriod->Enable(false);
  }
}

std::string Main::FormatVal(float avg, std::string suffix)
{
  return std::to_string((int)avg) + suffix;
}

void Main::OnClose(wxCloseEvent& event)
{
  Show(false);
}

void Main::InitStats(int samples)
{
  if (m_ploss)
  {
    delete m_ploss;
  }
  if (m_latency)
  {
    delete m_latency;
  }
  m_ploss = new Stats(samples);
  m_latency = new Stats(samples);
}