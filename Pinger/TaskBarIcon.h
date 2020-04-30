#pragma once

#include <wx/taskbar.h>

class TaskBarIcon : public wxTaskBarIcon
{
public:
  TaskBarIcon(wxFrame* frame);
  void SetIconFromNumber(int n);
  void SetActive(bool isActive);

private:
  wxFrame* m_frame = nullptr;
  bool m_isActive = false;
  int m_num = 0;
  void OnLeftDoubleClick(wxTaskBarIconEvent& event);
  void OnMenuExit(wxCommandEvent& event);
  virtual wxMenu* CreatePopupMenu() wxOVERRIDE;

  DECLARE_EVENT_TABLE();
};