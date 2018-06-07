#include "editor/scimed.h"

#include "core/range.h"
#include "core/canvaslogic.h"
#include "core/str.h"
#include "core/numeric.h"

#include "render/texture.h"

#include "window/imgui_ext.h"

#include "editor/canvas.h"

bool
IsCloseTo(float a, float b, float c = 3)
{
  return Abs(a - b) < c;
}

constexpr int RulerSize     = 20;
constexpr int SizerDistance = 20;

class TrackingLine
{
 public:
  static TrackingLine
  Null()
  {
    return {};
  }

  static TrackingLine
  FromIndex(int index)
  {
    return {index};
  }

  int
  GetIndex() const
  {
    ASSERT(is_valid);
    return index;
  }

  operator bool() const
  {
    return is_valid;
  }

 private:
  TrackingLine()
      : is_valid(false)
      , index(-1)
  {
  }

  TrackingLine(int index)
      : is_valid(true)
      , index(index)
  {
  }
  bool is_valid;
  int  index;
};

class TextRenderData
{
 public:
  TextRenderData(std::string t, int aleft, int aright, int i)
      : text(t)
      , position(aleft + (aright - aleft) / 2)
      , left(aleft)
      , right(aright)
      , index(i)
  {
  }
  std::string text;
  int         position;
  int         left;
  int         right;
  int         index;
};

bool
KindaTheSame(int a, int b)
{
  return std::abs(a - b) < 5;
}

class LineData
{
 public:
  LineData(int p, int mi, int ma)
      : position(p)
      , min_value(mi)
      , max_value(ma)
  {
  }
  int position;
  int min_value;
  int max_value;
};

enum class PositionType
{
  OnRuler,
  OnText,
  OnImage,
  OnOther
};

class PositionClassification
{
 public:
  PositionClassification()
      : type(PositionType::OnOther)
      , index(-1)
  {
  }
  PositionClassification(PositionType t, int i)
      : type(t)
      , index(i)
  {
  }
  PositionType type;
  int          index;
};

class Data
{
 public:
  PositionClassification
  Classify(int y, int image_start, int image_size, float scale_) const
  {
    const int image_end_y = image_start + static_cast<int>(image_size * scale_);

    const bool within_image = y > image_start && y < image_end_y;
    const bool on_top_side  = y < image_start - SizerDistance && y > RulerSize;

    const int dx = static_cast<int>((y - image_start) / scale_);

    if(on_top_side)
    {
      TextRenderData d = GetTextOver(dx);
      return PositionClassification(PositionType::OnText, d.index);
    }
    else if(within_image)
    {
      TextRenderData d = GetTextOver(dx);
      return PositionClassification(PositionType::OnImage, d.index);
    }
    else if(y < RulerSize)
    {
      TextRenderData d = GetTextOver(dx);
      return PositionClassification(PositionType::OnRuler, d.index);
    }
    else
    {
      return PositionClassification(PositionType::OnOther, -1);
    }
  }

  std::pair<int, int>
  GetExtentsForRange(int index)
  {
    int min = 0;
    for(int i = 0; i <= index; ++i)
    {
      const int value = std::abs((*data)[i]);
      if(i == index)
      {
        return std::pair<int, int>(min, min + value);
      }
      min += value;
    }
    ASSERT(false && "invalid index");
    return std::pair<int, int>(min, min + 2);
  }

  int
  GetTotalPercentage() const
  {
    int total = 0;
    for(int i : *data)
    {
      if(i < 0)
      {
        total += -i;
      }
    }
    return total;
  }

  int
  GetSizeExceptForLine(int index) const
  {
    int r = 0;
    for(unsigned int i = 0; i < data->size(); ++i)
    {
      if(i == index || i + i == index)
      {
      }
      else
      {
        r += std::abs((*data)[i]);
      }
    }
    return r;
  }

  TrackingLine
  GetTracking(int d, float initial, float scale) const
  {
    const auto lines = GetLines();
    for(unsigned int i = 0; i < lines.size(); ++i)
    {
      const LineData& data = lines[i];
      const int       l    = static_cast<int>(initial + scale * data.position);
      if(KindaTheSame(d, l))
      {
        return TrackingLine::FromIndex(i);
      }
    }
    return TrackingLine::Null();
  }

  std::vector<TextRenderData>
  GetText() const
  {
    std::vector<TextRenderData> ret;
    const int                   total_percentage = GetTotalPercentage();
    int                         x                = 0;
    for(unsigned int index = 0; index < data->size(); ++index)
    {
      const int          i = (*data)[index];
      const int          d = abs(i);
      std::ostringstream ss;
      if(i > 0)
      {
        ss << i << "px";
      }
      else
      {
        ss << static_cast<float>(-i * 100.0f / total_percentage) << "%";
      }
      ret.emplace_back(ss.str(), x, x + d, index);
      x += d;
    }
    return ret;
  }

