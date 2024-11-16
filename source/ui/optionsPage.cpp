#include "ui/optionsPage.hpp"
#include "ui/MainApplication.hpp"
#include "ui/instPage.hpp"
#include "ui/mainPage.hpp"
#include "util/config.hpp"
#include "util/curl.hpp"
#include "util/lang.hpp"
#include "util/unzip.hpp"
#include "util/util.hpp"
#include <dirent.h>
#include <experimental/filesystem>
#include <filesystem>
#include <switch.h>
#include <sys/stat.h>

#define COLOR(hex) pu::ui::Color::FromHex(hex)

namespace inst::ui {
extern MainApplication *mainApp;
s32 prev_touchcount = 0;
std::string flag = "romfs:/images/flags/en.png";
std::vector<std::string> languageStrings = {"Sys", "En", "Jpn", "Fr", "De", "It", "Ru", "Es", "Tw", "Cn"};

optionsPage::optionsPage() : Layout::Layout() {
  this->infoRect = Rectangle::New(0, 95, 1280, 60, COLOR("#00000080"));
  this->SetBackgroundColor(COLOR("#000000FF"));
  this->topRect = Rectangle::New(0, 0, 1280, 94, COLOR("#000000FF"));
  this->botRect = Rectangle::New(0, 659, 1280, 61, COLOR("#000000FF"));
  this->titleImage =
      Image::New(0, 0, pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage("romfs:/images/Settings.png")));
  this->SetBackgroundImage(pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage("romfs:/images/Background.png")));

  this->appVersionText = TextBlock::New(1200, 680, "v" + inst::config::appVersion);
  this->appVersionText->SetColor(COLOR("#FFFFFFFF"));
  this->appVersionText->SetFont(pu::ui::MakeDefaultFontName(20));

  this->pageInfoText = TextBlock::New(10, 109, "options.title"_lang);
  this->pageInfoText->SetColor(COLOR("#FFFFFFFF"));

  this->butText = TextBlock::New(10, 678, "options.buttons"_lang);
  this->butText->SetColor(COLOR("#FFFFFFFF"));

  this->menu = pu::ui::elm::Menu::New(0, 156, 1280, COLOR("#FFFFFF00"), COLOR("#4f4f4d33"), 84, (506 / 84));
  this->menu->SetItemsFocusColor(COLOR("#4f4f4dAA"));
  this->menu->SetScrollbarColor(COLOR("#1A1919FF"));

  this->Add(this->topRect);
  this->Add(this->infoRect);
  this->Add(this->botRect);
  this->Add(this->titleImage);
  this->Add(this->appVersionText);
  this->Add(this->butText);
  this->Add(this->pageInfoText);
  this->setMenuText();
  this->Add(this->menu);
}
void optionsPage::askToUpdate(std::vector<std::string> updateInfo) {

  std::string update = "romfs:/images/icons/update.png";

  if (!mainApp->CreateShowDialog("options.update.title"_lang,
                                 "options.update.desc0"_lang + updateInfo[0] + "options.update.desc1"_lang,
                                 {"options.update.opt0"_lang, "common.cancel"_lang}, false,
                                 pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage(update)))) {
    inst::ui::instPage::loadInstallScreen();
    inst::ui::instPage::setTopInstInfoText("options.update.top_info"_lang + updateInfo[0]);
    inst::ui::instPage::setInstBarPerc(0);
    inst::ui::instPage::setInstInfoText("options.update.bot_info"_lang + updateInfo[0]);
    try {
      std::string downloadName = inst::config::appDir + "/temp_download.zip";
      inst::curl::downloadFile(updateInfo[1], downloadName.c_str(), 0, true);
      romfsExit();
      inst::ui::instPage::setInstInfoText("options.update.bot_info2"_lang + updateInfo[0]);
      inst::zip::extractFile(downloadName, "sdmc:/");
      std::filesystem::remove(downloadName);
      mainApp->CreateShowDialog(
          "options.update.complete"_lang, "options.update.end_desc"_lang, {"common.ok"_lang}, false,
          pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage("romfs:/images/icons/update.png")));
      mainApp->FadeOut();
      mainApp->Close();
    } catch (...) {
      std::string fail = "romfs:/images/icons/fail.png";
      mainApp->CreateShowDialog("options.update.failed"_lang, "options.update.end_desc"_lang, {"common.ok"_lang}, false,
                                pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage(fail)));
      return;
    }
  }
  return;
}

