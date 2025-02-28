#pragma once
#include <pu/Plutonium>

using namespace pu::ui::elm;
namespace inst::ui {
class optionsPage : public pu::ui::Layout {
public:
  optionsPage();
  PU_SMART_CTOR(optionsPage)
  void onInput(u64 Down, u64 Up, u64 Held, pu::ui::TouchPoint touch_pos);
  void setMenuText();

private:
  TextBlock::Ref topText;
  TextBlock::Ref butText;
  pu::ui::elm::Menu::Ref menu;
  // void setMenuText();
  std::string getMenuOptionIcon(bool ourBool);
  std::string getMenuLanguage(int ourLangCode);
};
} // namespace inst::ui
