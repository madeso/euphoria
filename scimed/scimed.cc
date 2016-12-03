#include "scimed/wx.h"
#include <wx/sizer.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>

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
  float scale_;

  int left;
  int right;
  int top;
  int bottom;

 public:
  explicit ImagePanel(wxFrame* parent)
      : wxPanel(parent), displayImage_(false), scale_(8.0f), left(10), right(10), top(10), bottom(10)
  {
    Bind(wxEVT_SIZE, &ImagePanel::OnSize, this);
    Bind(wxEVT_PAINT, &ImagePanel::OnPaint, this);
    Bind(wxEVT_MOUSEWHEEL, &ImagePanel::OnMouseWheel, this);
  }

  void OnMouseWheel(const wxMouseEvent& me) {
    if( displayImage_ == false ) {
      return;
    }
    if( me.GetWheelAxis() != wxMOUSE_WHEEL_VERTICAL) {
      return;
    }
    const float scroll = me.GetWheelRotation()/static_cast<float>(me.GetWheelDelta());
    scale_ =  std::max(0.1f, std::min(scale_ + scroll * 0.1f, 10.0f));

    Refresh();
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

  static void VerticalLine(wxDC& dc, int left) {
    int window_width, window_height;
    dc.GetSize( &window_width, &window_height );
    dc.DrawLine(left, 0, left, window_height);
  }

  static void HorizontalLine(wxDC& dc, int y) {
    int window_width, window_height;
    dc.GetSize( &window_width, &window_height );
    dc.DrawLine(0, y, window_width, y);
  }

  void OnPaint(wxPaintEvent & evt) {
    const bool useBuffer = false;

    if ( useBuffer )
    {
      wxBufferedPaintDC bpdc(this);
      Draw(bpdc);
    }
    else
    {
      wxPaintDC pdc(this);
      Draw(pdc);
    }
  }

  void Draw(wxDC& pdc) {
    wxGCDC gdc;

    if(displayImage_ == false) {
      return;
    }

    wxGraphicsRenderer* renderer = wxGraphicsRenderer::GetDefaultRenderer();
    assert(renderer);

    wxGraphicsContext* context;
    if ( wxPaintDC *paintdc = wxDynamicCast(&pdc, wxPaintDC) )
    {
      context = renderer->CreateContext(*paintdc);
    }
    else if ( wxMemoryDC *memdc = wxDynamicCast(&pdc, wxMemoryDC) )
    {
      context = renderer->CreateContext(*memdc);
    }
    else
    {
      wxFAIL_MSG( "Unknown wxDC kind" );
      return;
    }

    gdc.SetGraphicsContext(context);
    wxDC &dc = (wxDC&) gdc;

    int window_width, window_height;
    dc.GetSize( &window_width, &window_height );

    const int w = static_cast<int>(image.GetWidth() * scale_);
    const int h = static_cast<int>(image.GetHeight() * scale_);

    const int image_x = (window_width - w)/2;
    const int image_y = (window_height - h)/2;

    DrawImage(dc, image_x, image_y, w, h);

    const bool draw_ruler = true;
    const bool draw_guides = true;

    if( draw_guides ) {
      wxPen guide_pen(wxColour(0, 255, 0));
      guide_pen.SetStyle(wxPENSTYLE_SHORT_DASH);
      dc.SetPen(guide_pen);

      VerticalLine(dc, static_cast<int>(image_x + left * scale_));
      VerticalLine(dc, static_cast<int>(image_x + (image.GetWidth()-right) * scale_));

      HorizontalLine(dc, static_cast<int>(image_y + top * scale_));
      HorizontalLine(dc, static_cast<int>(image_y + (image.GetHeight()-bottom) * scale_));
    }


    if( draw_ruler ) {
      const int ruler_size = 20;
      const int big_mark_size = 15;
      const int small_mark_size = 5;
      const int mark_index = 5;

      wxBrush ruler_background(wxColour(246, 247, 249), wxSOLID);
      wxPen mark_color(wxColour(0, 0, 0), 1, wxSOLID);
      wxFont ruler_font(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

      dc.SetBrush(ruler_background);
      dc.SetPen(*wxTRANSPARENT_PEN);
      dc.DrawRectangle(0, 0, ruler_size, window_height);
      dc.DrawRectangle(0, 0, window_width, ruler_size);

      dc.SetPen(mark_color);
      dc.DrawLine(0, ruler_size, window_width, ruler_size);
      dc.DrawLine(ruler_size, 0, ruler_size, window_height);

      dc.SetFont(ruler_font);
      DrawTopRuler(dc, image_x, mark_index, big_mark_size, small_mark_size, ruler_size, 0, window_width, scale_, 1);
      DrawTopRuler(dc, image_x, mark_index, big_mark_size, small_mark_size, ruler_size, 0, window_width, scale_, -1);

      DrawLeftRuler(dc, image_y, mark_index, big_mark_size, small_mark_size, ruler_size, 0, window_height, scale_, 1);
      DrawLeftRuler(dc, image_y, mark_index, big_mark_size, small_mark_size, ruler_size, 0, window_height, scale_, -1);

      dc.SetPen(*wxTRANSPARENT_PEN);
      dc.DrawRectangle(0, 0, ruler_size, ruler_size);
      dc.SetPen(mark_color);
      wxString zoom_text = wxString::Format("%.0fx", scale_);
      const int zoom_x = (ruler_size - dc.GetTextExtent(zoom_text).GetWidth()) / 2;
      dc.DrawText(zoom_text, zoom_x, 4);
    }
  }

  static void DrawTopRuler(wxDC& dc, int image_x, int mark_index, int big_mark_size, int small_mark_size, int ruler_size, int start_index, int end_index, float scale_, int step) {
    for(int index=start_index; true; index+=step) {
      const int x = image_x + static_cast<int>(index * scale_);
      if( x < 0 && step < 0 ) return;
      if( x > end_index ) return;
      const bool big_size = index % mark_index== 0;
      const int size = (big_size)?big_mark_size:small_mark_size;
      dc.DrawLine(x, ruler_size-size, x, ruler_size);

      if( big_size ) {
        wxString text;
        text = wxString::Format("%d", index);
        dc.DrawText(text, x+3, 0);
      }
    }
  }

  static void DrawLeftRuler(wxDC& dc, int image_y, int mark_index, int big_mark_size, int small_mark_size, int ruler_size, int start_index, int end_index, float scale_, int step) {
    for(int index=start_index; true; index+=step) {
      const int y = image_y + static_cast<int>(index * scale_);
      if( y < 0 && step < 0 ) return;
      if( y > end_index ) return;
      const bool big_size = index % mark_index== 0;
      const int size = (big_size)?big_mark_size:small_mark_size;
      dc.DrawLine(ruler_size-size, y, ruler_size, y);

      if( big_size ) {
        wxString text;
        text = wxString::Format("%d", index);
        const wxSize extent = dc.GetTextExtent(text);
        dc.DrawRotatedText(text, 3, y + 3 + extent.GetWidth(), 90);
      }
    }
  }

  void DrawImage(wxDC& dc, int x, int y, int neww, int newh) {
    if( neww != w || newh != h ) {
      resized = wxBitmap( image.Scale( neww, newh /*, wxIMAGE_QUALITY_HIGH*/ ) );
      w = neww;
      h = newh;
      dc.DrawBitmap( resized, x, y, false );
    }
    else {
      dc.DrawBitmap( resized, x, y, false );
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