  TextRenderData
  GetTextOver(int x) const
  {
    const auto     data = GetText();
    TextRenderData last("", -1, -1, 0);
    for(const auto& d : data)
    {
      last = d;
      if(x < d.right)
      {
        return d;
      }
    }
    // ASSERT(false && "shouldnt happen"); // but will if our data is less than
    // the image size
    return last;
  }

  // lines lie between datapoints
  std::vector<LineData>
  GetLines() const
  {
    std::vector<LineData> ret;
    bool                  has_data = false;
    int                   x        = 0;
    int                   last_x   = 0;
    for(const auto i : *data)
    {
      int dx = std::abs(i);
      if(has_data)
      {
        ret.emplace_back(LineData(x, last_x, x + dx));
      }
      else
      {
        has_data = true;
      }
      last_x = x;
      x += dx;
    }
    return ret;
  }

  bool
  IsOk() const
  {
    return data->empty() == false;
  }

  explicit Data(std::vector<int>* d)
      : data(d)
  {
  }
  std::vector<int>* data;
};

struct Dc
{
  Canvas* canvas;

  explicit Dc(Canvas* c)
      : canvas(c)
  {
  }

  void
  DrawAnchorDown(int x, int y, int size) const
  {
    DrawLine(x, y, x, y + size);
  }

  void
  DrawAnchorLeft(int x, int y, int size) const
  {
    DrawLine(x, y, x + size, y);
  }

  void
  DrawLine(int x, int y, int tx, int ty) const
  {
    const auto  color     = IM_COL32(0, 0, 0, 255);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const auto  from      = canvas->WorldToScreen(ImVec2{x, y});
    const auto  to        = canvas->WorldToScreen(ImVec2{tx, ty});
    draw_list->AddLine(from, to, color);
  }

  void
  DrawHorizontalCenteredText(
      int left, int right, int y, const std::string& s) const
  {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    // const wxSize size = dc.GetTextExtent(s);
    // int          x    = left + (right - left) / 2 - size.GetWidth() / 2;
    // dc.DrawText(s, x, y - size.GetHeight());
    draw_list->AddText(
        canvas->WorldToScreen(ImVec2{left, y}),
        IM_COL32(255, 255, 255, 255),
        s.c_str());
  }

  void
  DrawVerticalCenteredText(
      int top, int bottom, int x, const std::string& s) const
  {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    // const wxSize size = dc.GetTextExtent(s);
    // int          y    = top + (bottom - top) / 2 - size.GetHeight() / 2;
    // dc.DrawText(s, x - size.GetWidth(), y);
    draw_list->AddText(
        canvas->WorldToScreen(ImVec2{x, top}),
        IM_COL32(255, 255, 255, 255),
        s.c_str());
  }
};

void
DrawSizer(
    std::shared_ptr<Texture2d>    image,
    const Dc&                     dc,
    scalingsprite::ScalingSprite* sprite)
{
  const int distance    = SizerDistance;
  const int anchor_size = 6;

  Data col{&sprite->cols};
  Data row{&sprite->rows};

  const int image_x = 0;
  const int image_y = 0;
  const int scale   = 1;

  const int image_end_x = image_x + image->GetWidth() * scale;
  const int anchor_y    = image_y - distance;
  const int text_y      = anchor_y - 3;

  const auto col_text = col.GetText();
  const auto row_text = row.GetText();

  dc.DrawAnchorDown(image_x, anchor_y, anchor_size);
  int end = image_end_x;
  for(const auto& t : col_text)
  {
    // DrawAnchorDown(dc, image_x + t.position * scale, anchor_y,
    // anchor_size);
    end = image_x + t.right * scale;
  }
  dc.DrawLine(image_x, anchor_y, end, anchor_y);
  dc.DrawAnchorDown(end, anchor_y, anchor_size);

  for(const auto& t : col_text)
  {
    dc.DrawHorizontalCenteredText(
        image_x + t.left * scale, image_x + t.right * scale, text_y, t.text);
  }

  const int image_end_y = image_y + image->GetHeight() * scale;
  const int anchor_x    = image_x - distance;
  const int text_x      = anchor_x - 3;


  dc.DrawAnchorLeft(anchor_x, image_y, anchor_size);
  end = image_end_y;
  for(const auto t : row_text)
  {
    // DrawAnchorLeft(dc, anchor_x, image_y + t.position * scale,
    // anchor_size);
    end = image_y + t.right * scale;
  }
  dc.DrawLine(anchor_x, image_y, anchor_x, end);
  dc.DrawAnchorLeft(anchor_x, end, anchor_size);

  for(const auto t : row_text)
  {
    dc.DrawVerticalCenteredText(
        image_y + t.left * scale, image_y + t.right * scale, text_x, t.text);
  }
}


