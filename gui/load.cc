#include "gui/load.h"

#include <iostream>

#include "core/proto.h"

#include "gui/layoutcontainer.h"
#include "gui/layout.h"
#include "gui/button.h"
#include "gui/panelwidget.h"
#include "gui/skin.h"

#include "render/scalablesprite.h"

#include "gui.pb.h"

template<typename T> std::vector<T> ToVector(const google::protobuf::RepeatedField<T>& a) {
  std::vector<T> r;
  for(const T& t: a) {
    r.push_back(t);
  }
  return r;
}

std::shared_ptr<Layout> GetLayout(const gui::Layout& c) {
  if( c.has_table() ) {
    return CreateTableLayout(ToVector(c.table().expanded_rows()), ToVector(c.table().expanded_cols()), c.table().padding());
  }
  else {
    return CreateSingleRowLayout(c.single_row().padding());
  }
}

class CmdButton : public Button {
 public:
  CmdButton(UiState* state) : Button(state) {}
  void OnClicked() override {
    std::cout << "Executing cmd: " << cmd << "\n";
  }
  std::string cmd;
};

void BuildLayoutContainer(UiState* state, Font* font, LayoutContainer* root, const gui::LayoutContainer& c, TextureCache* cache, TextBackgroundRenderer* br, const std::map<std::string, Skin*>& skins);

void SetupLayout(LayoutData* data, const gui::Widget& src) {
  data->SetColumn(src.column());
  data->SetRow(src.row());
  data->SetPreferredWidth(src.preferred_width());
  data->SetPreferredHeight(src.preferred_height());
}

std::shared_ptr<Widget> CreateWidget(UiState* state, Font* font, const gui::Widget& w, TextureCache* cache, TextBackgroundRenderer* br, const std::map<std::string, Skin*>& skins) {
  std::shared_ptr<Widget> ret;

  if( w.has_button() ) {
    CmdButton* b = new CmdButton(state);
    std::shared_ptr<ScalableSprite> sp(new ScalableSprite("gui/metalPanel_blueCorner.png", Sizef::FromSquare(2.0f), cache));
    b->SetSprite(sp);
    ret.reset( b );
    b->cmd = w.button().command();
    b->Text().SetString(w.button().text());
    b->Text().SetFont(font);
    b->Text().SetBackgroundRenderer(br);

    const std::string skin_name = w.button().skin();
    const auto skin_it = skins.find(skin_name);
    if( skin_it != skins.end()) {
      b->SetSkin(skin_it->second);
    }
    else {
      std::cerr<< "Failed to find skin " << skin_name << "\n";
    }
  }
  else {
    PanelWidget* l = new PanelWidget(state);
    ret.reset(l);
    BuildLayoutContainer(state, font, &l->container, w.panel().container(), cache, br, skins);
  }

  assert(ret.get());

  SetupLayout(&ret->Layout(), w);

  return ret;
}

void BuildLayoutContainer(UiState* state, Font* font, LayoutContainer* root, const gui::LayoutContainer& c, TextureCache* cache, TextBackgroundRenderer* br, const std::map<std::string, Skin*>& skins) {
  root->SetLayout( GetLayout(c.layout()) );
  for(const gui::Widget& widget: c.widgets()) {
    root->Add(CreateWidget(state, font, widget, cache, br, skins));
  }
}

Rgb Load(const gui::Rgb& src) {
  return Rgb(src.r(), src.g(), src.b());
}

ButtonState LoadButton(const gui::ButtonState& src) {
  ButtonState ret;
  ret.image = src.image();
  ret.scale = src.scale();
  ret.image_color = Load(src.image_color());
  ret.text_color = Load(src.text_color());
  ret.dx = src.dx();
  ret.dy = src.dy();
  return ret;
}

std::shared_ptr<Skin> LoadSkin(const gui::Skin& src) {
  std::shared_ptr<Skin> skin(new Skin());
  skin->name = src.name();
  skin->button_idle = LoadButton(src.button_idle());
  skin->button_hot = LoadButton(src.button_hot());
  skin->button_active_hot = LoadButton(src.button_active_hot());
  return skin;
}

bool Load(UiState* state, Font* font, LayoutContainer* root, const std::string& path, TextureCache* cache, TextBackgroundRenderer* br, std::vector<std::shared_ptr<Skin>>* skins) {
  gui::File f;
  const std::string load_result = LoadProtoJson(&f, path);
  if( false == load_result.empty() ) {
    std::cerr << "Failed to load gui from " << path << ": " << load_result << "\n";
    return false;
  }

  std::map<std::string, Skin*> skin_map;

  for(const gui::Skin& skin: f.skins()) {
    std::shared_ptr<Skin> skin_ptr = LoadSkin(skin);
    skin_map.insert(std::make_pair(skin.name(), skin_ptr.get()));
    skins->push_back(skin_ptr);
  }

  BuildLayoutContainer(state, font, root, f.root(), cache, br, skin_map);

  return root->HasWidgets();
}
