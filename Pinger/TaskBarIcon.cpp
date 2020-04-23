#include <wx/wx.h>
#include <wx/taskbar.h>
#include <wx/icon.h> 
#include <wx/dc.h>
#include <wx/image.h>
#include <string>
#include "TaskBarIcon.h"

#define TBI_ID_EXIT 10001
#define ICON_SIZE 32

BEGIN_EVENT_TABLE(TaskBarIcon, wxTaskBarIcon)
  EVT_MENU(TBI_ID_EXIT, TaskBarIcon::OnMenuExit)
  EVT_TASKBAR_LEFT_DCLICK(TaskBarIcon::OnLeftDoubleClick)
END_EVENT_TABLE()

TaskBarIcon::TaskBarIcon(wxFrame* window) : wxTaskBarIcon()
{
  m_frame = window;
  SetIconFromNumber(0);
}

void TaskBarIcon::OnLeftDoubleClick(wxTaskBarIconEvent& event)
{
  if (m_frame)
  {
    m_frame->Show();
  }
  event.Skip();
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

void TaskBarIcon::SetIconFromNumber(int n)
{
  wxBitmap bm(ICON_SIZE, ICON_SIZE);
  wxMemoryDC dc(bm);

  dc.SetTextBackground(*wxBLACK);
  dc.SetTextForeground(*wxWHITE);

  wxFont font = dc.GetFont();
  font.Scale(2.0);
  font.SetWeight(wxFONTWEIGHT_BOLD);
  dc.SetFont(font);

  wxRect rect = wxRect(0, 0, ICON_SIZE, ICON_SIZE);
  dc.DrawLabel(std::to_string(n), rect, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
  dc.SelectObject(wxNullBitmap);

  wxIcon icon = wxIcon();
  icon.CopyFromBitmap(bm);
  SetIcon(icon);
}