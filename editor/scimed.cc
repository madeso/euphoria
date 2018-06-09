#include "editor/scimed.h"

#include "core/range.h"
#include "core/canvaslogic.h"
#include "core/str.h"
#include "core/numeric.h"
#include "core/optionalindex.h"

#include "render/texture.h"
#include "render/texturecache.h"

#include "window/imgui_ext.h"
#include "window/imgui_icons.h"

#include "editor/canvas.h"

#include <iomanip>

bool
IsCloseTo(float a, float b, float c = 3)
{
  return Abs(a - b) < c;
}

bool
KindaTheSame(int a, int b)
{
  return std::abs(a - b) < 5;
}

constexpr int RulerSize = 20;

using TrackingLine           = OptionalIndex<int>;
using PositionClassification = OptionalIndex<int>;

/// Extra calculated data from the corresponding data row (either row or column)
/// in the ScalingSprite struct. Like the data, this can be viewed as the space
/// between the dividing lines.
class SpaceData
{
 public:
  SpaceData(std::string t, int aleft, int aright, int i)
      : text(t)
      , left(aleft)
      , right(aright)
      , index(i)
  {
  }
  std::string text;

  int left;
  int right;
  int index;
};


/// The visual representation of a line, dividing or splitting a area
/// The actual data is either the row or col array of the ScalingSprite struct
class SplitData
{
 public:
  SplitData(int p, int mi, int ma)
      : position(p)
      , min_value(mi)
      , max_value(ma)
  {
  }
  // defines the position of the split
  int position;

  // the split area
  int min_value;
  int max_value;
};

std::string
PixelsOrPercentageString(int data_value, int total_percentage)
{
  if(data_value > 0)
  {
    return Str{} << data_value << "px";
  }
  else
  {
    const auto p = (-data_value * 100.0f) / total_percentage;
    return Str{} << std::setprecision(3) << p << "%";
  }
}

int
GetTotalPercentage(const std::vector<int>& data)
{
  int total = 0;
  for(int i : data)
  {
    if(i < 0)
    {
      total += -i;
    }
  }
  return total;
}

class Data
{
 public:
  PositionClassification
  FindSpaceIndexOrNull(int y, int image_size) const
  {
    const bool within_image = y > 0 && y < image_size;

    if(within_image)
    {
      return PositionClassification::FromIndex(FindSpaceIndex(y));
    }
    else
    {
      return PositionClassification::Null();
    }
  }

  int
  FindSpaceIndex(int x) const
  {
    const auto data  = CalculateAllSpaces();
    int        last  = 0;
    int        index = 0;
    for(const auto& d : data)
    {
      last = index;
      if(x < d.right)
      {
        return last;
      }
      index += 1;
    }
    return last;
  }

  std::vector<SpaceData>
  CalculateAllSpaces() const
  {
    std::vector<SpaceData> ret;
    const int              total_percentage = GetTotalPercentage(*data);
    int                    x                = 0;
    for(unsigned int index = 0; index < data->size(); ++index)
    {
      const int value = (*data)[index];
      const int step  = abs(value);
      ret.emplace_back(
          PixelsOrPercentageString(value, total_percentage),
          x,
          x + step,
          index);
      x += step;
    }
    return ret;
  }