std::string optionsPage::getMenuOptionIcon(bool ourBool) {
  std::string checked = "romfs:/images/icons/checked.png";
  std::string unchecked = "romfs:/images/icons/unchecked.png";
  if (ourBool)
    return checked;
  else
    return unchecked;
}

std::string optionsPage::getMenuLanguage(int ourLangCode) {
  std::string sys = "romfs:/images/flags/sys.png";
  std::string en = "romfs:/images/flags/en.png";
  std::string jpn = "romfs:/images/flags/jpn.png";
  std::string fr = "romfs:/images/flags/fr.png";
  std::string de = "romfs:/images/flags/de.png";
  std::string it = "romfs:/images/flags/it.png";
  std::string ru = "romfs:/images/flags/ru.png";
  std::string es = "romfs:/images/flags/es.png";
  std::string tw = "romfs:/images/flags/tw.png";
  std::string cn = "romfs:/images/flags/cn.png";
  if (ourLangCode >= 0) {
    if (ourLangCode == 0)
      flag = sys;
    else if (ourLangCode == 1)
      flag = en;
    else if (ourLangCode == 2)
      flag = jpn;
    else if (ourLangCode == 3)
      flag = fr;
    else if (ourLangCode == 4)
      flag = de;
    else if (ourLangCode == 5)
      flag = it;
    else if (ourLangCode == 6)
      flag = ru;
    else if (ourLangCode == 7)
      flag = es;
    else if (ourLangCode == 8)
      flag = tw;
    else if (ourLangCode == 9)
      flag = cn;
    return languageStrings[ourLangCode];
  } else {
    flag = en;
    return languageStrings[0];
  }
}

void lang_message() {
  std::string flag = "romfs:/images/icons/flags/sys.png";
  int ourResult = inst::ui::mainApp->CreateShowDialog("sig.restart"_lang, "theme.restart"_lang,
                                                      {"common.no"_lang, "common.yes"_lang}, true,
                                                      pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage(flag)));
  if (ourResult != 0) {
    mainApp->FadeOut();
    mainApp->Close();
  }
}

