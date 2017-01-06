#ifndef GUI_LAYOUTDATA_H
#define GUI_LAYOUTDATA_H

class LayoutData {
public:
  LayoutData();
  ~LayoutData();

  int GetColumn() const;
  LayoutData& SetColumn(int c);

  int GetRow() const;
  LayoutData& SetRow(int r);

  int GetPreferredWidth() const;
  LayoutData& SetPreferredWidth(int w);

  int GetPreferredHeight() const;
  LayoutData& SetPreferredHeight(int h);

private:
  int column_;
  int row_;
  int preferred_width_;
  int preferred_height_;
};

#endif  // GUI_LAYOUTDATA_H
