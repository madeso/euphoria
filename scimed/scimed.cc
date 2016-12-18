#include "scimed/wx.h"
#include <wx/sizer.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>

#include <vector>
#include <sstream>

#include "scalingsprite.pb.h"
#include "scimed/wxproto.h"

class Line {
 public:
  static Line Null() { return Line(); }

  operator bool() const {
    return isValid_;
  }
 private:
  Line() : isValid_(false) {
  }
  bool isValid_;
};

class TextRenderData {
 public:
  TextRenderData(std::string t, int aleft, int aright) : text(t), position(aleft + (aright-aleft) / 2), left(aleft), right(aright) {}
  std::string text;
  int position;
  int left;
  int right;
};

class Data {
 public:
  int GetTotalPercentage() const {
    int total = 0;
    for(int i: data) {
      if( i < 0 ) total += -i;
    }
    return total;
  }
  Line GetTracking(int d) const {
    return Line::Null();
  }
  std::vector<TextRenderData> GetText() const {
    std::vector<TextRenderData> ret;
    const int total_percentage = GetTotalPercentage();
    int x = 0;
    for(int i: data) {
      const int d = abs(i);
      std::ostringstream ss;
      if( i > 0) ss << i << "px";
      else ss << static_cast<float>(-i*100.0f/total_percentage) << "%";
      ret.push_back(TextRenderData(ss.str(), x, x + d));
      x += d;
    }
    return ret;
  }

  // lines lie between datapoints
  std::vector<float> GetLines() const {
    std::vector<float> ret;
    bool has_data = false;
    int x = 0;
    for(int i: data) {
      int dx = std::abs(i);
      if( has_data ) {
        ret.push_back(x);
      }
      else {
        has_data = true;
      }
      x += dx;
    }
    return ret;
  }
  std::vector<int> data;
};


class ImagePanel : public wxPanel
{
  wxImage image;
  wxBitmap resized;
  int w, h;
  bool displayImage_;
  float scale_;

  wxStockCursor last_cursor_;
  bool left_mouse;
  bool last_left_mouse;

  int image_x;
  int image_y;

  Data col;
  Data row;
  Line track_col;
  Line track_row;

 public:
  explicit ImagePanel(wxFrame* parent)
      : wxPanel(parent), displayImage_(false), scale_(8.0f), last_cursor_(wxCURSOR_ARROW), left_mouse(false), last_left_mouse(false)
      , image_x(0), image_y(0)
      , track_col(Line::Null()), track_row(Line::Null())
  {
    Bind(wxEVT_SIZE, &ImagePanel::OnSize, this);
    Bind(wxEVT_PAINT, &ImagePanel::OnPaint, this);
    Bind(wxEVT_MOUSEWHEEL, &ImagePanel::OnMouseWheel, this);
    Bind(wxEVT_MOTION, &ImagePanel::OnMouseMove, this);
    Bind(wxEVT_LEFT_DOWN, &ImagePanel::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &ImagePanel::OnMouseUp, this);
  }

  void GetRect(scalingsprite::ScalingSprite* r) {
  }

