#include <wx/wx.h>
#include <wx/taskbar.h>
#include <wx/icon.h> 

static const char* const sample_xpm[] = {
  /* columns rows colors chars-per-pixel */
  "32 32 6 1",
  "  c black",
  ". c navy",
  "X c red",
  "o c yellow",
  "O c gray100",
  "+ c None",
  /* pixels */
  "++++++++++++++++++++++++++++++++",
  "++++++++++++++++++++++++++++++++",
  "++++++++++++++++++++++++++++++++",
  "++++++++++++++++++++++++++++++++",
  "++++++++++++++++++++++++++++++++",
  "++++++++              ++++++++++",
  "++++++++ ............ ++++++++++",
  "++++++++ ............ ++++++++++",
  "++++++++ .OO......... ++++++++++",
  "++++++++ .OO......... ++++++++++",
  "++++++++ .OO......... ++++++++++",
  "++++++++ .OO......              ",
  "++++++++ .OO...... oooooooooooo ",
  "         .OO...... oooooooooooo ",
  " XXXXXXX .OO...... oOOooooooooo ",
  " XXXXXXX .OO...... oOOooooooooo ",
  " XOOXXXX ......... oOOooooooooo ",
  " XOOXXXX ......... oOOooooooooo ",
  " XOOXXXX           oOOooooooooo ",
  " XOOXXXXXXXXX ++++ oOOooooooooo ",
  " XOOXXXXXXXXX ++++ oOOooooooooo ",
  " XOOXXXXXXXXX ++++ oOOooooooooo ",
  " XOOXXXXXXXXX ++++ oooooooooooo ",
  " XOOXXXXXXXXX ++++ oooooooooooo ",
  " XXXXXXXXXXXX ++++              ",
  " XXXXXXXXXXXX ++++++++++++++++++",
  "              ++++++++++++++++++",
  "++++++++++++++++++++++++++++++++",
  "++++++++++++++++++++++++++++++++",
  "++++++++++++++++++++++++++++++++",
  "++++++++++++++++++++++++++++++++",
  "++++++++++++++++++++++++++++++++"
};

#include "TaskBarIcon.h"

#define TBI_ID_EXIT 10001

BEGIN_EVENT_TABLE(TaskBarIcon, wxTaskBarIcon)
  EVT_MENU(TBI_ID_EXIT, TaskBarIcon::OnMenuExit)
  EVT_TASKBAR_LEFT_DCLICK(TaskBarIcon::OnLeftDoubleClick)
END_EVENT_TABLE()

TaskBarIcon::TaskBarIcon(wxFrame* window) : wxTaskBarIcon()
{
  m_frame = window;
  SetIcon(wxIcon(sample_xpm));
}

void TaskBarIcon::OnLeftDoubleClick(wxTaskBarIconEvent& event)
{
  if (m_frame)
  {
    m_frame->Show();
  }
}

void TaskBarIcon::OnMenuExit(wxCommandEvent& event)
{
  m_frame->Destroy();
}

wxMenu* TaskBarIcon::CreatePopupMenu()
{
  wxMenu* menu = new wxMenu;
  menu->Append(TBI_ID_EXIT, "Exit");
  return menu;
}