#include "ui/hdInstPage.hpp"
#include "install/install_file.hpp"
#include "ui/MainApplication.hpp"
#include "ui/mainPage.hpp"
#include "util/config.hpp"
#include "util/lang.hpp"
#include "util/util.hpp"
#include <filesystem>

namespace inst::ui {
extern MainApplication *mainApp;

HDInstPage::HDInstPage() : Layout::Layout() {

  this->SetBackgroundColor(COLOR("#000000FF"));

  this->topText = TextBlock::New(10, 14, "inst.hd.top_info"_lang);
  this->topText->SetColor(COLOR("#FFFFFFFF"));
  this->butText = TextBlock::New(10, 1028, "inst.hd.buttons"_lang);
  this->butText->SetColor(COLOR("#FFFFFFFF"));

  this->menu = pu::ui::elm::Menu::New(0, 61, 1920, COLOR("#FFFFFF00"), COLOR("#4f4f4d33"), 84, 11);
  this->menu->SetItemsFocusColor(COLOR("#4f4f4dAA"));
  // this->menu->SetScrollbarColor(COLOR("#1A1919FF"));
  this->menu->SetItemAlphaIncrementSteps(1);
  this->menu->SetShadowBaseAlpha(0);

  this->Add(this->topText);
  this->Add(this->butText);
  this->Add(this->menu);
}

void HDInstPage::drawMenuItems(std::filesystem::path path) {
  this->currentDir = path;
  auto pathStr = this->currentDir.string();
  if (pathStr.ends_with(':')) {
    this->currentDir = this->currentDir / "";
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

  this->menu->SetSelectedIndex(this->lastIdx[this->currentDir]);
}

void HDInstPage::select() {
  int selectedIndex = this->menu->GetSelectedIndex();
  this->lastIdx[this->currentDir] = selectedIndex;
  int dirListSize = this->ourDirectories.size();
  if (this->menu->GetItems()[selectedIndex]->GetName() == "..") {
    this->drawMenuItems(this->currentDir.parent_path());
  } else if (selectedIndex - 1 < dirListSize) {
    this->drawMenuItems(this->ourDirectories[selectedIndex - 1]);
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
    auto storageId = dialogResult == 0 ? NcmStorageId_SdCard : NcmStorageId_BuiltInUser;
    inst::installNspFromFile(file, storageId);
  }
}

void HDInstPage::onInput(u64 Down, u64 Up, u64 Held, pu::ui::TouchPoint touch_pos) {
  if (Down & HidNpadButton_B) {
    if (this->currentDir.string().ends_with(":/")) {
      mainApp->LoadLayout(mainApp->mainPage);
    } else {
      this->lastIdx[this->currentDir] = this->menu->GetSelectedIndex();
      this->drawMenuItems(this->currentDir.parent_path());
    }
  }

  HidTouchScreenState state = {0};
  static s32 zzz = 0;
  if (hidGetTouchScreenStates(&state, 1)) {
    if ((Down & HidNpadButton_A) || (state.count != zzz)) {
      zzz = state.count;
      if (zzz != 1) {
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