void optionsPage::setMenuText() {
  this->menu->ClearItems();

  auto ignoreFirmOption = pu::ui::elm::MenuItem::New("options.menu_items.ignore_firm"_lang);
  ignoreFirmOption->SetColor(COLOR("#FFFFFFFF"));
  ignoreFirmOption->SetIcon(
      pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage(this->getMenuOptionIcon(inst::config::ignoreReqVers))));
  this->menu->AddItem(ignoreFirmOption);

  auto validateOption = pu::ui::elm::MenuItem::New("options.menu_items.nca_verify"_lang);
  validateOption->SetColor(COLOR("#FFFFFFFF"));
  validateOption->SetIcon(
      pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage(this->getMenuOptionIcon(inst::config::validateNCAs))));
  this->menu->AddItem(validateOption);

  auto overclockOption = pu::ui::elm::MenuItem::New("options.menu_items.boost_mode"_lang);
  overclockOption->SetColor(COLOR("#FFFFFFFF"));
  overclockOption->SetIcon(
      pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage(this->getMenuOptionIcon(inst::config::overClock))));
  this->menu->AddItem(overclockOption);

  auto deletePromptOption = pu::ui::elm::MenuItem::New("options.menu_items.ask_delete"_lang);
  deletePromptOption->SetColor(COLOR("#FFFFFFFF"));
  deletePromptOption->SetIcon(
      pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage(this->getMenuOptionIcon(inst::config::deletePrompt))));
  this->menu->AddItem(deletePromptOption);

  auto autoUpdateOption = pu::ui::elm::MenuItem::New("options.menu_items.auto_update"_lang);
  autoUpdateOption->SetColor(COLOR("#FFFFFFFF"));
  autoUpdateOption->SetIcon(
      pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage(this->getMenuOptionIcon(inst::config::autoUpdate))));
  this->menu->AddItem(autoUpdateOption);

  auto useSoundOption = pu::ui::elm::MenuItem::New("options.menu_items.useSound"_lang);
  useSoundOption->SetColor(COLOR("#FFFFFFFF"));
  useSoundOption->SetIcon(
      pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage(this->getMenuOptionIcon(inst::config::useSound))));
  this->menu->AddItem(useSoundOption);

  auto useMusicOption = pu::ui::elm::MenuItem::New("options.menu_items.useMusic"_lang);
  useMusicOption->SetColor(COLOR("#FFFFFFFF"));
  useMusicOption->SetIcon(
      pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage(this->getMenuOptionIcon(inst::config::useMusic))));
  this->menu->AddItem(useMusicOption);

  auto fixticket = pu::ui::elm::MenuItem::New("options.menu_items.fixticket"_lang);
  fixticket->SetColor(COLOR("#FFFFFFFF"));
  fixticket->SetIcon(
      pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage(this->getMenuOptionIcon(inst::config::fixticket))));
  this->menu->AddItem(fixticket);

  auto listoveride = pu::ui::elm::MenuItem::New("options.menu_items.listoveride"_lang);
  listoveride->SetColor(COLOR("#FFFFFFFF"));
  listoveride->SetIcon(
      pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage(this->getMenuOptionIcon(inst::config::listoveride))));
  this->menu->AddItem(listoveride);

  auto httpkeyboard = pu::ui::elm::MenuItem::New("options.menu_items.usehttpkeyboard"_lang);
  httpkeyboard->SetColor(COLOR("#FFFFFFFF"));
  httpkeyboard->SetIcon(
      pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage(this->getMenuOptionIcon(inst::config::httpkeyboard))));
  this->menu->AddItem(httpkeyboard);

  auto httpServerUrlOption = pu::ui::elm::MenuItem::New(
      "options.menu_items.http_url"_lang + inst::util::shortenString(inst::config::httpIndexUrl, 42, false));
  httpServerUrlOption->SetColor(COLOR("#FFFFFFFF"));
  std::string neturl = "romfs:/images/icons/url.png";
  httpServerUrlOption->SetIcon(pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage(neturl)));
  this->menu->AddItem(httpServerUrlOption);

  auto languageOption = pu::ui::elm::MenuItem::New("options.menu_items.language"_lang +
                                                   this->getMenuLanguage(inst::config::languageSetting));
  languageOption->SetColor(COLOR("#FFFFFFFF"));
  std::string lang = "romfs:/images/icons/speak.png";
  languageOption->SetIcon(pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage(lang)));
  this->menu->AddItem(languageOption);

  auto updateOption = pu::ui::elm::MenuItem::New("options.menu_items.check_update"_lang);
  updateOption->SetColor(COLOR("#FFFFFFFF"));
  std::string upd = "romfs:/images/icons/update2.png";
  updateOption->SetIcon(pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage(upd)));
  this->menu->AddItem(updateOption);

  auto creditsOption = pu::ui::elm::MenuItem::New("options.menu_items.credits"_lang);
  creditsOption->SetColor(COLOR("#FFFFFFFF"));
  std::string credit = "romfs:/images/icons/credits2.png";
  creditsOption->SetIcon(pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage(credit)));
  this->menu->AddItem(creditsOption);
}

