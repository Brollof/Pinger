#pragma once

#include <wx/taskbar.h>

class Main;

class TaskBarIcon : public wxTaskBarIcon
{
public:
  TaskBarIcon(Main* frame);
  void SetIconFromNumber(int n);
  void SetActive(bool isActive);

private:
  Main* m_frame = nullptr;
  bool m_isActive = false;
  int m_num = 0;
  void OnLeftDoubleClick(wxTaskBarIconEvent& event);
  void OnMenuExit(wxCommandEvent& event);
  void OnMenuRun(wxCommandEvent& event);
  virtual wxMenu* CreatePopupMenu() wxOVERRIDE;

  DECLARE_EVENT_TABLE();
};