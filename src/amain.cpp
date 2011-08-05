#include "stdafx.h"
const wchar_t* copyright = L"Dichotic Harmony Accords Generator (c) 2010 Vadim Madgazin";
const int VER_NUM = 304; // версия*100 от 5 августа 2011 г.
/*
  =Сделано:
  -включаем диалогбокс 2 только при включённой автоконвертации
  -убрал чекбокс Converter_Play, передвинул некот. другие контролы
  -добавил low-level манипуляторы:
   -Stereo_Base стереобаза: амплитуда панорамы звука от 0 до 100%
   -Notes_Shift сдвиг высоты всех нот из регуляторов аккорда -12...+12
    этот сдвиг считается как сдвиг в самих регуляторах высоты нот, а не доп. сдвиг при пост-обработке
    однако при воспроизведении секвенции он не обнуляется, поэтому может быть использован также как и
    основной регулятор Transposition...
  -исправил мелкие недочёты...

  =Надо:
  -сделать инсталлер, записать всё в Git!

  =Затем:
  -см. тетрадь стр. 115: составить новые библиотеки аккордов!
  -гармонизировать гамму до-мажор
  -найти одногол. музыку, преобразовать в 2H...

  =Затем:
  -перевести самые лучшие демо файлы в .wav файлы через timidity player!
  -добавить в доку ссылки на утилиты, переводящие midi в wav файлы...
  -на сайте положить несколько лучших дихотических midi/wav файлов для быстрого прослушивания!

  -написать 2-ю статью:
   "Дихотическая гармония для современного искусства композиции и аранжировки",
   описать банки аккордов, дать примеры простых "музыкальных" секвенций...
   описать методику 2H-аранжировки и дать несколько наиболее удачных примеров!

  -для статьи: обозначение отдельного аккорда (n1p1,n2p2,...,n15p15),
   где n1... - относит-й номер ноты,
       p1... - значёк панорамы (+- или пусто)
  -статью добавить на сайты и в инсталлер последней версии программы!

  -подкатиться к производителям дорогих (арматурных) наушников... к известным музыкантам...
   в организации (Консерватории, инновационные конторы и т.п.)
  -распостранить информацию среди учащихся муз. школ и студентов!!

  -заменить демки дисс/музыки на более аутентичную...
  -сделать англ. инсталлер и короткую доку?
  -сделать англ. страницу программы http://www.vmgames.com/music/
  -ссылка на англ музык. страницу с главной страницы сайта!
  -сделать PAD файл, засабмитить везде и по миди-сайтам...
  -см. также файл amain.h

  =Потом:
  =Следующая версия:
  -найти midi и 2H преобразовать сложную органную музыку (Бах), прослушать...
  -можно поискать современных композеров (типа Кейджа) - суперсложную музыку...

  =Важно:
  -если выбран чекбокс Edit_Sequence то для 2H преобразования берётся аккорд из регуляторов!
  -если не выбран чекбокс With_Unisons из группы "манипуляторов" текущего аккорда бокса 0, то перед
   2H конвертацией удаляем все лишние голоса-унисоны (с любой панорамой) и голоса-паузы аккорда!
  -в самом начале хоткеи клавиатуры попадают в бокс 0 а не в бокс 1, видимо проскакивают какие-то "левые"
   мессаги, переключающие фокус ввода на бокс 0... SetFocus() на бокс 1 тоже не даёт эффекта...
  -ноты с одинаковыми номерами в одинаковых миди каналах будут "общими" для всех диалог боксов!
  -по хоткею P - файловый диалог загрузки секвенции аккордов из модифицированных PAS файлов редактора Muse!
   при этом возможно получение секвенции с удвоенным числом голосов, когда в правый канал попадают исходные
   ноты со сдвигом на произвольное число полутонов, равное значению относительного номера ноты 15-го голоса!
  -Звуковая подсистема ПК обычно подмешивает небольшую часть звука одного канала в противоположный при
   нейтральном положении регулятора стереобаланса, и это немного ухудшает дихотичность звука...
  -Миди каналы нумеруются от 0 до 15 (на 1 меньше), т.о. канал ударных = 9
  -При максимальной громкости нот (скорость=127) звук аккорда уже из 3-х нот искажается на краях
   панорамы (макс. слева или справа), но громкость = 100 избавляет от этой грязи!
  -velocity отпускаемой ноты иногда используется для регулировки процесса затухания, поэтому лучше её
   делать равной тому же значению что и при нажатии...
  -короткие ноты (10-20 мсек) воспроизводятся в системном midi-плеере гораздо точнее чем в DHAG
   daccords-плеере: секвенция из 100 нот по 10 мсек замедляется в нём раза в 3!
*/