void optionsPage::onInput(u64 Down, u64 Up, u64 Held, pu::ui::TouchPoint touch_pos) {

  if (Down & HidNpadButton_B) {
    mainApp->LoadLayout(mainApp->mainPage);
  }

  if (Down & HidNpadButton_ZL)
    this->menu->SetSelectedIndex(std::max(0, this->menu->GetSelectedIndex() - 6));

  if (Down & HidNpadButton_ZR)
    this->menu->SetSelectedIndex(std::min((s32)this->menu->GetItems().size() - 1, this->menu->GetSelectedIndex() + 6));

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

  HidTouchScreenState state = {0};

  if (hidGetTouchScreenStates(&state, 1)) {

    if ((Down & HidNpadButton_A) || (state.count != prev_touchcount)) {
      prev_touchcount = state.count;

      if (prev_touchcount != 1) {

        std::string keyboardResult;
        int rc;
        std::vector<std::string> downloadUrl;
        std::vector<std::string> languageList;
        int index = this->menu->GetSelectedIndex();
        switch (index) {
        case 0:
          inst::config::ignoreReqVers = !inst::config::ignoreReqVers;
          inst::config::setConfig();
          this->setMenuText();
          // makes sure to jump back to the selected item once the menu is reloaded
          this->menu->SetSelectedIndex(index);
          //
          break;
        case 1:
          if (inst::config::validateNCAs) {
            std::string info = "romfs:/images/icons/information.png";
            if (inst::ui::mainApp->CreateShowDialog("options.nca_warn.title"_lang, "options.nca_warn.desc"_lang,
                                                    {"common.cancel"_lang, "options.nca_warn.opt1"_lang}, false,
                                                    pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage(info))) == 1)
              inst::config::validateNCAs = false;
          } else
            inst::config::validateNCAs = true;
          inst::config::setConfig();
          this->setMenuText();
          this->menu->SetSelectedIndex(index);
          break;
        case 2:
          inst::config::overClock = !inst::config::overClock;
          inst::config::setConfig();
          this->setMenuText();
          this->menu->SetSelectedIndex(index);
          break;
        case 3:
          inst::config::deletePrompt = !inst::config::deletePrompt;
          inst::config::setConfig();
          this->setMenuText();
          this->menu->SetSelectedIndex(index);
          break;
        case 4:
          inst::config::autoUpdate = !inst::config::autoUpdate;
          inst::config::setConfig();
          this->setMenuText();
          this->menu->SetSelectedIndex(index);
          break;
        case 5:
          if (inst::config::useSound) {
            inst::config::useSound = false;
          } else {
            inst::config::useSound = true;
          }
          this->setMenuText();
          this->menu->SetSelectedIndex(index);
          inst::config::setConfig();
          break;
        case 6:
          if (inst::config::useMusic) {
            inst::config::useMusic = false;
          } else {
            inst::config::useMusic = true;
          }
          this->setMenuText();
          this->menu->SetSelectedIndex(index);
          inst::config::setConfig();
          break;
        case 7:
          if (inst::config::fixticket) {
            inst::config::fixticket = false;
          } else {
            inst::config::fixticket = true;
          }
          this->setMenuText();
          this->menu->SetSelectedIndex(index);
          inst::config::setConfig();
          break;
        case 8:
          if (inst::config::listoveride) {
            inst::config::listoveride = false;
          } else {
            inst::config::listoveride = true;
          }
          this->setMenuText();
          this->menu->SetSelectedIndex(index);
          inst::config::setConfig();
          break;
        case 9:
          if (inst::config::httpkeyboard) {
            inst::config::httpkeyboard = false;
          } else {
            inst::config::httpkeyboard = true;
          }
          this->setMenuText();
          this->menu->SetSelectedIndex(index);
          inst::config::setConfig();
          break;
        case 10:
          keyboardResult =
              inst::util::softwareKeyboard("inst.net.url.hint"_lang, inst::config::httpIndexUrl.c_str(), 500);
          if (keyboardResult.size() > 0) {
            inst::config::httpIndexUrl = keyboardResult;
            inst::config::setConfig();
            this->setMenuText();
            this->menu->SetSelectedIndex(index);
          }
          break;
        case 11:
          languageList = languageStrings;
          languageList[0] = "options.language.system_language"_lang; // replace "sys" with local language string
          rc = inst::ui::mainApp->CreateShowDialog("options.language.title"_lang, "options.language.desc"_lang,
                                                   languageList, false,
                                                   pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage(flag)));
          if (rc == -1)
            break;
          switch (rc) {
          case 0:
            inst::config::languageSetting = 0;
            break;
          case 1:
            inst::config::languageSetting = 1;
            break;
          case 2:
            inst::config::languageSetting = 2;
            break;
          case 3:
            inst::config::languageSetting = 3;
            break;
          case 4:
            inst::config::languageSetting = 4;
            break;
          case 5:
            inst::config::languageSetting = 5;
            break;
          case 6:
            inst::config::languageSetting = 6;
            break;
          case 7:
            inst::config::languageSetting = 7;
            break;
          case 8:
            inst::config::languageSetting = 8;
            break;
          case 9:
            inst::config::languageSetting = 9;
            break;
          default:
            inst::config::languageSetting = 0;
          }
          inst::config::setConfig();
          lang_message();
          break;
        case 12:
          if (inst::util::getIPAddress() == "1.0.0.127") {
            std::string update = "romfs:/images/icons/update.png";
            inst::ui::mainApp->CreateShowDialog("main.net.title"_lang, "main.net.desc"_lang, {"common.ok"_lang}, true,
                                                pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage(update)));
            break;
          }
          downloadUrl = inst::util::checkForAppUpdate();
          if (!downloadUrl.size()) {
            std::string fail = "romfs:/images/icons/fail.png";
            mainApp->CreateShowDialog("options.update.title_check_fail"_lang, "options.update.desc_check_fail"_lang,
                                      {"common.ok"_lang}, false,
                                      pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage(fail)));
            break;
          }
          this->askToUpdate(downloadUrl);
          break;
        case 13:
          inst::ui::mainApp->CreateShowDialog(
              "options.credits.title"_lang, "options.credits.desc"_lang, {"common.close"_lang}, true,
              pu::sdl2::TextureHandle::New(pu::ui::render::LoadImage("romfs:/images/icons/credits.png")));
          break;
        default:
          break;
        }
      }
    }
  }
}
} // namespace inst::ui