  std::vector<SplitData>
  CalculateAllSplits() const
  {
    std::vector<SplitData> ret;
    bool                   has_data = false;
    int                    x        = 0;
    int                    last_x   = 0;
    for(const auto i : *data)
    {
      int dx = std::abs(i);
      if(has_data)
      {
        ret.emplace_back(SplitData(x, last_x, x + dx));
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

  explicit Data(std::vector<int>* d)
      : data(d)
  {
  }
  std::vector<int>* data;
};

struct Dc
{
  const Canvas& canvas;

  explicit Dc(const Canvas& c)
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
    const auto  from      = canvas.WorldToScreen(ImVec2{x, y});
    const auto  to        = canvas.WorldToScreen(ImVec2{tx, ty});
    draw_list->AddLine(from, to, color);
  }

  bool
  ButtonAt(const ImVec2& p, const char* label) const
  {
    const auto backup = ImGui::GetCursorPos();
    ImVec2     pp     = p;
    pp.x -= canvas.position.x;
    pp.y -= canvas.position.y;
    ImGui::SetCursorPos(pp);
    const bool clicked = ImGui::Button(label);
    ImGui::SetCursorPos(backup);
    return clicked;
  }

  bool
  DrawHorizontalCenteredText(
      int left_p, int right_p, int y_p, const std::string& s) const
  {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const auto  size      = ImGui::CalcTextSize(s.c_str());
    const auto  left      = canvas.WorldToScreen(ImVec2{left_p, y_p});
    const auto  right     = canvas.WorldToScreen(ImVec2{right_p, y_p});
    const auto  y         = left.y;
    auto        x         = left.x + (right.x - left.x) / 2 - size.x / 2;
    const auto  p         = ImVec2{x, y - size.y};
    return ButtonAt(p, s.c_str());
  }

  bool
  DrawVerticalCenteredText(
      int top_p, int bottom_p, int x_p, const std::string& s) const
  {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const auto  size      = ImGui::CalcTextSize(s.c_str());
    const auto  top       = canvas.WorldToScreen(ImVec2{x_p, top_p});
    const auto  bottom    = canvas.WorldToScreen(ImVec2{x_p, bottom_p});
    const auto  x         = bottom.x;
    const auto  y         = top.y + (bottom.y - top.y) / 2 - size.y / 2;
    const auto  p         = ImVec2{x - size.x, y};
    return ButtonAt(p, s.c_str());
  }
};

void
DrawSizerCol(
    std::shared_ptr<Texture2d>    image,
    const Scimed&                 sc,
    scalingsprite::ScalingSprite* sprite)
{
  Dc dc{sc.canvas};

  std::vector<int>* data   = &sprite->cols;
  const int         end    = image->GetWidth();

  const auto        spaces = Data{data}.CalculateAllSpaces();
  dc.DrawAnchorDown(0, -sc.sizer_distance, sc.anchor_size);
  dc.DrawLine(0, -sc.sizer_distance, end, -sc.sizer_distance);
  dc.DrawAnchorDown(end, -sc.sizer_distance, sc.anchor_size);

  int index = 0;
  for(const auto& t : spaces)
  {
    const bool clicked = dc.DrawHorizontalCenteredText(
        t.left, t.right, -sc.sizer_text_distance, t.text);
    if(clicked)
    {
      (*data)[index] = -(*data)[index];
    }
    index += 1;
  }
}

void
DrawSizerRow(
    std::shared_ptr<Texture2d>    image,
    const Scimed&                 sc,
    scalingsprite::ScalingSprite* sprite)
{
  Dc dc{sc.canvas};

  std::vector<int>* data = &sprite->rows;

  const int end = image->GetHeight();

  const auto spaces = Data{data}.CalculateAllSpaces();
  dc.DrawAnchorLeft(-sc.sizer_distance, 0, sc.anchor_size);
  dc.DrawLine(-sc.sizer_distance, 0, -sc.sizer_distance, end);
  dc.DrawAnchorLeft(-sc.sizer_distance, end, sc.anchor_size);

  int index = 0;
  for(const auto& t : spaces)
  {
    const bool clicked = dc.DrawVerticalCenteredText(
        t.left, t.right, -sc.sizer_text_distance, t.text);
    if(clicked)
    {
      (*data)[index] = -(*data)[index];
    }
    index += 1;
  }
}

void
DrawSizer(
    std::shared_ptr<Texture2d>    image,
    const Scimed&                 sc,
    scalingsprite::ScalingSprite* sprite)
{
  DrawSizerRow(image, sc, sprite);
  DrawSizerCol(image, sc, sprite);
}


int
DrawSingleGuideLineGroup(
    bool                          isvert,
    float                         mouse,
    scalingsprite::ScalingSprite* sprite,
    Canvas*                       canvas)
{
  const auto guide_color = IM_COL32(0, 255, 0, 255);
  const auto vert =
      Data{isvert ? &sprite->cols : &sprite->rows}.CalculateAllSplits();

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
  const auto lines            = Data{data_ptr}.CalculateAllSplits();
  const auto d                = lines[line_index];
  const auto scaled           = static_cast<int>(world);
  const auto max              = std::max(d.min_value + 1, scaled);
  const int  line_x           = std::min(d.max_value - 1, max);
  const int  left_size        = line_x - d.min_value;
  const int  right_size       = d.max_value - line_x;
  (*data_ptr)[line_index]     = Sign((*data_ptr)[line_index]) * left_size;
  (*data_ptr)[line_index + 1] = Sign((*data_ptr)[line_index + 1]) * right_size;
}

void
DoSplitData(
    std::vector<int>* data_ptr, const PositionClassification& class_x, int x)
{
  const int scale   = 1;
  const int image_x = 0;

  Data       data(data_ptr);
  const auto text    = data.CalculateAllSpaces();
  const auto size    = text[class_x.GetIndex()];
  const int  mouse_x = std::min(
      size.right - 1,
      std::max(size.left + 1, static_cast<int>((x - image_x) / scale)));
  const int split_x                = mouse_x - size.left;
  int       value                  = (*data.data)[class_x.GetIndex()];
  const int sign                   = Sign(value);
  (*data.data)[class_x.GetIndex()] = split_x * sign;  // value / 2;
  const int new_value =
      sign * (sign * value - sign * (*data.data)[class_x.GetIndex()]);
  (*data.data).insert((*data.data).begin() + class_x.GetIndex() + 1, new_value);
}

bool
PopupButton(bool enabled, const char* label)
{
  if(enabled)
  {
    if(ImGui::Selectable(label))
    {
      return true;
    }
  }
  else
  {
    ImguiDisabled disabled;
    ImGui::TextUnformatted(label);
  }

  return false;
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
  DrawSizer(texture, *this, &scaling);

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
    // ImguiLabel(Str() << "Mouse: " << mouse_popup);

    const auto class_y = Data{&scaling.rows}.FindSpaceIndexOrNull(
        mouse_popup.y, texture->GetHeight());
    const auto class_x = Data{&scaling.cols}.FindSpaceIndexOrNull(
        mouse_popup.x, texture->GetWidth());

    if(PopupButton(class_y, ICON_FK_ARROWS_H " New Horizontal divider"))
    {
      DoSplitData(&scaling.rows, class_y, mouse_popup.y);
    }

    if(PopupButton(class_x, ICON_FK_ARROWS_V " New Vertical divider"))
    {
      DoSplitData(&scaling.cols, class_x, mouse_popup.x);
    }
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

void
Scimed::LoadFile(TextureCache* cache, FileSystem* fs, const std::string& path)
{
  texture = cache->GetTexture(path);
  scaling = scalingsprite::ScalingSprite{};

  if(texture)
  {
    if(scaling.rows.empty())
    {
      scaling.rows.emplace_back(texture->GetHeight());
    }

    if(scaling.cols.empty())
    {
      scaling.cols.emplace_back(texture->GetWidth());
    }
  }
}
