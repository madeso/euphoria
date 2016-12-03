#include "scimed/wx.h"
#include <wx/sizer.h>

// todo:
// add loading, zooming and panning in image
// add loading/saving and specifying areas
// add preview pane


class ImagePanel : public wxPanel
{
  wxImage image;
  wxBitmap resized;
  int w, h;
  bool displayImage_;

 public:
  explicit ImagePanel(wxFrame* parent)
      : wxPanel(parent), displayImage_(false)
  {
    Bind(wxEVT_SIZE, &ImagePanel::OnSize, this);
    Bind(wxEVT_PAINT, &ImagePanel::OnPaint, this);
  }

  bool LoadImage(wxString file, wxBitmapType format) {
    displayImage_ = image.LoadFile(file, format);
    w = -1;
    h = -1;
    // displayImage_ = true;

    if( displayImage_ ) {
      Refresh();
    }

    return displayImage_;
  }

  void OnPaint(wxPaintEvent & evt) {
    if(displayImage_ == false) {
      return;
    }

    wxPaintDC dc(this);

    int neww, newh;
    dc.GetSize( &neww, &newh );

    if( neww != w || newh != h ) {
      resized = wxBitmap( image.Scale( neww, newh /*, wxIMAGE_QUALITY_HIGH*/ ) );
      w = neww;
      h = newh;
      dc.DrawBitmap( resized, 0, 0, false );
    }
    else {
      dc.DrawBitmap( resized, 0, 0, false );
    }
  }

  void OnSize(wxSizeEvent& event){
    Refresh();
    //skip the event.
    event.Skip();
  }

  // some useful events
  /*
   void mouseMoved(wxMouseEvent& event);
   void mouseDown(wxMouseEvent& event);
   void mouseWheelMoved(wxMouseEvent& event);
   void mouseReleased(wxMouseEvent& event);
   void rightClick(wxMouseEvent& event);
   void mouseLeftWindow(wxMouseEvent& event);
   void keyPressed(wxKeyEvent& event);
   void keyReleased(wxKeyEvent& event);
   */
};

// some useful events
/*
 void ImagePanel::mouseMoved(wxMouseEvent& event) {}
 void ImagePanel::mouseDown(wxMouseEvent& event) {}
 void ImagePanel::mouseWheelMoved(wxMouseEvent& event) {}
 void ImagePanel::mouseReleased(wxMouseEvent& event) {}
 void ImagePanel::rightClick(wxMouseEvent& event) {}
 void ImagePanel::mouseLeftWindow(wxMouseEvent& event) {}
 void ImagePanel::keyPressed(wxKeyEvent& event) {}
 void ImagePanel::keyReleased(wxKeyEvent& event) {}
 */

class MyFrame: public wxFrame
{
 public:
  MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(NULL, wxID_ANY, title, pos, size), title_(title) {
    SetMenuBar(CreateMenuBar());
    CreateStatusBar();
    SetStatusText("");

    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    drawPane = new ImagePanel(this);
    sizer->Add(drawPane, 1, wxEXPAND);
    SetSizer(sizer);

    Bind(wxEVT_COMMAND_MENU_SELECTED, &MyFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MyFrame::OnSave, this, wxID_SAVE);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MyFrame::OnSaveAs, this, wxID_SAVEAS);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MyFrame::OnOpen, this, wxID_OPEN);
  }

 private:
  ImagePanel * drawPane;
  wxString title_;
  wxString filename_;

  void UpdateTitle() {
    if( filename_.IsEmpty() ) {
      SetTitle( title_ );
    }
    else {
      SetTitle( title_ + " - " + filename_ );
    }
  }

  wxMenu* CreateFileMenu() {
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(wxID_OPEN);
    menuFile->Append(wxID_SAVE);
    menuFile->Append(wxID_SAVEAS);
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    return menuFile;
  }

  wxMenu* CreateHelpMenu() {
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    return menuHelp;
  }

  wxMenuBar* CreateMenuBar() {
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(CreateFileMenu(), "&File");
    menuBar->Append(CreateHelpMenu(), "&Help");
    return menuBar;
  }

  void OnSave(wxCommandEvent& event){
    if( filename_.IsEmpty() ) {
      OnSaveAs(event);
      return;
    }

    // do saving...
  }

  void OnSaveAs(wxCommandEvent& event) {
    wxFileDialog saveFileDialog(this, _("Save scim file"), "", "", "SCIM files (*.png)|*.png", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL) {
      return;
    }
    filename_ = saveFileDialog.GetPath();
    UpdateTitle();
    OnSave(event);
  }

  void OnOpen(wxCommandEvent& event) {
    wxFileDialog openFileDialog(this, _("Open scim file"), "", "", "SCIM files (*.png)|*.png", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL) {
      return;
    }

    if( drawPane->LoadImage(openFileDialog.GetPath(), wxBITMAP_TYPE_PNG) ) {
      filename_ = openFileDialog.GetPath();
    }
    else {
      filename_ = "";
    }
    UpdateTitle();
  }

  void OnExit(wxCommandEvent& event){
    Close(true);
  }

  void OnAbout(wxCommandEvent& event){
    wxMessageBox("Scimed - the Scalable Image Editor", "About Scimed", wxOK | wxICON_INFORMATION);
  }
};

////////////////////////////////////////////////////////////////////////////////

class MyApp: public wxApp
{
 public:
  virtual bool OnInit() {
    wxInitAllImageHandlers();
    MyFrame *frame = new MyFrame( "Scimed", wxPoint(50, 50), wxSize(450, 340) );
    frame->Show( true );
    return true;
  }
};
wxIMPLEMENT_APP(MyApp);
