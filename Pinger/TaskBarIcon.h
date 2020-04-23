#pragma once

#include <wx/taskbar.h>

class TaskBarIcon : public wxTaskBarIcon
{
public:
  TaskBarIcon(wxFrame* frame);
  void SetIconFromNumber(int n);

private:
  wxFrame* m_frame = nullptr;

  void OnLeftDoubleClick(wxTaskBarIconEvent& event);
  void OnMenuExit(wxCommandEvent& event);
  virtual wxMenu* CreatePopupMenu() wxOVERRIDE;

  DECLARE_EVENT_TABLE();
};

