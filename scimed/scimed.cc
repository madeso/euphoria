#include "scimed/wx.h"

class MyApp: public wxApp
{
 public:
  virtual bool OnInit();
};

class MyFrame: public wxFrame
{
 public:
  MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
 private:
  void OnHello(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
};

enum
{
  ID_Hello = 1
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
  MyFrame *frame = new MyFrame( "Hello World", wxPoint(50, 50), wxSize(450, 340) );
  frame->Show( true );
  return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
  wxMenu *menuFile = new wxMenu;
  menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                   "Help string shown in status bar for this menu item");
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT);
  wxMenu *menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append( menuFile, "&File" );
  menuBar->Append( menuHelp, "&Help" );
  SetMenuBar( menuBar );
  CreateStatusBar();
  SetStatusText( "Welcome to wxWidgets!" );

  Bind(wxEVT_COMMAND_MENU_SELECTED, &MyFrame::OnExit, this, wxID_EXIT);
  Bind(wxEVT_COMMAND_MENU_SELECTED, &MyFrame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_COMMAND_MENU_SELECTED, &MyFrame::OnHello, this, ID_Hello);
}

void MyFrame::OnExit(wxCommandEvent& event)
{
  Close( true );
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
  wxMessageBox( "This is a wxWidgets' Hello world sample",
                "About Hello World", wxOK | wxICON_INFORMATION );
}

void MyFrame::OnHello(wxCommandEvent& event)
{
  wxLogMessage("Hello world from wxWidgets!");
}