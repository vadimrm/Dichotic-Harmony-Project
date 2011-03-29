#include "stdafx.h"
const wchar_t* copyright = L"Dichotic Harmony Accords Generator (c) 2010 Vadim Madgazin";
const int VER_NUM = 301; // версия*100 от 25 марта 2011 г.
/*
  =Сделано:
  -небольшой рефакторинг кода...
  -по умолчанию создаётся один диалог бокс
  -новая документация, инсталлер, midi файлы...
  -в архиве версии включены все сопутствующие файлы для публики
  -два варианта версии: в кодировке Win1251 и UTF-8 (для GitHub)

  =Надо:

  -версию в UTF-8 выложить на GitHub!

  =Следующая версия:

  -в ините доп. диалог бокса сразу грузим 7 банков 2H аккордов
  -по кнопке грузится файл 1H секвенции (моно!), которую надо преобразовать в 2H
  -каждый исходный 1H аккорд преобразуем в 2H аккорд, который выбирается (на слух!) из
   множества "подходящих" аккордов, формируемого из произвольного набора 2H банков:
   -у 1H и 2H аккордов одинаковое число голосов (N нот) и с точностью до октавного сдвига
    одинаковый состав нот
   -то же, но у 2H акода 1 дополнительная нота, отсутствующая у 1H аккорда
   -то же, но --//--     2 --//--
   -и.т.д до 9 доп. нот
  -частично готовая 2H секвенция запоминается из отдельных аккордов (с их вариантами?)
   и проигрывается по мере создания...

  =Следующая версия:

  -код для преобразования обычной музыки в 2H, см. тетрадь Projects-III, стр. 105 и далее!
  -найти хорошие полифонич. произведения (напр. короткие целиком 4-х голосные хоралы Баха)
   и сделать 2H варианты!

  -перевести самые лучшие демо файлы в .wav файлы?
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

  =Потом:

  -заменить демки дисс/музыки на более аутентичную...
  -сделать англ. инсталлер и короткую доку?
  -сделать англ. страницу программы http://www.vmgames.com/music/
  -ссылка на англ музык. страницу с главной страницы сайта!
  -сделать PAD файл, засабмитить везде и по миди-сайтам...
  -см. также файл main.h

  =Важно:

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


int dialog_numbers = 1; // умолчание для количества диалогбоксов: 1 обычный режим, 2 расширенный

const wchar_t *website = L"http://www.vmgames.com/music/"; // домашняя страница программы

wchar_t dialog_prefix = L'?'; // число диалогбоксов кодируется этим префиксом: "?1" или "?2"

MusicDialogBoxCtrl dbox[2]; // сами диалогбоксы...

int act_dialog_index = 0; // индекс текущего активного диалог бокса, 0, 1 ...

MidiInterface MIDII; // аппаратный интерфейс midi


// ссылка на текущий активный диалог бокс
inline MusicDialogBoxCtrl &act_dbox() { return dbox[act_dialog_index]; }

int WINAPI MyMsgBoxFun(HWND, LPCWSTR text, LPCWSTR, UINT)
{
  // модальный мессаг-бокс над активным диалог боксом
  return MessageBoxW( act_dbox().hwnd(), text, L"Mbox", MB_OK );
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
#ifdef DDD
  // Mbox(WCmd);
#endif
  wchar_t **argv = CommandLineToArgvW(WCmd, &argc);
  wchar_t *file = L""; // имя файла для загрузки (и проигрывания в диалог боксе 0)
  if (argc == 2) // запуск с параметром
  {
    if ( *argv[1] == dialog_prefix ) // это префикс числа диалогбоксов
      dialog_numbers = _wtol( argv[1] + 1 ); // извлекаем число
    else
      file = argv[1]; // это имя файла
  }
  else
  if (argc == 3) // запуск с 2-мя параметрами: число и имя
  {
    dialog_numbers = _wtol( argv[1] + 1 );
    file = argv[2];
  }

#ifdef DDD
  Mbox(dialog_numbers, file);
#endif

  // запускаем диалогбокс(ы), используем указатель на функцию-член класса!
  dbox[0].Create(Instance, IDD_Dialog0, HWND_DESKTOP, &MusicDialogBoxCtrl::DialogProc0);
  dbox[0].Show();
  if ( 2 == dialog_numbers )
  {
    dbox[1].Create(Instance, IDD_Dialog1, HWND_DESKTOP, &MusicDialogBoxCtrl::DialogProc1);
    dbox[1].Show();
  }

  // если есть имя - грузим (daccords) файл, а при одном боксе 0 ещё и запускаем его на проигрывание
  if ( wcslen(file) > 0 )
  {
    if ( dialog_numbers == 1 && dbox[0].LoadAccords(0, file) ) dbox[0].PlayStopAccords();
    if ( dialog_numbers == 2 )  dbox[1].LoadAccords(0, file);
  }

  // для обращение к объектам активного диалог бокса используется act_dbox() !! вроде всё работает... 
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0))
  {
    if ( act_dbox().get_check(Edit_Comment).checked_state() ) // если клавиатура нужна в диалог боксах
    {
      if ( dbox[0].IsDialogMsg(&msg) ) ; // клавиатура попадает в диалогбокс 0
      else
      if ( dbox[1].IsDialogMsg(&msg) ) ; // клавиатура попадает в диалогбокс 1
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
              act_dbox().MaybeGenerateAccord(1); // возможное вкл. аккорда
            // else это автоповтор!
            break;

          // переключаем чекбокс Swap Output (Add5 работает при NumLock)
          case KeyCod5: case KeyCodAdd5:
            if ( !(msg.lParam & 0x40000000) )
            {
              act_dbox().checks[Swap_Output].inverse_state();
              act_dbox().RegenAccord();
            }
            break;

          // сдвигаем номер текущего аккорда секвенции на -1 или +1
          // эти клавиши работают с автоповторами, но аккорд перенажимается только при их отпускании!
          case KeyCodAdd4: case KeyCod4: // почему-то Add4/6 работают только при нажатой NumLock!
          case KeyCodAdd6: case KeyCod6:
            if ( act_dbox().accords_in_chain < 2 ) break;
            if ( msg.wParam == KeyCodAdd4 || msg.wParam == KeyCod4 )
            {
              if ( --act_dbox().accord_act_index < 0) act_dbox().accord_act_index = 0;
            }
            else if ( ++act_dbox().accord_act_index >= act_dbox().accords_in_chain )
            {
              act_dbox().accord_act_index = act_dbox().accords_in_chain-1;
            }
            act_dbox().SetupAccord( act_dbox().accord_act_index );
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
            act_dbox().MaybeGenerateAccord(3); // возможное откл. аккорда
            break;

          case KeyCodAdd4: case KeyCod4: case KeyCodAdd6: case KeyCod6:
            act_dbox().RegenAccord(); // если аккорд звучал - перенажимаем аккорд
            break;

          case KeyCodP:
            if ( act_dbox().LoadAccords(1) ) act_dbox().RegenAccord();
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
            act_dbox().SetupInstrument(instrument_number);
            act_dbox().RegenAccord(); // если аккорд звучал - перенажимаем аккорд
            break;
        }
      }
      // это надо обязательно, иначе диалог вообще не работает
      DispatchMessage(&msg);
    }
  }

  return 0;
}