const wchar_t *files_default_dir = L"accords"; // директория daccords файлов относительно программы
// const wchar_t *files_default_dir = L".";

const int DNUMS = 3; // макс. количество диалог боксов

const int DIALOG_VOICES[DNUMS] = {12, 6, 6}; // число голосов с контролами в боксе [index], не более MAX_MIDI_CHANNELS = 15

const wchar_t *website = L"http://www.vmgames.com/music/"; // домашняя страница программы

MusicDialogBoxCtrl dbox[DNUMS]; // сами диалогбоксы...

int act_dialog_index = 0; // индекс текущего активного диалог бокса, 0, 1 ...

MidiInterface MIDII; // аппаратный интерфейс midi


// ссылка на текущий активный диалог бокс
MusicDialogBoxCtrl &actbox() { return dbox[act_dialog_index]; }

int WINAPI MyMsgBoxFun(HWND, LPCWSTR text, LPCWSTR, UINT)
{
  // модальный мессаг-бокс над активным диалог боксом
  return MessageBoxW( actbox().hwnd(), text, L"Mbox", MB_OK );
}

int WINAPI WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR Cmd, int CmdShow)
{
  InitCommonControls();
  // изменяем указатель на мою функцию мессаг-бокса, которая знает хендл текущего активного диалог бокса
  MsgBoxFun = MyMsgBoxFun;
  MusicDialogBoxCtrl::LoadData();

  // отрабатываем запуск типа drag-n-drop файл на иконку приложения
  int argc;
  wchar_t *WCmd = GetCommandLineW();
  wchar_t **argv = CommandLineToArgvW(WCmd, &argc);
  wchar_t *file = (wchar_t*)L""; // имя файла для загрузки (и проигрывания в диалог боксе 0)
  if (argc == 2) file = argv[1]; // запуск с параметром "имя файла"
#ifdef DDD
  Mbox(WCmd, file);
#endif

  // запускаем диалогбоксы, используем указатель на функцию-член класса!
  dbox[0].Create(Instance, IDD_Dialog0, HWND_DESKTOP, &MusicDialogBoxCtrl::DialogProc0);
  dbox[1].Create(Instance, IDD_Dialog1, HWND_DESKTOP, &MusicDialogBoxCtrl::DialogProc1);
  dbox[2].Create(Instance, IDD_Dialog2, HWND_DESKTOP, &MusicDialogBoxCtrl::DialogProc2);
  // сначала показываем только главный диалогбокс 0!
  dbox[0].Show();

  // если есть - грузим daccords файл и запускаем его на проигрывание
  if ( wcslen(file) > 0 && dbox[0].LoadAccords(0, file) ) dbox[0].Play_StopAccords();

  // для обращение к объектам активного диалог бокса используется actbox() !! вроде всё работает...
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0))
  {
    if ( actbox().get_check(Edit_Comment).checked_state() ) // если клавиатура нужна в диалог боксах
    {
      if ( dbox[0].IsDialogMsg(&msg) ) ; // клавиатура попадает в диалогбокс 0
      else
      if ( dbox[1].IsDialogMsg(&msg) ) ; // клавиатура попадает в диалогбокс 1
      else
      if ( dbox[2].IsDialogMsg(&msg) ) ; // клавиатура попадает в диалогбокс 2
      else
        DispatchMessage(&msg); // остальные сообщения передаем стандартному обработчику
    }
    else // клавиатура не нужна в боксах, это независимые от номера бокса хоткеи, они работают здесь!
    {
      if (msg.message == WM_KEYDOWN)
      {
        switch (msg.wParam)
        {
          default: break;

          // генерация звука аккорда
          case KeyCodIns: case KeyCodAdd0: case KeyCodCtrl:
            if ( !(msg.lParam & 0x40000000) )
              actbox().MaybeGenerateAccord(1); // возможное вкл. аккорда
            // else это автоповтор!
            break;

          // переключаем чекбокс Swap Output (Add5 работает при NumLock)
          case KeyCod5: case KeyCodAdd5:
            if ( !(msg.lParam & 0x40000000) )
            {
              actbox().checks[Swap_Output].inverse_state();
              actbox().RegenAccord();
            }
            break;

          // сдвигаем номер текущего аккорда секвенции на -1 или +1
          // эти клавиши работают с автоповторами, но аккорд перенажимается только при их отпускании!
          case KeyCodAdd4: case KeyCod4: // почему-то Add4/6 работают только при нажатой NumLock!
          case KeyCodAdd6: case KeyCod6:
            if ( actbox().accords_in_chain < 2 ) break;
            if ( msg.wParam == KeyCodAdd4 || msg.wParam == KeyCod4 )
            {
              if ( --actbox().accord_act_index < 0) actbox().accord_act_index = 0;
            }
            else if ( ++actbox().accord_act_index >= actbox().accords_in_chain )
            {
              actbox().accord_act_index = actbox().accords_in_chain-1;
            }
            actbox().SetupAccord( actbox().accord_act_index );
            break;
        }
      }

      if (msg.message == WM_KEYUP)
      {
        switch (msg.wParam)
        {
          int instrument_number;

          default: break;

          case KeyCodIns: case KeyCodAdd0: case KeyCodCtrl:
            actbox().MaybeGenerateAccord(3); // возможное откл. аккорда
            break;

          case KeyCodAdd4: case KeyCod4: case KeyCodAdd6: case KeyCod6:
            actbox().RegenAccord(); // если аккорд звучал - перенажимаем аккорд
            break;

          case KeyCodP:
            if ( actbox().LoadAccords(1) ) actbox().RegenAccord();
            break;

          // переключение midi инструментов по клавишам F1...F8
          case KeyCodF1:  instrument_number =  0;  goto setup_instrument; // "000 Acoustic Grand Piano"
          case KeyCodF2:  instrument_number =  2;  goto setup_instrument; // "002 Electric Grand Piano"
          case KeyCodF3:  instrument_number = 16;  goto setup_instrument; // "016 Drawbar Organ"
          case KeyCodF4:  instrument_number = 19;  goto setup_instrument; // "019 Church Organ"
          case KeyCodF5:  instrument_number = 20;  goto setup_instrument; // "020 Reed Organ"
          case KeyCodF6:  instrument_number = 25;  goto setup_instrument; // "025 Acoustic Guitar (steel)"
          case KeyCodF7:  instrument_number = 40;  goto setup_instrument; // "040 Violin"
          case KeyCodF8:  instrument_number = 43;  goto setup_instrument; // "043 Contrabass"
          // т.к. нажатие F10 не отрабатывается - не используем F9...F12!

          // установим нужный номер инструмента и отобразим его на диалогпанели
          setup_instrument:
            MusicDialogBoxCtrl::SetInstrument(instrument_number);
            actbox().RegenAccord(); // если аккорд звучал - перенажимаем аккорд
            break;
        }
      }
      // это надо обязательно, иначе диалог вообще не работает
      DispatchMessage(&msg);
    }
  }

  return 0;
}

