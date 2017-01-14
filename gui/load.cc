#include "gui/load.h"

#include <iostream>

#include "core/proto.h"

#include "gui/layoutcontainer.h"
#include "gui/layout.h"
#include "gui/button.h"
#include "gui/panelwidget.h"

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

void BuildLayoutContainer(UiState* state, Font* font, LayoutContainer* root, const gui::LayoutContainer& c, TextureCache* cache, TextBackgroundRenderer* br);

void SetupLayout(LayoutData* data, const gui::Widget& src) {
  data->SetColumn(src.column());
  data->SetRow(src.row());
  data->SetPreferredWidth(src.preferred_width());
  data->SetPreferredHeight(src.preferred_height());
}

std::shared_ptr<Widget> CreateWidget(UiState* state, Font* font, const gui::Widget& w, TextureCache* cache, TextBackgroundRenderer* br) {
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
  }
  else {
    PanelWidget* l = new PanelWidget(state);
    ret.reset(l);
    BuildLayoutContainer(state, font, &l->container, w.panel().container(), cache, br);
  }

  assert(ret.get());

  SetupLayout(&ret->Layout(), w);

  return ret;
}

void BuildLayoutContainer(UiState* state, Font* font, LayoutContainer* root, const gui::LayoutContainer& c, TextureCache* cache, TextBackgroundRenderer* br) {
  root->SetLayout( GetLayout(c.layout()) );
  for(const gui::Widget& widget: c.widgets()) {
    root->Add(CreateWidget(state, font, widget, cache, br));
  }
}

bool Load(UiState* state, Font* font, LayoutContainer* root, const std::string& path, TextureCache* cache, TextBackgroundRenderer* br) {
  gui::LayoutContainer c;
  const std::string load_result = LoadProtoJson(&c, path);
  if( false == load_result.empty() ) {
    std::cerr << "Failed to load gui from " << path << ": " << load_result << "\n";
    return false;
  }

  BuildLayoutContainer(state, font, root, c, cache, br);

  return root->HasWidgets();
}
