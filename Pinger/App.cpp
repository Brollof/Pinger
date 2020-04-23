#include "App.h"

wxIMPLEMENT_APP(App);

App::App()
{

}

App::~App()
{
	delete m_frame;
}

bool App::OnInit()
{
	wxImage::AddHandler(new wxJPEGHandler);
	m_frame = new Main();
	m_frame->Show();
	return true;
}