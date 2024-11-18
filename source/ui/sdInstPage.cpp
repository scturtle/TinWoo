#include "ui/sdInstPage.hpp"
#include "sdInstall.hpp"
#include "ui/MainApplication.hpp"
#include "ui/mainPage.hpp"
#include "util/config.hpp"
#include "util/lang.hpp"
#include "util/util.hpp"
#include <filesystem>

#define COLOR(hex) pu::ui::Color::FromHex(hex)

namespace inst::ui {
extern MainApplication *mainApp;

sdInstPage::sdInstPage() : Layout::Layout() {

  this->SetBackgroundColor(COLOR("#000000FF"));
  this->infoRect = Rectangle::New(0, 95, 1920, 60, COLOR("#00000080"));
  this->topRect = Rectangle::New(0, 0, 1920, 94, COLOR("#000000FF"));
  this->botRect = Rectangle::New(0, 1019, 1920, 61, COLOR("#000000FF"));

  this->pageInfoText = TextBlock::New(10, 109, "inst.sd.top_info"_lang);
  this->pageInfoText->SetFont(pu::ui::MakeDefaultFontName(30));
  this->pageInfoText->SetColor(COLOR("#FFFFFFFF"));

  this->butText = TextBlock::New(10, 1028, "inst.sd.buttons"_lang);
  this->butText->SetColor(COLOR("#FFFFFFFF"));

  this->menu = pu::ui::elm::Menu::New(0, 156, 1920, COLOR("#FFFFFF00"), COLOR("#4f4f4d33"), 84, 10);
  this->menu->SetItemsFocusColor(COLOR("#4f4f4dAA"));
  this->menu->SetScrollbarColor(COLOR("#1A1919FF"));
  this->menu->SetItemAlphaIncrementSteps(1);

  this->Add(this->topRect);
  this->Add(this->infoRect);
  this->Add(this->botRect);
  this->Add(this->butText);
  this->Add(this->pageInfoText);
  this->Add(this->menu);
}

void sdInstPage::drawMenuItems(std::filesystem::path ourPath) {
  int myindex = this->menu->GetSelectedIndex(); // store index so when page redraws we can get the last item we checked.

  this->currentDir = ourPath;
  auto pathStr = this->currentDir.string();
  if (pathStr.length()) {
    if (pathStr[pathStr.length() - 1] == ':') {
      this->currentDir = this->currentDir / "";
    }
  }

  try {
    this->ourDirectories = util::getDirsAtPath(this->currentDir);
    this->ourFiles = util::getDirectoryFiles(this->currentDir, {".nsp", ".nsz", ".xci", ".xcz"});
  } catch (std::exception &e) {
    this->drawMenuItems(this->currentDir.parent_path());
    return;
  }

  this->menu->ClearItems();
  std::string itm = "..";
  auto ourEntry = pu::ui::elm::MenuItem::New(itm);
  ourEntry->SetColor(COLOR("#FFFFFFFF"));
  ourEntry->SetIcon(inst::util::LoadTexture(inst::icon::upper));
  this->menu->AddItem(ourEntry);

  for (auto &file : this->ourDirectories) {
    std::string itm = file.filename().string();
    auto ourEntry = pu::ui::elm::MenuItem::New(itm);
    ourEntry->SetColor(COLOR("#FFFFFFFF"));
    ourEntry->SetIcon(inst::util::LoadTexture(inst::icon::folder));
    this->menu->AddItem(ourEntry);
  }

  for (auto &file : this->ourFiles) {
    std::string itm = file.filename().string();
    auto ourEntry = pu::ui::elm::MenuItem::New(itm);
    ourEntry->SetColor(COLOR("#FFFFFFFF"));
    this->menu->AddItem(ourEntry);
  }

  this->menu->SetSelectedIndex(myindex); // jump to the index we saved from above
}

void sdInstPage::select() {
  int selectedIndex = this->menu->GetSelectedIndex();
  int dirListSize = this->ourDirectories.size();
  if (this->menu->GetItems()[selectedIndex]->GetName() == "..") {
    this->drawMenuItems(this->currentDir.parent_path());
    this->menu->SetSelectedIndex(0);
  } else if (selectedIndex - 1 < dirListSize) {
    this->drawMenuItems(this->ourDirectories[selectedIndex - 1]);
    this->menu->SetSelectedIndex(0);
  } else {
    auto file = this->ourFiles[selectedIndex - 1 - dirListSize];
    int dialogResult = -1;
    dialogResult = mainApp->CreateShowDialog(
        "inst.target.desc0"_lang + ":\n\n" +
            inst::util::shortenString(std::filesystem::path(file).filename().string(), 32, true) + "\n\n" +
            "inst.target.desc1"_lang,
        "\n\n\n\n\n\n\n" + "common.cancel_desc"_lang, {"inst.target.opt0"_lang, "inst.target.opt1"_lang}, false,
        inst::util::LoadTexture(inst::icon::install));
    if (dialogResult == -1)
      return;
    nspInstStuff::installNspFromFile({file}, dialogResult);
  }
}

void sdInstPage::onInput(u64 Down, u64 Up, u64 Held, pu::ui::TouchPoint touch_pos) {

  if (Down & HidNpadButton_B) {
    mainApp->LoadLayout(mainApp->mainPage);
  }

  HidTouchScreenState state = {0};
  static s32 yyy = 0;
  if (hidGetTouchScreenStates(&state, 1)) {
    if ((Down & HidNpadButton_A) || (state.count != yyy)) {
      yyy = state.count;
      if (yyy != 1) {
        this->select();
      }
    }
  }

  if (Down & HidNpadButton_Plus) {
    this->select();
  }

  // page up
  if (Down & HidNpadButton_ZL)
    this->menu->SetSelectedIndex(std::max(0, this->menu->GetSelectedIndex() - 10));

  // page down
  if (Down & HidNpadButton_ZR)
    this->menu->SetSelectedIndex(std::min((s32)this->menu->GetItems().size() - 1, this->menu->GetSelectedIndex() + 10));

  // goto top of list
  if (Down & HidNpadButton_L) {
    int x = this->menu->GetItems().size() - 1;
    this->menu->SetSelectedIndex(std::max(0, this->menu->GetSelectedIndex() - x));
  }

  // goto bottom of list
  if (Down & HidNpadButton_R) {
    int x = this->menu->GetItems().size() - 1;
    this->menu->SetSelectedIndex(std::min((s32)this->menu->GetItems().size() - 1, this->menu->GetSelectedIndex() + x));
  }
}
} // namespace inst::ui