  void SetRect(const scalingsprite::ScalingSprite& r) {
    row.data.clear();
    row.data.push_back(12);
    row.data.push_back(-20);
    row.data.push_back(30);

    col.data.clear();
    col.data.push_back(12);
    col.data.push_back(-10);
    col.data.push_back(20);

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

 private:

  static bool KindaTheSame(int lhs, int rhs) {
    return std::abs(lhs-rhs) < 5;
  }

  void OnMouseUp(const wxMouseEvent& me) {
    left_mouse = false;
    OnMouse(me);
    OnMouse(me);
  }

  void OnMouseDown(const wxMouseEvent& me) {
    left_mouse = true;
    OnMouse(me);
  }

  void OnMouseMove(const wxMouseEvent& me) {
    OnMouse(me);
  }

  void TrackUtil(Data* data, Line* line, int x, int image_x, float scale_, int width) {
    /*
     if( track_left ) {
      int new_left = std::min(width, std::max(0, static_cast<int>((x - image_x) / scale_)));
      if( new_left != left) {
        left = new_left;
        Refresh();
      }
    }
     */
  }

  void OnMouse(const wxMouseEvent& me) {
    const int x = me.GetX();
    const int y = me.GetY();

    const bool is_tracking = track_col || track_row;

    if( is_tracking ) {
      if( left_mouse ) {
        TrackUtil(&row, &track_row, me.GetX(), image_x, scale_, image.GetWidth());
        TrackUtil(&col, &track_col, me.GetY(), image_y, scale_, image.GetHeight());
      }
      else {
        track_col = Line::Null();
        track_row = Line::Null();
      }
    }
    else {
      Line over_col = col.GetTracking(x);
      Line over_row = row.GetTracking(y);

      const bool is_over = over_col || over_row;

      if( left_mouse && !last_left_mouse && is_over ) {
        track_col = over_col;
        track_row = over_row;
      }

      wxStockCursor cursor = last_cursor_;

      const bool hor = over_col;
      const bool vert = over_row;

      if( hor && vert ) {
        cursor = wxCURSOR_SIZENESW;
      } else if (hor){
        cursor = wxCURSOR_SIZEWE;
      } else if (vert){
        cursor = wxCURSOR_SIZENS;
      } else {
        cursor = wxCURSOR_ARROW;
      }

      if( cursor != last_cursor_) {
        SetCursor(wxCursor(cursor));
        last_cursor_ = cursor;
      }
    }

    last_left_mouse = left_mouse;
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

  static void DrawAnchorDown(wxDC& dc, int x, int y, int size) {
    dc.DrawLine(x, y, x, y+size);
  }

  static void DrawAnchorLeft(wxDC& dc, int x, int y, int size) {
    dc.DrawLine(x, y, x+size, y);
  }

  static void DrawHorizontalCenteredText(wxDC &dc, int left, int right, int y,
                                         const wxString &s) {
    const wxSize size = dc.GetTextExtent(s);
    int x = left + (right-left)/2 - size.GetWidth()/2;
    dc.DrawText(s, x, y - size.GetHeight());
  }

  static void DrawVerticalCenteredText(wxDC &dc, int top, int bottom, int x,
                                         const wxString &s) {
    const wxSize size = dc.GetTextExtent(s);
    int y = top + (bottom-top)/2 - size.GetHeight()/2;
    dc.DrawText(s, x - size.GetWidth() , y);
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

    image_x = (window_width - w)/2;
    image_y = (window_height - h)/2;

    DrawImage(dc, image_x, image_y, w, h);

    const bool draw_ruler = true;
    const bool draw_guides = true;
    const bool draw_sizer = true;

    if( draw_guides ) {
      wxPen guide_pen(wxColour(0, 255, 0));
      guide_pen.SetStyle(wxPENSTYLE_SHORT_DASH);
      dc.SetPen(guide_pen);

      const auto vert = col.GetLines();
      const auto hor = row.GetLines();

      for (auto v : vert) {
        VerticalLine(dc, static_cast<int>(image_x + v * scale_));
      }

      for (auto h : hor) {
        HorizontalLine(dc, static_cast<int>(image_y + h * scale_));
      }
    }

    if( draw_sizer ) {
      const int distance = 20;
      const int anchor_size = 6;

      wxPen sizer_pen(wxColour(0, 0, 0));
      wxFont sizer_font(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
      dc.SetPen(sizer_pen);
      dc.SetFont(sizer_font);

      const int image_end_x = image_x + static_cast<int>(image.GetWidth()*scale_);
      const int anchor_y = image_y - distance;
      const int text_y = anchor_y - 3;

      const auto col_text = col.GetText();
      const auto row_text = row.GetText();

      DrawAnchorDown(dc, image_x, anchor_y, anchor_size);
      int end = image_end_x;
      for(const auto t : col_text) {
        // DrawAnchorDown(dc, image_x + t.position * scale_, anchor_y, anchor_size);
        end = image_x + t.right * scale_;
      }
      dc.DrawLine(image_x, anchor_y, end, anchor_y);
      DrawAnchorDown(dc, end, anchor_y, anchor_size);

      for(const auto t: col_text) {
        DrawHorizontalCenteredText(dc, image_x + t.left * scale_, image_x + t.right * scale_, text_y, wxString::Format("%s", t.text.c_str()));
      }

      const int image_end_y = image_y + static_cast<int>(image.GetHeight()*scale_);
      const int anchor_x = image_x - distance;
      const int text_x = anchor_x - 3;


      DrawAnchorLeft(dc, anchor_x, image_y, anchor_size);
      end = image_end_y;
      for(const auto t : row_text) {
        // DrawAnchorLeft(dc, anchor_x, image_y + t.position * scale_, anchor_size);
        end = image_y + t.right * scale_;
      }
      dc.DrawLine(anchor_x, image_y, anchor_x, end);
      DrawAnchorLeft(dc, anchor_x, end, anchor_size);

      for(const auto t: row_text) {
        DrawVerticalCenteredText(dc, image_y + t.left * scale_, image_y + t.right * scale_, text_x, wxString::Format("%s", t.text.c_str()));
      }
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
};

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
  scalingsprite::ScalingSprite data_;

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
    drawPane->GetRect(&data_);
    SaveProtoText(data_, filename_ + ".txt");
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

      LoadProtoText(&data_, filename_ + ".txt");
      drawPane->SetRect(data_);
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
