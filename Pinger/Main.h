#pragma once

#include "wx/wx.h"
#include "wx/time.h"
#include "wx/file.h"
#include "Stats.h"
#include "TaskBarIcon.h"

class Main : public wxFrame
{
public:
  Main(std::string appName);
  ~Main();
  void OnButtonRun(wxCommandEvent& event);

private:
  wxButton* m_btnStartStop = nullptr;
  wxButton* m_btnExit = nullptr;
  wxTextCtrl* m_txtTarget = nullptr;
  wxTextCtrl* m_txtSamples = nullptr;
  wxTextCtrl* m_txtPeriod = nullptr;
  wxStaticText* m_labPacketLoss = nullptr;
  wxStaticText* m_labPing = nullptr;
  TaskBarIcon* m_taskBarIcon = nullptr;
  wxCheckBox* m_checkBox = nullptr;
  wxFile* m_logFile = nullptr;

  wxTimer m_timer;
  Stats* m_ploss;
  Stats* m_latency;

  void OnCheckbox(wxCommandEvent& event);
  void OnButtonExit(wxCommandEvent& event);
  void OnTimer(wxTimerEvent& event);
  void OnClose(wxCloseEvent& event);
  std::string FormatVal(float avg, std::string suffix);
  void InitStats(int samples);
  bool ValidateInput(int val, int min, int max);

  wxDECLARE_EVENT_TABLE();
};