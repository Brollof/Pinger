#pragma once

#include <wx/taskbar.h>

class TaskBarIcon : public wxTaskBarIcon
{
public:
  TaskBarIcon(wxFrame* frame);
  void OnLeftDoubleClick(wxTaskBarIconEvent& event);
  void OnMenuExit(wxCommandEvent& event);
  virtual wxMenu* CreatePopupMenu() wxOVERRIDE;

private:
  wxFrame* m_frame = nullptr;
  DECLARE_EVENT_TABLE();
};

