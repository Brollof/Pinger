#include <wx/wx.h>
#include <wx/taskbar.h>
#include <wx/icon.h> 
#include <wx/dc.h>
#include <wx/image.h>
#include <string>
#include "TaskBarIcon.h"
#include "Common.h"

#define ICON_SIZE         32
#define ACTIVE_COLOR      wxColour("GREEN")
#define INACTIVE_COLOR    wxColour("GREY")

BEGIN_EVENT_TABLE(TaskBarIcon, wxTaskBarIcon)
  EVT_MENU(ID_TRAY_BTN_EXIT, TaskBarIcon::OnMenuExit)
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
  menu->Append(ID_TRAY_BTN_EXIT, "Exit");
  return menu;
}

void TaskBarIcon::SetIconFromNumber(int n)
{
  m_num = n;
  wxBitmap bm(ICON_SIZE, ICON_SIZE);
  wxMemoryDC dc(bm);

  dc.SetTextForeground(m_isActive ? ACTIVE_COLOR : INACTIVE_COLOR);

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

void TaskBarIcon::SetActive(bool isActive)
{
  m_isActive = isActive;
  SetIconFromNumber(m_num);
}