int
DrawSingleGuideLineGroup(
    bool                          isvert,
    float                         mouse,
    scalingsprite::ScalingSprite* sprite,
    Canvas*                       canvas)
{
  const auto guide_color = IM_COL32(0, 255, 0, 255);
  const auto vert = Data{isvert ? &sprite->cols : &sprite->rows}.GetLines();

  int ret = -1;

  int i = 0;
  for(auto v : vert)
  {
    if(isvert)
      canvas->VerticalLine(v.position, guide_color);
    else
      canvas->HorizontalLine(v.position, guide_color);
    const auto p = canvas->WorldToScreen(ImVec2{v.position, v.position});
    if(IsCloseTo(mouse, isvert ? p.x : p.y))
    {
      ret = i;
    }
    i += 1;
  }

  return ret;
}

LineHoverData
DrawGuides(scalingsprite::ScalingSprite* sprite, Canvas* canvas)
{
  const auto    mouse = ImGui::GetMousePos();
  LineHoverData ret;

  ret.vertical_index = DrawSingleGuideLineGroup(true, mouse.x, sprite, canvas);
  ret.horizontal_index =
      DrawSingleGuideLineGroup(false, mouse.y, sprite, canvas);

  return ret;
}

void
SetMouseCursorFromHover(const LineHoverData& hover)
{
  if(hover.horizontal_index != -1 || hover.vertical_index != -1)
  {
    if(hover.horizontal_index != -1 && hover.vertical_index != -1)
    {
      ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
    }
    else if(hover.horizontal_index != -1)
    {
      ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
    }
    else
    {
      ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    }
  }
}


void
TrackUtil(
    int line_index, std::vector<int>* data_ptr, float world, int total_size)
{
  if(line_index == -1)
    return;
  const auto lines            = Data{data_ptr}.GetLines();
  const auto d                = lines[line_index];
  const auto scaled           = static_cast<int>(world);
  const auto max              = std::max(d.min_value + 1, scaled);
  const int  line_x           = std::min(d.max_value - 1, max);
  const int  at_line          = Data{data_ptr}.GetSizeExceptForLine(line_index);
  const int  remaining_size   = total_size - at_line;
  const int  left_size        = line_x - d.min_value;
  const int  right_size       = d.max_value - line_x;
  (*data_ptr)[line_index]     = Sign((*data_ptr)[line_index]) * left_size;
  (*data_ptr)[line_index + 1] = Sign((*data_ptr)[line_index + 1]) * right_size;
}

bool
Scimed::Run()
{
  canvas.Begin();
  canvas.ShowGrid();
  ImDrawList* draw_list = ImGui::GetWindowDrawList();

  const auto mouse = ImGui::GetMousePos();

  if(!texture)
  {
    canvas.ShowRuler();
    canvas.End();
    return false;
  }

  auto tex_id = reinterpret_cast<ImTextureID>(texture->GetId());

  const auto pos = canvas.WorldToScreen(ImVec2{0, 0});
  const auto size =
      canvas.WorldToScreen(ImVec2{texture->GetWidth(), texture->GetHeight()});

  draw_list->AddImage(tex_id, pos, size);

  const auto current_hover = DrawGuides(&scaling, &canvas);
  DrawSizer(texture, Dc{&canvas}, &scaling);

  canvas.ShowRuler();
  canvas.End();

  if(ImGui::IsMouseClicked(0))
  {
    hover = current_hover;
  }

  const bool is_tracking =
      hover.horizontal_index != -1 || hover.vertical_index != -1;
  if(is_tracking)
  {
    if(ImGui::IsMouseDown(0))
    {
      SetMouseCursorFromHover(hover);

      const auto me = canvas.ScreenToWorld(ImGui::GetMousePos());

      TrackUtil(
          hover.horizontal_index, &scaling.rows, me.y, texture->GetHeight());
      TrackUtil(hover.vertical_index, &scaling.cols, me.x, texture->GetWidth());
    }
    else
    {
      SetMouseCursorFromHover(current_hover);
      hover = LineHoverData{};
    }
  }
  else
  {
    SetMouseCursorFromHover(current_hover);
  }

  if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
  {
    ImGui::OpenPopup("asd");
    mouse_popup = C(canvas.ScreenToWorld(ImGui::GetMousePos()));
  }

  if(ImGui::BeginPopup("asd"))
  {
    ImguiLabel(Str() << "Mouse: " << mouse_popup);
    if(ImGui::Selectable("Ha ha"))
    {
    }
    ImGui::Selectable("dog");
    ImGui::EndPopup();
  }


  // debug
  if(BeginFixedOverlay(ImguiCorner::TopRight, ""))
  {
    ImguiLabel(Str() << "Mouse: " << C(canvas.GetMouse()));
    ImguiLabel(
        Str() << "Hover: " << hover.horizontal_index << "/"
              << hover.vertical_index);
    ImguiLabel(
        Str() << "Current: " << current_hover.horizontal_index << "/"
              << current_hover.vertical_index);
    ImguiLabel(is_tracking ? "Tracking" : "Not tracking");
    ImGui::End();
  }

  return false;
}
