#include "gui/ui.h"

void Ui::Step() {
  state.Begin();
  container.Step();
  state.End();
}
