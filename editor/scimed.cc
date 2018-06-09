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


template <
    typename TAnchorFunction,
    typename TLineFunction,
    typename TButtonFunction>
void
DrawSizerCommon(
    std::shared_ptr<Texture2d> image,
    const Scimed&              sc,
    std::vector<int>*          data,
    int                        end,
    TAnchorFunction            anchor_function,
    TLineFunction              line_function,
    TButtonFunction            button_function)
{
  const auto spaces = Data{data}.CalculateAllSpaces();
  anchor_function(0, -sc.sizer_distance, sc.anchor_size);
  line_function(end, -sc.sizer_distance);
  anchor_function(end, -sc.sizer_distance, sc.anchor_size);

  int index = 0;
  for(const auto& t : spaces)
  {
    const bool clicked =
        button_function(t.left, t.right, -sc.sizer_text_distance, t.text);
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
  DrawSizerCommon(
      image,
      sc,
      &sprite->cols,
      image->GetWidth(),
      [&sc](int position, int distance, int size) {
        Dc{sc.canvas}.DrawAnchorDown(position, distance, size);
      },
      [&sc](int end, int distance) {
        Dc{sc.canvas}.DrawLine(0, distance, end, distance);
      },
      [&sc](
          int left, int right, int distance, const std::string& text) -> bool {
        return Dc{sc.canvas}.DrawHorizontalCenteredText(
            left, right, distance, text);
      });

  DrawSizerCommon(
      image,
      sc,
      &sprite->rows,
      image->GetHeight(),
      [&sc](int position, int distance, int size) {
        Dc{sc.canvas}.DrawAnchorLeft(distance, position, size);
      },
      [&sc](int end, int distance) {
        Dc{sc.canvas}.DrawLine(distance, 0, distance, end);
      },
      [&sc](
          int left, int right, int distance, const std::string& text) -> bool {
        return Dc{sc.canvas}.DrawVerticalCenteredText(
            left, right, distance, text);
      });
}


template <typename TLineFunction, typename TCoordFunction>
int
DrawSingleAxisSplits(
    std::vector<int>* data,
    const ImVec2&     mouse,
    Canvas*           canvas,
    TLineFunction     line_function,
    TCoordFunction    coord_function)
{
  const auto splits = Data{data}.CalculateAllSplits();

  int ret = -1;

  int i = 0;
  for(auto s : splits)
  {
    line_function(s.position);
    const auto p = canvas->WorldToScreen(ImVec2{s.position, s.position});
    if(IsCloseTo(coord_function(mouse), coord_function(p)))
    {
      ret = i;
    }
    i += 1;
  }

  return ret;
}

LineHoverData
DrawSplits(scalingsprite::ScalingSprite* sprite, Canvas* canvas)
{
  const auto    mouse = ImGui::GetMousePos();
  LineHoverData ret;

  const auto guide_color = IM_COL32(0, 255, 0, 255);

  ret.vertical_index = DrawSingleAxisSplits(
      &sprite->cols,
      mouse,
      canvas,
      [&](int position) { canvas->VerticalLine(position, guide_color); },
      [](const ImVec2& p) -> float { return p.x; });
  ret.horizontal_index = DrawSingleAxisSplits(
      &sprite->rows,
      mouse,
      canvas,
      [&](int position) { canvas->HorizontalLine(position, guide_color); },
      [](const ImVec2& p) -> float { return p.y; });

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
MoveSplit(int split_index, std::vector<int>* data_ptr, float world_position)
{
  if(split_index == -1)
  {
    return;
  }

  const auto split      = Data{data_ptr}.CalculateAllSplits()[split_index];
  const auto position   = static_cast<int>(world_position);
  const auto max        = std::max(split.min_value + 1, position);
  const int  line_x     = std::min(split.max_value - 1, max);
  const int  left_size  = line_x - split.min_value;
  const int  right_size = split.max_value - line_x;

  // move split to new position
  auto& data            = *data_ptr;
  data[split_index]     = Sign(data[split_index]) * left_size;
  data[split_index + 1] = Sign(data[split_index + 1]) * right_size;
}

void
SplitSpaceInTwo(
    std::vector<int>*             data_ptr,
    const PositionClassification& optional_space_index,
    int                           x)
{
  ASSERT(optional_space_index);
  const auto index = optional_space_index.GetIndex();
  auto&      data  = *data_ptr;
  const auto size  = Data{data_ptr}.CalculateAllSpaces()[index];
  const int  mouse = std::min(size.right - 1, std::max(size.left + 1, x));
  ASSERT(mouse > size.left);
  const int left_abs_val = mouse - size.left;
  const int old_value    = data[index];
  const int sign         = Sign(old_value);
  const int left_val     = left_abs_val * sign;
  data[index]            = left_val;
  const int right_val    = sign * (std::abs(old_value) - left_abs_val);
  data.insert(data.begin() + index + 1, right_val);
}

bool
ImguiSelectableOrDisabled(bool enabled, const char* label)
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

  // draw texture
  auto       tex_id = reinterpret_cast<ImTextureID>(texture->GetId());
  const auto pos    = canvas.WorldToScreen(ImVec2{0, 0});
  const auto size =
      canvas.WorldToScreen(ImVec2{texture->GetWidth(), texture->GetHeight()});
  draw_list->AddImage(tex_id, pos, size);

  const auto current_hover = DrawSplits(&scaling, &canvas);
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

      MoveSplit(hover.horizontal_index, &scaling.rows, me.y);
      MoveSplit(hover.vertical_index, &scaling.cols, me.x);
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
    const auto w = canvas.ScreenToWorld(ImGui::GetMousePos());
    mouse_popup  = vec2i{static_cast<int>(w.x), static_cast<int>(w.y)};
  }

  if(ImGui::BeginPopup("asd"))
  {
    // ImguiLabel(Str() << "Mouse: " << mouse_popup);

    const auto space_index_y = Data{&scaling.rows}.FindSpaceIndexOrNull(
        mouse_popup.y, texture->GetHeight());
    const auto space_index_x = Data{&scaling.cols}.FindSpaceIndexOrNull(
        mouse_popup.x, texture->GetWidth());

    if(ImguiSelectableOrDisabled(
           space_index_y, ICON_FK_ARROWS_H " New Horizontal divider"))
    {
      SplitSpaceInTwo(&scaling.rows, space_index_y, mouse_popup.y);
    }

    if(ImguiSelectableOrDisabled(
           space_index_x, ICON_FK_ARROWS_V " New Vertical divider"))
    {
      SplitSpaceInTwo(&scaling.cols, space_index_x, mouse_popup.x);
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
