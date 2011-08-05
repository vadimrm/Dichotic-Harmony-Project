
#include "stdafx.h"

// при старте программы:
int midi_instrument_number = 1; // номер инструмента для всех голосов аккорда, 1 = Bright Acoustic Piano
int midi_device_number = 0; // номер midi девайса: 0 (без синтюка) = программный синтезатор звуковой

// массив идентификаторов для массива контролов MusicDialogBoxCtrl::Ar <DichoticVoice> dvoices
int control_id_array[][3] =
{
  // численные значения любых из этих идентификаторов произвольны!!
  { IDC_Note_Voice_1, IDC_Pan_Voice_1, IDC_Switchon_Voice_1, },
  { IDC_Note_Voice_2, IDC_Pan_Voice_2, IDC_Switchon_Voice_2, },
  { IDC_Note_Voice_3, IDC_Pan_Voice_3, IDC_Switchon_Voice_3, },
  { IDC_Note_Voice_4, IDC_Pan_Voice_4, IDC_Switchon_Voice_4, },
  { IDC_Note_Voice_5, IDC_Pan_Voice_5, IDC_Switchon_Voice_5, },
  { IDC_Note_Voice_6, IDC_Pan_Voice_6, IDC_Switchon_Voice_6, },
  { IDC_Note_Voice_7, IDC_Pan_Voice_7, IDC_Switchon_Voice_7, },
  { IDC_Note_Voice_8, IDC_Pan_Voice_8, IDC_Switchon_Voice_8, },
  { IDC_Note_Voice_9, IDC_Pan_Voice_9, IDC_Switchon_Voice_9, },
  { IDC_Note_Voice_10, IDC_Pan_Voice_10, IDC_Switchon_Voice_10, },
  { IDC_Note_Voice_11, IDC_Pan_Voice_11, IDC_Switchon_Voice_11, },
  { IDC_Note_Voice_12, IDC_Pan_Voice_12, IDC_Switchon_Voice_12, },
};

// массивы инициализации в каждом диалог боксе всех контролов-чекбоксов
CheckCtrl check[][24] =
{
  // диалог бокс 0
  {
  { IDC_CHECK_Generate_Accord, BST_UNCHECKED, { UNI_NULL_STR } }, // генерировать аккорд основной секвенции
  { IDC_CHECK_Switchon_Left_Pan, BST_CHECKED, { UNI_NULL_STR } }, // включение левой панорамы
  { IDC_CHECK_Switchon_Mid_Pan, BST_CHECKED, { UNI_NULL_STR } },  // включение средней панорамы
  { IDC_CHECK_Switchon_Right_Pan, BST_CHECKED, { UNI_NULL_STR } },// включение правой панорамы
  { IDC_CHECK_Loop_Chain, BST_UNCHECKED, { UNI_NULL_STR } }, // зациклить секвенцию

  { IDC_CHECK_Edit_Sequence, BST_UNCHECKED, { UNI_NULL_STR } }, // разрешение редактирования текущего аккорда секвенции
  { IDC_CHECK_Save_With_Manipuls, BST_UNCHECKED, { UNI_NULL_STR } }, // учёт реал-тайм контролов при запись в файл
  { IDC_CHECK_Sort_Mode, BST_UNCHECKED, { L"Sort Mode 3+3", L"Sort Mode Mono", UNI_NULL_STR } },
  { IDC_CHECK_Big_Duration, BST_CHECKED, { UNI_NULL_STR } }, // меняет предел слайдера Duration
  // динамическое приведение всех номеров нот к диапазону одной октавы вверх от нижнего голоса
  { IDC_CHECK_One_Octave, BST_UNCHECKED, { UNI_NULL_STR } },

  // игнорирование инструментов из аккордов секвенции (вместо них звучит текущий инструмент диалогбокса)
  { IDC_CHECK_Dont_Change_GM_Instr, BST_UNCHECKED, { UNI_NULL_STR } }, // "не менять General Midi инструмент"
  { IDC_CHECK_Edit_Comment, BST_UNCHECKED, { UNI_NULL_STR } }, // флаг пересылки клавиатурных сообщений в диалогбокс!
  { IDC_CHECK_With_Unisons, BST_UNCHECKED, { UNI_NULL_STR } }, // если сброшен: без унисонов, незвучание совпадающих нот
  { IDC_CHECK_Dichotic_Output, BST_CHECKED, { UNI_NULL_STR } },// стерео-моно
  { IDC_CHECK_Swap_Output, BST_UNCHECKED, { UNI_NULL_STR } },  // обмен звук. каналов местами

  // при включении чекбокса В ИНДИКАТОРАХ нот аккорда все номера нот сдвигаются до min=0
  { IDC_CHECK_Zero_Ground_Notes, BST_UNCHECKED, { L"Original Notes", L"Zero Ground Notes", UNI_NULL_STR } },
  // исключаем все голоса-паузы из аккорда, это надо для записи в файл...
  { IDC_CHECK_Dont_Save_Empty_Voices, BST_UNCHECKED, { UNI_NULL_STR } },
  { IDC_CHECK_Show_Sorted_Notes, BST_UNCHECKED, { UNI_NULL_STR } }, // сортируем голоса в порядке возрастания номеров нот
  // общий уровень громкости 100%, ~10%, ~1% этот контрол меняет громкость на всех диалог боксах!!
  { IDC_CHECK_Mute_Sound, BST_UNCHECKED, { L"Don't Mute", L"Mute Sound", L"Soundless" } }, // Tri-state чекбокс!
  { IDC_CHECK_Converter_Mode, BST_UNCHECKED, { UNI_NULL_STR } }, // включение режима 2H преобразования музыки

  { IDC_CHECK_Converter_Auto, BST_UNCHECKED, { UNI_NULL_STR } }, // автоматическое преобразование
  { IDC_CHECK_Converter_Mirror_Accords, BST_UNCHECKED, { UNI_NULL_STR } }, // оставлять зеркальные аккорды
  { IDC_CHECK_Converter_With_Unisons,   BST_UNCHECKED, { UNI_NULL_STR } }, // оставлять аккорды с унисонами
  { IDC_CHECK_No_Sound, BST_UNCHECKED, { UNI_NULL_STR } }, // если выбран, то кнопка Generate Accord делает всё кроме звука
  },
  // для диалог бокса 1 некоторые чекбоксы не нужны, но без их нормальной инициализации прога виснет!
  {
  check[0][ 0],check[0][ 1],check[0][ 2],check[0][ 3],check[0][ 4],check[0][ 5],check[0][ 6],check[0][ 7],check[0][ 8],check[0][ 9],
  check[0][10],check[0][11],check[0][12],check[0][13],check[0][14],check[0][15],check[0][16],check[0][17],check[0][18],check[0][19],
  check[0][20],check[0][21],check[0][22],check[0][23],
  },
  // то же самое для диалогбокса 2
  {
  check[0][ 0],check[0][ 1],check[0][ 2],check[0][ 3],check[0][ 4],check[0][ 5],check[0][ 6],check[0][ 7],check[0][ 8],check[0][ 9],
  check[0][10],check[0][11],check[0][12],check[0][13],check[0][14],check[0][15],check[0][16],check[0][17],check[0][18],check[0][19],
  check[0][20],check[0][21],check[0][22],check[0][23],
  }
};

ButtonCtrl button[][3] =
{
  {
  { IDC_Play_Stop, L"Play Chain", L"Stop Chain", 0, 0 }, // кнопка Play/Stop
  { IDC_Rewind, L"Rewind", UNI_NULL_STR, 0, 0 },         // кнопка Rewind
  { IDC_Save_Accords_Chain, L"Save Accords Chain", UNI_NULL_STR, 0, 0 }, // кнопка Save Accords Chain
  },
  { button[0][0], button[0][1], button[0][2], },
  { button[0][0], button[0][1], button[0][2], },
};

TextSliderCtrl::Members slider[][8] =
{
  {
  // номер аккорда, индикатор-регулятор при проигрывании секвенции аккордов
  { 1, 1, 1, 10, IDC_SLIDER_Accord_Number, IDC_STATIC_Accord_Number, L"Accord ", 1, 1, 1 },
  // скорость проигрывания секвенции 0.1...10
  { 1, 100, 1, 10, IDC_SLIDER_Chain_Speed, IDC_STATIC_Chain_Speed, L"Chain Speed  ", 10, 0.1, 0 },
  // скорость нажатия миди ноты
  { 1, 127, 1, 10, IDC_SLIDER_Accord_Volume, IDC_STATIC_Accord_Volume, L"Volume    ", 100, 1, 0 },
  // параметр транспозиции: это абсолютный миди номер ноты с относительным номером 0
  { 0, 120, 1, 12, IDC_SLIDER_Transposition, IDC_STATIC_Transposition, L"Transposition  ", 60, 1, 0 },
  // длительность аккорда (ударов метронома), для записи в файл и как индикатор при проигрывании
  { 1, 1000, 1, 10, IDC_SLIDER_Accord_Duration, IDC_STATIC_Duration, L"Duration   ", 1, 1, 0 }, // зависит от Big_Duration

  // темп (ударов метронома в секунду), для записи в файл и как индикатор при проигрывании
  { 1, 1000, 1, 10, IDC_SLIDER_Accord_Temp, IDC_STATIC_Temp, L"Temp (bps)  ", 1, 1, 0 },
  // стереобаза: амплитуда панорамы звука от 0 до 100%
  { 0, 100, 1, 10, IDC_SLIDER_Stereo_Base, IDC_STATIC_Stereo_Base, L"Stereo ", 100, 0.01, 0 },
  // сдвиг высоты всех нот из регуляторов аккорда -12...+12
  { -12, 12, 1, 12, IDC_SLIDER_Notes_Shift, IDC_STATIC_Notes_Shift, L"Shift ", 0, 1, 0 },
  },

  // слайдеры [>=2] отсутствуют на диалог боксе 1, но их значения используются в коде функций бокса 1
  { slider[0][0], slider[0][1], slider[0][2], slider[0][3], slider[0][4], slider[0][5], slider[0][6], slider[0][7], },
  // то же самое для диалогбокса 2
  { slider[0][0], slider[0][1], slider[0][2], slider[0][3], slider[0][4], slider[0][5], slider[0][6], slider[0][7], },
};

ComboBoxCtrl combo[][2] =
{
  {
  { IDC_GM_Instrument }, // midi инструмент всех голосов
  { IDC_MidiOutDevice }, // устройство midi вывода
  },
  { combo[0][0], combo[0][1] },
  { combo[0][0], combo[0][1] },
};

EditCtrl edit[][1] =
{
  {
  { IDC_EDIT_Accords_Chain_Comment, 1, 1 }, // строка комментария к секвенции аккордов
  },
  { edit[0][0] },
  { edit[0][0] },
};

TextCtrl textctrl[][5] =
{
  {
  { IDC_STATIC_LEFT,  L"Left:    " }, // ноты аккорда в левой точке панорамы
  { IDC_STATIC_MID,   L"Mid:    " },  // ноты аккорда в центре панорамы
  { IDC_STATIC_RIGHT, L"Right:  " },  // ноты аккорда в правой точке панорамы
  { IDC_STATIC_Total_Diss, L"Dichotic Diss  " }, // полный диссонанс аккорда
  { IDC_STATIC_Uniq_Notes, L"Unique Notes  " },  // число уникальных нот аккорда
  },
  { textctrl[0][0], textctrl[0][1], textctrl[0][2], textctrl[0][3], textctrl[0][4], },
  { textctrl[0][0], textctrl[0][1], textctrl[0][2], textctrl[0][3], textctrl[0][4], },
};

// ===============================  MusicDialogBoxCtrl  =====================================

int MusicDialogBoxCtrl::diss_rating[MAX_INTERVAL];

inline double MusicDialogBoxCtrl::AccordDtime()
{
  return sliders[Accord_Duration].actpos /
       ( sliders[Chain_Speed].get_reg_value() * (double)sliders[Accord_Temp].actpos );
}

void MusicDialogBoxCtrl::LoadData()
// грузим параметры программы из файлов
// числа в файлах должны отделяться друг от друга пробелом (пробелами) или новой строкой (строками)!
{
  const wstring2 dr_file = "drfile.txt";

  {
  // читаем файл как незашифрованный текстовый файл (ANSI или Unicode)
  TextFile file(dr_file);
  if (file.length != 0) // если файл есть
  {
    // копируем текст в строку2 и вводим из неё в массив
    wstring2 data = file.content;
    for (int i = 0; i < MAX_INTERVAL; ++i) data >> diss_rating[i];
  }
  }
}

void MusicDialogBoxCtrl::PrintFileTitle(const wchar_t *filename, const wchar_t *ext, bool load)
// выводим в титуле диалог бокса название файла с расширением, считанного при load=true, или записанного
{
  wstring2 str;
  if ( load ) str << L"Load file ";
  else        str << L"Save file ";
  str << filename << L"." << ext;
  SetTitle( str );
}

void MusicDialogBoxCtrl::SetTitle(const wchar_t *str)
{
  wstring2 title;
  if ( wcslen(str) == 0 ) { title << copyright; title << L" Version " << 0.01*VER_NUM; }
  else                      title = str;
  DialogBoxCtrl::SetTitle(  title.c_str()  );
}

void MusicDialogBoxCtrl::SortSimilarAccordsMono()
// аккорд секвенции Accord_Number становится 1-м, а остальные аккорды выстраиваются по степени
// совпадения с ним (количество совпадающих нот в монофоническом варианте), без записи в файл!
// все аккорды должны быть N-голосными (N = const, N >= 2) и не должны иметь голоса-паузы...
{
  if (accords_in_chain < 2) return;
  int voices = accords_chain[0].voices_number;
  if (voices < 2) return;

  DichoticAccord *accords1 = accords_chain;
  Ar <DichoticAccord> accords2(accords_in_chain);

  int n, m;
  // копируем исходную секвенцию аккордов во 2-ю с сортировкой
  for (n = 0; n < accords_in_chain; ++n)
  {
    accords2[n] = accords1[n];
    // сортируем голоса аккорда по возрастанию нот
    qsort(accords2[n].dn, voices, sizeof(DichoticNote), DichoticNote::CmpNote);
  }

  // копируем "опорный аккорд" с индексом accord_act_index = Accord_Number.actpos-1
  DichoticAccord main_acc = accords2[accord_act_index];
  // определяем "отличие" (сумма модулей разности нот) каждого аккорда от опорного
  for (n = 0; n < accords_in_chain; ++n)
  {
    int sum = 0;
    for (m = 0; m < voices; ++m) sum += abs(main_acc.dn[m].note - accords2[n].dn[m].note);
    // отличие записываем в исходную секвенцию!!
    accords1[n].spare1 = sum;
  }

  // сортируем исходную секвенцию по возрастанию отличий аккордов от опорного
  qsort(accords1, accords_in_chain, sizeof(DichoticAccord), DichoticAccord::CmpAcc2);

  // меняем индекс текущего аккорда на 0-й, в котором теперь находится опорный аккорд
  accord_act_index = 0;
  SetupAccord(accord_act_index);
}

void MusicDialogBoxCtrl::SortSimilarAccords3and3()
// работает примерно как SortSimilarAccordsMono(), но отличия определяет по (дихотическим)
// половинкам 6-голосных аккордов:
// сравнивает ноты голосов 0-2 и 3-5, выбирая суммарно-минимально отличные комбинации половинок!
{
  if (accords_in_chain < 2) return;
  int voices = accords_chain[0].voices_number;
  if (voices != 6) return;
  int voices2 = voices/2; // 3 голоса в половинке аккорда

  DichoticAccord *accords1 = accords_chain;
  Ar <DichoticAccord> accords2(accords_in_chain);

  int n, m;
  // копируем исходную секвенцию аккордов во 2-ю с сортировкой
  for (n = 0; n < accords_in_chain; ++n)
  {
    accords2[n] = accords1[n];
    // сортируем половинки аккорда по возрастанию нот
    qsort(&accords2[n].dn[0],        voices/2, sizeof(DichoticNote), DichoticNote::CmpNote);
    qsort(&accords2[n].dn[voices/2], voices/2, sizeof(DichoticNote), DichoticNote::CmpNote);
  }

  // копируем "опорный аккорд" с индексом accord_act_index = Accord_Number.actpos-1
  DichoticAccord main_acc1 = accords2[accord_act_index];
  // делаем 2-й вариант опорного аккорда, с обменом местами его (дихотических) половинок
  DichoticAccord main_acc2 = main_acc1;
  for (m = 0; m < voices2; ++m) swap( main_acc2.dn[m].note, main_acc2.dn[voices2+m].note );

  // определяем "отличие" (сумма модулей разности нот) каждого аккорда от опорного
  for (n = 0; n < accords_in_chain; ++n)
  {
    // суммарное отличие от каждого варианта опорного аккорда
    int sum1 = 0, sum2 = 0;
    for (m = 0; m < voices; ++m)
    {
      sum1 += abs(main_acc1.dn[m].note - accords2[n].dn[m].note);
      sum2 += abs(main_acc2.dn[m].note - accords2[n].dn[m].note);
    }
    // минимальное отличие записываем в исходную секвенцию!!
    accords1[n].spare1 = min( sum1, sum2 );
  }

  // сортируем исходную секвенцию по возрастанию отличий аккордов от опорного
  qsort(accords1, accords_in_chain, sizeof(DichoticAccord), DichoticAccord::CmpAcc2);

  // меняем индекс текущего аккорда на 0-й, в котором теперь находится опорный аккорд
  accord_act_index = 0;
  SetupAccord(accord_act_index);
}

void MusicDialogBoxCtrl::AddInversePanAccords()
// добавляет после каждого аккорда точно такой же, но с инверсной панорамой!
{
  if (accords_in_chain < 1 || accords_in_chain > MAX_ACCORDS/2) return;
  DichoticAccord *accords1 = accords_chain;
  Ar <DichoticAccord> accords2(accords_in_chain);

  int n, m;
  // копируем исходную секвенцию аккордов во 2-ю
  for (n = 0; n < accords_in_chain; ++n) accords2[n] = accords1[n];

  // копируем обратно в исходную секвенцию с удвоением каждого аккорда
  for (n = 0; n < accords_in_chain; ++n)
  {
    accords1[2*n] = accords1[2*n+1] = accords2[n];
    int voices = accords2[n].voices_number;
    for (m = 0; m < voices; ++m)
    {
      accords1[2*n+1].dn[m].pan = -accords1[2*n+1].dn[m].pan;
    }
  }

  // удваиваем число аккордов и предел регулятора номера аккорда
  accords_in_chain = 2*accords_in_chain;
  sliders[Accord_Number].reset_minmax(1, accords_in_chain);
}

void MusicDialogBoxCtrl::DeleteIdenticAccords6()
// функция для обработки "сырых" 6 голосных дихотик-секвенций (3+3голоса):
// отбрасывание лишних совпадающих аккордов и сортировка в порядке увеличения нот, запись в файл!
{
  int n, m;

  // переставляем голоса в 1-й и 2-й тройке голосов по возрастанию номеров нот
  for (n = 0; n < accords_in_chain; ++n)
  {
    DichoticAccord &acc = accords_chain[n];
    if (acc.voices_number == 6)
    {
      DichoticAccord::Sort3notes(acc, 0); // сортируем ноты голосов 0,1,2
      DichoticAccord::Sort3notes(acc, 3); // сортируем ноты голосов 3,4,5
      // первым будет та тройка голосов, которая начинается с меньшего номера ноты
      // если начальные ноты двух троек одинаковы, сравниваем следующую пару нот (и т.д.)
      bool sw = 0;
      if (acc.dn[0].note > acc.dn[3].note) sw = 1;
      else
      if (acc.dn[0].note == acc.dn[3].note)
      {
        if (acc.dn[1].note > acc.dn[4].note) sw = 1;
        else
        if (acc.dn[1].note == acc.dn[4].note)
        {
          if (acc.dn[2].note > acc.dn[5].note) sw = 1;
        }
      }
      if (sw)
      {
        swap(acc.dn[0].note, acc.dn[3].note);
        swap(acc.dn[1].note, acc.dn[4].note);
        swap(acc.dn[2].note, acc.dn[5].note);
      }
    }
  }

  // уничтожаем совпадающие аккорды - делаем из них паузы
  for (n = 0; n < accords_in_chain; ++n)
  {
    DichoticAccord &acc1 = accords_chain[n];
    if (acc1.voices_number != 6) continue;
    for (m = n+1; m < accords_in_chain; ++m)
    {
      DichoticAccord &acc2 = accords_chain[m];
      if (acc2.voices_number != 6) continue;
      if ( acc1.dn[0].note == acc2.dn[0].note &&
           acc1.dn[1].note == acc2.dn[1].note &&
           acc1.dn[2].note == acc2.dn[2].note &&
           acc1.dn[3].note == acc2.dn[3].note &&
           acc1.dn[4].note == acc2.dn[4].note &&
           acc1.dn[5].note == acc2.dn[5].note ) acc2.voices_number = 0;
    }
  }

  // уничтожаем паузы, сдвигая аккорды к началу секвенции
  for (n = 0; n < accords_in_chain; ++n)
  {
    DichoticAccord &acc1 = accords_chain[n];
    if (acc1.voices_number == 0) // найдена пауза
    {
      // ищем аккорд (не паузу), ставим его на место паузы
      for (m = n+1; m < accords_in_chain; ++m)
      {
        DichoticAccord &acc2 = accords_chain[m];
        if (acc2.voices_number != 0)
        {
          swap(acc1, acc2);
          break;
        }
      }
    }
  }

  // определяем и уменьшаем число аккордов в цепочке на число пауз
  int npause = 0; // количество пауз в секвенции
  for (n = 0; n < accords_in_chain; ++n) if ( accords_chain[n].voices_number == 0 ) ++npause;
  accords_in_chain -= npause;

  // сортируем секвенцию по возрастанию номеров нот, 1-й голос старший, 2-й следующий и т.д.
  if (accords_in_chain > 0)
  {
    qsort( accords_chain.memory(), accords_in_chain, sizeof(DichoticAccord), DichoticAccord::CmpAcc );
    // записываем отсортированную секвенцию без дополнительных изменений в файл
    SaveMusic(3);
  }
}

void MusicDialogBoxCtrl::DeleteIdenticAccords4()
// функция для обработки "сырых" 4 голосных (или 5 голосных) дихотик-секвенций (2+2голоса):
// отбрасывание лишних совпадающих аккордов и сортировка в порядке увеличения нот, запись в файл!
// если число голосов равно 5, то сначала средний голос отбрасывается (считается что в нём пауза)
{
  int n, m;

  // если число голосов равно 5, то средний голос отбрасывается
  for (n = 0; n < accords_in_chain; ++n)
  {
    DichoticAccord &acc = accords_chain[n];
    if (acc.voices_number == 5)
    {
      // голос acc.dn[2] считается паузой, голоса [3] и [4] сдвигаются вниз!
      acc.dn[2] = acc.dn[3];
      acc.dn[3] = acc.dn[4];
      acc.voices_number = 4;
    }
  }

  // переставляем голоса в 1-й и 2-й паре голосов по возрастанию номеров нот
  for (n = 0; n < accords_in_chain; ++n)
  {
    DichoticAccord &acc = accords_chain[n];
    if (acc.voices_number == 4)
    {
      DichoticAccord::Sort2notes(acc, 0); // сортируем ноты голосов 0,1
      DichoticAccord::Sort2notes(acc, 2); // сортируем ноты голосов 2,3
      // первым будет та пара голосов, которая начинается с меньшего номера ноты
      // если начальные ноты двух пар одинаковы, сравниваем следующую пару нот
      bool sw = 0;
      if (acc.dn[0].note > acc.dn[2].note) sw = 1;
      else
      if (acc.dn[0].note == acc.dn[2].note)
      {
        if (acc.dn[1].note > acc.dn[3].note) sw = 1;
      }
      if (sw)
      {
        swap(acc.dn[0].note, acc.dn[2].note);
        swap(acc.dn[1].note, acc.dn[3].note);
      }
    }
  }

  // уничтожаем совпадающие аккорды - делаем из них паузы
  for (n = 0; n < accords_in_chain; ++n)
  {
    DichoticAccord &acc1 = accords_chain[n];
    if (acc1.voices_number != 4) continue;
    for (m = n+1; m < accords_in_chain; ++m)
    {
      DichoticAccord &acc2 = accords_chain[m];
      if (acc2.voices_number != 4) continue;
      if ( acc1.dn[0].note == acc2.dn[0].note &&
           acc1.dn[1].note == acc2.dn[1].note &&
           acc1.dn[2].note == acc2.dn[2].note &&
           acc1.dn[3].note == acc2.dn[3].note ) acc2.voices_number = 0;
    }
  }

  // уничтожаем паузы, сдвигая аккорды к началу секвенции
  for (n = 0; n < accords_in_chain; ++n)
  {
    DichoticAccord &acc1 = accords_chain[n];
    if (acc1.voices_number == 0) // найдена пауза
    {
      // ищем аккорд (не паузу), ставим его на место паузы
      for (m = n+1; m < accords_in_chain; ++m)
      {
        DichoticAccord &acc2 = accords_chain[m];
        if (acc2.voices_number != 0)
        {
          swap(acc1, acc2);
          break;
        }
      }
    }
  }

  // определяем и уменьшаем число аккордов в цепочке на число пауз
  int npause = 0; // количество пауз в секвенции
  for (n = 0; n < accords_in_chain; ++n) if ( accords_chain[n].voices_number == 0 ) ++npause;
  accords_in_chain -= npause;

  // сортируем секвенцию по возрастанию номеров нот, 1-й голос старший, 2-й следующий и т.д.
  if (accords_in_chain > 0)
  {
    qsort( accords_chain.memory(), accords_in_chain, sizeof(DichoticAccord), DichoticAccord::CmpAcc );
    // записываем отсортированную секвенцию без дополнительных изменений в файл
    SaveMusic(3);
  }
}

DichoticAccord MusicDialogBoxCtrl::GetAccord()
// считываем все контролы, формируем аккорд (до манипуляторов)
{
  DichoticAccord acc;

  acc.instrument = combos[GM_Instrument].GetStringIndex();
  acc.duration = sliders[Accord_Duration].actpos;
  acc.temp = sliders[Accord_Temp].actpos;
  acc.voices_number = 0; // см. ниже
  acc.clear_comment(); // стираем коммент аккорда
  acc.volume = sliders[Accord_Volume].actpos;

  int note_shift = sliders[Notes_Shift].actpos; // low-level сдвиг высоты нот аккорда

  for (int i = 0; i < DichoticAccord::MAX_ACC_VOICES; ++i)
  {
    if ( dvoices[i].get_switchon() ) // нота голоса
    {
      acc.voices_number = i+1; // число голосов в аккорде, любые паузы голоса тоже считаются!

      acc.dn[i].pause = 0;
      acc.dn[i].note = note_shift + dvoices[i].get_note();
      acc.dn[i].pan = dvoices[i].get_pan();
    }
    else // пауза голоса
    {
      acc.dn[i].pause = 1;
      acc.dn[i].note = 0;
      acc.dn[i].pan = 0;
    }
  }

  return acc;
}

ChainHeader MusicDialogBoxCtrl::ChainHeaderFromControls()
// считываем контролы общих параметров, возвращаем их в виде ChainHeader
{
  ChainHeader ch;
  ch.transposition = sliders[Transposition].actpos;
  ch.chain_speed = sliders[Chain_Speed].get_reg_value();
  ch.dont_change_gm_instrument = checks[Dont_Change_GM_Instr].checked_state();
  ch.instrument_number = combos[GM_Instrument].GetStringIndex();
  return ch;
}

void MusicDialogBoxCtrl::SaveMusic(int mode)
// при mode=0 (Save_Accord       ) записываем текущий аккорд в файл, если файл есть - стираем и перезаписываем его
// при      1 (Append_Accord     ) добавляем аккорд к сушествующему файлу, а если файла нет - пишем его с нуля!
// при      2 (Save_Accords_Chain) записываем секвенцию аккордов в файл, если файл есть - стираем и перезаписываем его
// при      3 (запись секвенции после сортировки) то же что и при 2, кроме замены текущего аккорда!
// при (2) сначала текущий звучащий аккорд копируется в текущее место секвенции, т.о. возможно её редактирование!
// при (0,1,3) секвенцию не меняем!
// если чекбокс Edit_Sequence не выбран - секвенцию не меняем и при mode=2!
{
  // создаем данные текущего аккорда из контролов
  ChainHeader ch = ChainHeaderFromControls(); // считываем контролы общих параметров
  // Accords_Chain_Comment добавляется в MakeAccordsStrings()
  DichoticAccord acc = GetAccord(); // считываем контролы голосов, формируем аккорд (до манипуляторов)
  // манипуляторы обрабатываются в SaveAs(), т.к. при записи секвенции аккордов много!

  MusicFile mfile( hwnd() );

  if (mode == 0 || mode == 1)
  {
    // записываем один аккорд в файл в режиме mode
    mfile.SaveAs(ch, &acc, 1, mode);
  }
  else // mode = 2 или 3
  {
    // секвенция должна существовать, проигрывание должно быть остановлено
    if (accords_in_chain == 0 || play_accords_chain != 0) return;

    if ( mode == 2 && checks[Edit_Sequence].checked_state() ) // меняем текущий аккорд в секвенции!
    {
      // индекс редактируемого аккорда должен быть в пределах секвенции
      if ( !in_range(0, accord_act_index, accords_in_chain-1) ) return;

      // если будем игнорировать инструменты из аккордов - не меняем инструмент текущего аккорда в секвенции
      if (ch.dont_change_gm_instrument) acc.instrument = accords_chain[accord_act_index].instrument;

      // копируем аккорд в секвенцию, без изменения исходного комментария старого аккорда секвенции!
      accords_chain[accord_act_index].copy_wo_comment( acc );
    }
    // else не меняем секвенцию!

    // записываем секвенцию аккордов в новый файл
    mfile.SaveAs(ch, accords_chain, accords_in_chain, mode);
  }
}

void MusicDialogBoxCtrl::ResetMaxDuration()
{
  // устанавливаем предел слайдера Accord_Duration
  checks[Big_Duration].getstate();
  int new_max = checks[Big_Duration].checked_state()? 10000:1000;
  sliders[Accord_Duration].reset_minmax(1, new_max);
  int pos = sliders[Accord_Duration].getpos();
  sliders[Accord_Duration].setpos(pos);
}

void MusicDialogBoxCtrl::TimerMessage()
{
  static Times chain_time(Times::mid); // датчик времени секвенции с точностью 1 мсек
  static double acc_dtime = 0; // длительность проигрываемого аккорда, сек

  // запускаем секвенцер аккордов
  if (play_accords_chain == 1)
  {
    // если аккорд звучал (из-за других причин) - отключаем аккорд
    MaybeGenerateAccord(3);
    // копируем параметры аккорда в диалог-оснастку
    SetupAccord(accord_act_index);
    // длительность аккрда в сек
    acc_dtime = AccordDtime();
    // включаем аккорд
    MaybeGenerateAccord(1);
    // сбрасываем датчик времени на текущий момент
    chain_time.reset();
    play_accords_chain = 2;
  }
  else
  if (play_accords_chain == 2) // продолжение генерации
  {
    // ожидание конца аккорда и переход к следующему
    if (chain_time.dt() >= acc_dtime)
    {
      // откл. аккорда
      MaybeGenerateAccord(3);
      // переход к следующему аккорду, если был не последний
      if (++accord_act_index < accords_in_chain)
      {
        SetupAccord(accord_act_index);
        acc_dtime = AccordDtime();
        MaybeGenerateAccord(1); // вкл. аккорда
        chain_time.reset();
      }
      else // кончился последний аккорд в секвенции
      {
        // отрабатываем зацикливание секвенции
        if ( checks[Loop_Chain].checked_state() ) play_accords_chain = 0;
        RewindAccords(); // перематываем в начало
        // при play_accords_chain = 2 останавливаем проигрывание
        // при play_accords_chain = 0 перезапускаем проигрывание
        Play_StopAccords();
      }
    }
  }
}

void MusicDialogBoxCtrl::MaybeGenerateAccord(int new_state)
{
  if (new_state == 1 && gen_state == 0) // нормальное включение
  {
    gen_state = 1;
    GenerateAccord(); // включаем аккорд
  }
  else
  if (new_state == 3 && gen_state == 2) // нормальное выключение
  {
    gen_state = 3;
    GenerateAccord(); // выключаем аккорд
  }
}

void MusicDialogBoxCtrl::RegenAccord()
{
  if (gen_state == 2)
  {
    MaybeGenerateAccord(3); // откл. аккорда
    MaybeGenerateAccord(1); // вкл. аккорда
  }
}

void MusicDialogBoxCtrl::RewindAccords()
{
  // если шло проигрывание - его надо остановить и начать заново
  int play = play_accords_chain;
  if (play == 2) Play_StopAccords();

  accord_act_index = 0;
  SetupAccord(accord_act_index);

  if (play == 2) Play_StopAccords();
}

void MusicDialogBoxCtrl::Play_StopAccords()
// включаем проигрывание секвенции если оно остановлено или останавливаем если включено;
{
  // проверяем флаг проигрывания секвенции: 0=стоп, 1=старт, 2=продолжение проигрывания
  if (play_accords_chain == 0)
    play_accords_chain = 1;
  else // play_accords_chain = 1 или 2
    play_accords_chain = 0;

  buttons[Play_Stop].button_text(play_accords_chain==0? 0:1);

  // идентификатор таймера важен только если у одного диалог бокса есть более одного таймера!
  int id = 1;
  if (play_accords_chain == 1)
  {
    // диалог бокс определяется по hwnd()
    SetTimer( hwnd(), id, timer_msec, 0);
  }
  else
  if (play_accords_chain == 0)
  {
    KillTimer( hwnd(), id );
    // откл. аккорда
    MaybeGenerateAccord(3);
  }
}

void MusicDialogBoxCtrl::GenerateAccord()
{
  if (gen_state == 1) // включаем аккорд
  {
    // считываем все контролы, формируем аккорд (до манипуляторов)
    DichoticAccord src_acc = GetAccord();

    // пропускаем исходный аккорд через общие манипуляторы, получаем текущий аккорд
    gen_acc = DichoticAccord::accord_manipulator( src_acc );

    // включаем ноты аккорда
    for (int i = 0; i < gen_acc.voices_number; ++i)
    {
      if ( !gen_acc.dn[i].pause )
      {
        // определяем абсолютную миди ноту голоса
        int midi_note0 = sliders[Transposition].actpos;
        int midi_note = midi_note0 + gen_acc.dn[i].note;

        // определяем конфликт зашкаливания ноты и если оного нету - включаем голос
        if ( in_range(MidiInterface::MIDI_NOTE_MIN, midi_note, MidiInterface::MIDI_NOTE_MAX) )
        {
          // нет зашкаливания midi_note
          int volume = sliders[Accord_Volume].actpos;
          double kvol[3] = { 1., 0.4, 0.1 }; // громкость 100%,10%,1%.
          int mute = checks[Mute_Sound].return_state(); //    0,  1,  2 = BST_UNCHECKED, BST_CHECKED, BST_INDETERMINATE
          volume = float2int( kvol[mute]*volume );
          double pan = sliders[Stereo_Base].get_reg_value() * gen_acc.dn[i].pan;
          if ( get_check( No_Sound ).unchecked_state() ) // проверяем чекбокс No_Sound
            NewNoteOn(i, midi_note, pan, volume); // включаем ноту голоса i
          gen_acc.dn[i].midi_note = midi_note; // запоминаем midi_note
        }
        else // зашкаливание номера ноты: делаем паузу голоса!
        {
          gen_acc.dn[i].pause = 1;
        }
      }
    }

    NotesSetText(); // выводим список нот аккорда по каналам звука и др. параметры

    gen_state = 2;
  }

  if (gen_state == 3) // выключаем аккорд
  {
    // выключаем все включённые ранее ноты аккорда:
    for (int i = 0; i < gen_acc.voices_number; ++i)
    {
      if ( !gen_acc.dn[i].pause )
      {
        NewNoteOff(i, gen_acc.dn[i].midi_note);
      }
    }
    gen_state = 0;
  }
}

void MusicDialogBoxCtrl::SetupAccord(int index)
// копируем аккорд из массива с индексом index в аккорд на диалогпанели
{
  // номер аккорда в индикторе на 1 больше индекса
  sliders[Accord_Number].setpos(index+1);

  DichoticAccord acc = accords_chain[index];

  if ( checks[Dont_Change_GM_Instr].unchecked_state() &&
       combos[GM_Instrument].GetStringIndex() != acc.instrument &&
       acc.voices_number > 0 ) // при паузе номер инструмента не меняется!
  {
    SetInstrument(acc.instrument);
  }

  sliders[Accord_Temp].setpos( acc.temp );
  sliders[Accord_Duration].setpos( acc.duration );
  sliders[Accord_Volume].setpos( acc.volume );

  for (int n = 0; n < DichoticAccord::MAX_ACC_VOICES; ++n)
  {
    dvoices[n].set_switchon(false);
    if ( n < acc.voices_number && !acc.dn[n].pause )
    {
      dvoices[n].switchon_note( acc.dn[n].note, acc.dn[n].pan );
    }
  }
}

void MusicDialogBoxCtrl::AllVoicesSwitch()
// кнопка сброса включателей всех голосов в откл., затем циклически включение-выключение первых 6 голосов
{
  static int state = 0; // 0: все в 0; 1: первые 6 в 1;

  // здесь нужно отключать все голоса, а не только регулируемые на диалогпанели: при проигрывании файлов
  // "невидимые" голоса могут остаться включенными и будут звучать и дальше при создании аккорда вручную!
  for (int i = 0; i < DichoticAccord::MAX_ACC_VOICES; ++i)
  {
    bool switchon = false;
    // имеем ввиду, что dialog_voices в общем случае может быть меньше 6 !
    if (state == 1 && i < 6 && i < dialog_voices) switchon = true;
    dvoices[i].set_switchon(switchon);
  }

  state = state==0? 1:0;
}

bool MusicDialogBoxCtrl::AllPanSwitch()
// кнопка циклического переключения КНОПОК всех панорам лево-центр-право
// возвращает true если хотя бы один голос был включен (звучал)
{
  static int state = 0; // 0,1,2
  int pan[3] = {-1,0,1};
  bool switchon = false;

  for (int i = 0; i < dialog_voices; ++i)
  {
    dvoices[i].set_pan(pan[state]);
    switchon |= dvoices[i].get_switchon();
  }

  if (++state > 2) state = 0;
  return switchon;
}

bool MusicDialogBoxCtrl::VoicesGroup(int first_voice, int voices_num, int operation)
// в группе голосов делаем одинаковую (РУЧНУЮ!) операцию: -1, +1 сдвиг высоты; 0 сброс высоты в 0
// возвращает true если хотя бы один голос группы был включен (звучал)
{
  int last_voice = first_voice + voices_num - 1;
  test_max(last_voice, dialog_voices-1); // здесь dialog_voices - т.к. это ручная операция

  bool switchon = false;

  for (int i = first_voice; i <= last_voice; ++i)
  {
    int note = dvoices[i].get_note();
    if (operation == 0) note = 0;
    else note = (operation < 0)? note-1:note+1;
    dvoices[i].set_note(note);
    switchon |= dvoices[i].get_switchon();
  }

  return switchon;
}

// включаем генерацию голоса voice_index с нужной midi_note, панорамой и громкостью
inline void MusicDialogBoxCtrl::NewNoteOn(int voice_index, int midi_note, double pan, int volume)
{
  // панорамируем канал
  int midi_chan = dvoices[voice_index].get_midi_channel();
  MIDII.ChanPan(midi_chan, pan);
  // включаем ноту
  MIDII.MidiNoteOn(midi_chan, midi_note, volume);
}

inline void MusicDialogBoxCtrl::NewNoteOff(int voice_index, int midi_note) // отключаем генерацию голоса
{
  int midi_chan = dvoices[voice_index].get_midi_channel();
  // выключаем ноту c 0-й громкостью
  MIDII.MidiNoteOff(midi_chan, midi_note, 0);
}

template<int number> void MusicDialogBoxCtrl::InitDichoticVoices(const int (&id_array)[number][3], int max_number)
// инициал-м голоса dvoices из массива идентифик-в их контролов [number][Note_Voice_id,Pan_Voice_id,Switchon_Voice_id]
// при max_number == 0 используется весь массив, иначе используется не более max_number его членов
{
  // определяем кол-во видимых контролов
  dialog_voices = number;
  if (max_number > 0) dialog_voices = min(number, max_number);

#ifdef DDD
  Mbox(dialog_voices);
#endif

  for (int i = 0; i < dialog_voices; ++i)
  {
    // каждый голос инициализируем в отдельности
    dvoices[i].init(hwnd(), id_array[i][0], id_array[i][1], id_array[i][2], i, dialog_voices);
    //                      Note_Voice_id   Pan_Voice_id    Switchon_Voice_id
  }
}

void MusicDialogBoxCtrl::SetInstrument(int instrument_number)
{
  for (int i=0; i < DNUMS; ++i)
  {
    dbox[i].combos[GM_Instrument].SetStringIndex(instrument_number);
  }
  MIDII.AllChanPatchChange(instrument_number); // сменим инструмент во всех midi каналах
}

void MusicDialogBoxCtrl::InitInstruments(int instrument_number)
{
  for (int i = 0; i < MidiInterface::GM_INSTRUMENTS_NUMBER; ++i)
  {
    combos[GM_Instrument].AddString( MidiInterface::PatchNames[i] );
  }
  combos[GM_Instrument].SetStringIndex(instrument_number);
  MIDII.AllChanPatchChange(instrument_number); // сменим инструмент во всех midi каналах
}

void MusicDialogBoxCtrl::InitMidiDevices(int device_number)
{
  // заполняем список midi девайсов
  int dnums = midiOutGetNumDevs();
  for (int i = 0; i < dnums; i++)
  {
    MIDIOUTCAPS DevCaps;
    midiOutGetDevCaps(i, &DevCaps, sizeof (DevCaps));
    combos[MidiOutDevice].AddString( DevCaps.szPname );
  }

  // первоначальное открытие midi девайса
  bool res = MIDII.OpenDeviceOut(device_number);
  if ( !res ) Mbox(L"Error in MIDII.OpenDeviceOut(), device_number=", device_number);
  combos[MidiOutDevice].SetStringIndex( device_number ); // показываем имя девайса
}

void CALLBACK MusicDialogBoxCtrl::InitDialog(int dnum)
// отработка WM_INITDIALOG для диалог бокса номер dnum = 0...(DNUMS-1)
{
  if (dnum == 0)
  {
    SetTitle(); // титул основного диалог бокса 0

    // инициализируем все контролы диалог бокса 0 из массивов структур
    InitControls(check[dnum]);
    InitControls(button[dnum]);
    InitControls(slider[dnum]);
    InitControls(combo[dnum]);
    InitControls(edit[dnum]);
    InitControls(textctrl[dnum]);

    // заполняем список midi девайсов, открываем девайс
    InitMidiDevices(midi_device_number);
    // заполняем список midi инструментов, установим нужный инструмент во всех каналах
    InitInstruments(midi_instrument_number);

    // загреиваем это пока не загружен аккорд или секвенция
    sliders[Accord_Number].enable(0);
    sliders[Chain_Speed].enable(0);
    checks[Loop_Chain].enable(0);
    checks[Dont_Change_GM_Instr].enable(0);
    checks[Converter_Auto].enable(0);

    ResetMaxDuration(); // устанавливаем предел слайдера Duration

    // сетап голосов - в самом конце!
    // InitDichoticVoices( control_id_array ); // число видимых голосов = числу контролов = 15
    InitDichoticVoices( control_id_array, DIALOG_VOICES[0] ); // в боксе 0 видимы и работают только 12 голосов

    // включаем три первых голоса и делаем 2H аккорд до-мажор
    dvoices[0].switchon_note(0,-1);
    dvoices[1].switchon_note(4);
    dvoices[2].switchon_note(7, 1);
  }
  else
  if (dnum >= 1) // диалог боксы 1, 2 ...
  {
    wstring2 ws;
    ws << L"Dialog Box " << dnum;
    SetTitle(ws); // титул дополнительных диалог боксов

    // инициализируем все контролы диалог бокса 1 из массивов структур
    InitControls(check[dnum]);
    InitControls(button[dnum]);
    InitControls(slider[dnum]);
    InitControls(combo[dnum]);
    InitControls(edit[dnum]);
    InitControls(textctrl[dnum]);

    // заполняем список midi инструментов, установим нужный инструмент во всех каналах
    InitInstruments(midi_instrument_number);

    // загреиваем это пока не загружен аккорд или секвенция
    sliders[Accord_Number].enable(0);
    sliders[Chain_Speed].enable(0);
    checks[Loop_Chain].enable(0);
    checks[Dont_Change_GM_Instr].enable(0);

    // в диалог боксе 1 некоторые контролы используются "в тёмную"
    ResetMaxDuration();

    // сетап голосов - в самом конце!
    InitDichoticVoices( control_id_array, DIALOG_VOICES[1] ); // в боксе >=1 видимы и работают только 6 голосов
/*
    // включаем три первых голоса и делаем аккорд до-минор
    dvoices[0].switchon_note(0);
    dvoices[1].switchon_note(3);
    dvoices[2].switchon_note(7);
*/
  }
}

int CALLBACK MusicDialogBoxCtrl::DialogProc0(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
// обработчик сообщений диалог бокса 0
{
  act_dialog_index = 0; // определяем индекс активного диалогбокса!

  if (message == WM_INITDIALOG)
  {
    // этот мессаг приходит ДО возврата из CreateDialog(), поэтому хендл диалог бокса берём из hwnd !!
    actbox().hdlg = hwnd;
    actbox().InitDialog(act_dialog_index); // процедура инициализации активного диалогбокса!
    // return TRUE to set the keyboard focus to the control specified by wParam, otherwise return FALSE
    return TRUE; // ?
  }

  return CommonDialogProc(message, wparam, lparam); // общая часть всех DialogProc
}

int CALLBACK MusicDialogBoxCtrl::DialogProc1(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
// обработчик сообщений диалог бокса 1
{
  act_dialog_index = 1;

  if (message == WM_INITDIALOG)
  {
    actbox().hdlg = hwnd;
    actbox().InitDialog(act_dialog_index);
    return TRUE;
  }

  return CommonDialogProc(message, wparam, lparam);
}

int CALLBACK MusicDialogBoxCtrl::DialogProc2(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
// обработчик сообщений диалог бокса 2
{
  act_dialog_index = 2;

  if (message == WM_INITDIALOG)
  {
    actbox().hdlg = hwnd;
    actbox().InitDialog(act_dialog_index);
    return TRUE;
  }

  return CommonDialogProc(message, wparam, lparam);
}

int MusicDialogBoxCtrl::CommonDialogProc(UINT message, WPARAM wparam, LPARAM lparam)
// общая часть всех DialogProc
{
  bool regen_accord = false;
  int index = 0;

  // сообщения клавиатуры здесь не появляются, они попадают в IsDialogMessage() и транслируются
  // в специальные диалоговые сообщения типа изменения фокуса ввода по клавише Tab
  switch (message)
  {
    case WM_TIMER: actbox().TimerMessage(); EatMessages(actbox().hwnd(), message); break;

    case WM_HSCROLL: case WM_VSCROLL: // сообщения от любых регулируемых слайдеров
      {
        // если любой регулятор звучащего голоса передвинулся - перенажимаем аккорд
        int regen = 0, pos;

        // читаем позиции всех регуляторов слайдеров (т.е. видимых) голосов
        for (int i = 0; i < actbox().dialog_voices; ++i)
        {
          int pan = actbox().dvoices[i].get_pan();
          if ( pan != actbox().dvoices[i].get_pan_reg() &&
               actbox().dvoices[i].get_switchon() ) regen = 1;

          int note = actbox().dvoices[i].get_note();
          // т.к. при изменении включённой кнопкой ноты может измениться конфликт - перенажимаем аккорд!
          if ( note != actbox().dvoices[i].get_note_reg() &&
               actbox().dvoices[i].get_switchon() ) regen = 1;
        }

        pos = actbox().sliders[Chain_Speed].actpos;
        if (pos != actbox().sliders[Chain_Speed].getpos()) regen = 1;

        pos = actbox().sliders[Accord_Volume].actpos;
        if (pos != actbox().sliders[Accord_Volume].getpos()) regen = 1;

        pos = actbox().sliders[Transposition].actpos;
        if (pos != actbox().sliders[Transposition].getpos()) regen = 1; // тут апдейтится Transposition.actpos!

        pos = actbox().sliders[Stereo_Base].actpos;
        if (pos != actbox().sliders[Stereo_Base].getpos()) regen = 1;

        pos = actbox().sliders[Notes_Shift].actpos;
        if (pos != actbox().sliders[Notes_Shift].getpos()) regen = 1;

        // если номер аккорда изменился - обновляем аккорд
        // при автопроигрывании секвенции длительность первого нового аккорда будет неправильной!
        pos = actbox().sliders[Accord_Number].actpos;
        int convert = 0;
        if (pos != actbox().sliders[Accord_Number].getpos())
        {
          actbox().accord_act_index = actbox().sliders[Accord_Number].actpos-1; // индекс аккорда на 1 меньше позиции слайдера
          actbox().SetupAccord( actbox().accord_act_index );
          regen = 1;
          convert = 1; // возможно делаем 2H конвертацию
        }

        actbox().sliders[Accord_Duration].getpos();
        actbox().sliders[Accord_Temp].getpos();

        // сначала перенажимаем аккорд в actbox(), иначе после ConverterMode() этого почему-то не происходит
        if (regen) actbox().RegenAccord();
        // возможно делаем 2H конвертацию, затем перенажимаем аккорд в dbox[1]
        if (convert) dbox[0].ConverterModeMain();
      }
      break;

    case WM_COMMAND: // командные сообщения Windows
      {
      UINT control_id = LOWORD(wparam);
      UINT control_msg = HIWORD(wparam);
      switch (control_id)
      {
        case IDC_BUTTON_HELP: ShellExecuteOpen( AddModulePath(L"help.txt").c_str() ); break; // показать документацию

        case IDC_BUTTON_WWW: ShellExecuteOpen(website); break; // выход на WEB SITE

        case IDC_BUTTON_CLOSE: case IDCANCEL: // кнопки закрытия окна бокса 0
          if (act_dialog_index == 0)
          {
            PostMessage(actbox().hwnd(), WM_QUIT, 0, 0);
            return TRUE; // ?
          }
          // else в боксах 1, 2 делаем их невидимыми
          dbox[act_dialog_index].Show(false);
          break;

        case IDC_Del_Identic3and3: actbox().DeleteIdenticAccords6(); break; // кнопка "Del Identic (3+3)"

        case IDC_Del_Identic2and2: actbox().DeleteIdenticAccords4(); break; // кнопка "Del Identic (2+2)"

        case IDC_Add_Inverse_Pan_Accords: actbox().AddInversePanAccords(); break; // кнопка "Add Inverse Pan"

        case IDC_Sort_Similar_Accords: // кнопка "Sort Similar Accords"
          if ( actbox().checks[Sort_Mode].checked_state() ) actbox().SortSimilarAccordsMono();
          else                                              actbox().SortSimilarAccords3and3();
          break;

        // кнопки файловых операций и проигрывания секвенции:
        case IDC_Save_Accord:        actbox().SaveMusic(0); break;
        case IDC_Append_Accord:      actbox().SaveMusic(1); break;
        case IDC_Save_Accords_Chain: actbox().SaveMusic(2); break;
        case IDC_Load_Accords:
          if ( actbox().LoadAccords(0) ) // если файл секвенции загрузился
          {
            // сначала перенажимаем аккорд в actbox()
            actbox().RegenAccord();
            // возможно делаем 2H конвертацию, затем перенажимаем аккорд в dbox[1]
            dbox[0].ConverterModeMain(true);
          }
          break;

        case IDC_Play_Stop: actbox().Play_StopAccords(); break;
        case IDC_Rewind:    actbox().RewindAccords(); break;

        case IDC_All_Voices_Switch: actbox().AllVoicesSwitch(); actbox().RegenAccord(); break; // кнопка включателей всех голосов
        case IDC_All_Pan_Switch: if ( actbox().AllPanSwitch() ) actbox().RegenAccord(); break; // кнопка для всех панорам

        // кнопки синхронного сдвига +-1 и сброса в 0 высот троек голосов 1-3, 4-6, 7-9, 10-12, 13-15
        case IDC_V1V15_0: if ( actbox().VoicesGroup(0, 15, 0) ) actbox().RegenAccord(); break;

        case IDC_V1V3_INC: if ( actbox().VoicesGroup(0, 3, +1) ) actbox().RegenAccord(); break;
        case IDC_V1V3_0:   if ( actbox().VoicesGroup(0, 3,  0) ) actbox().RegenAccord(); break;
        case IDC_V1V3_DEC: if ( actbox().VoicesGroup(0, 3, -1) ) actbox().RegenAccord(); break;

        case IDC_V4V6_INC: if ( actbox().VoicesGroup(3, 3, +1) ) actbox().RegenAccord(); break;
        case IDC_V4V6_0:   if ( actbox().VoicesGroup(3, 3,  0) ) actbox().RegenAccord(); break;
        case IDC_V4V6_DEC: if ( actbox().VoicesGroup(3, 3, -1) ) actbox().RegenAccord(); break;

        case IDC_V7V9_INC: if ( actbox().VoicesGroup(6, 3, +1) ) actbox().RegenAccord(); break;
        case IDC_V7V9_0:   if ( actbox().VoicesGroup(6, 3,  0) ) actbox().RegenAccord(); break;
        case IDC_V7V9_DEC: if ( actbox().VoicesGroup(6, 3, -1) ) actbox().RegenAccord(); break;

        case IDC_V10V12_INC: if ( actbox().VoicesGroup(9, 3, +1) ) actbox().RegenAccord(); break;
        case IDC_V10V12_0:   if ( actbox().VoicesGroup(9, 3,  0) ) actbox().RegenAccord(); break;
        case IDC_V10V12_DEC: if ( actbox().VoicesGroup(9, 3, -1) ) actbox().RegenAccord(); break;

        case IDC_MidiOutDevice: // действие со списком устройств
          if (control_msg == CBN_SELENDOK) // завершен выбор из списка устройств (см также CBN_SELCHANGE)
          {
            // определяем номер выбранного устройства в списке
            int device_number = actbox().rw_combo(MidiOutDevice).GetStringIndex();
            MIDII.CloseDevicesOut(); // закроем текущее устройство
            if ( MIDII.OpenDeviceOut(device_number) ) // откроем новое устройство
            {
              // установим нужный номер инструмента, отобразим его и перенажмём аккорд на всех диалог боксах
              int instrument_number = actbox().rw_combo(GM_Instrument).GetStringIndex(); // это не менялось
              SetInstrument(instrument_number);
              for (int i=0; i < DNUMS; ++i) dbox[i].RegenAccord();
            }
            else Mbox(L"Error in MIDII.OpenDeviceOut(), device_number=", device_number);
          }
          break;

        case IDC_GM_Instrument: // действие со списком инструментов
          if (control_msg == CBN_SELCHANGE) // выбран новый инструмент в списке инструментов (см также CBN_SELENDOK)
          {
            // установим нужный номер инструмента, отобразим его и перенажмём аккорд на всех диалог боксах
            int instrument_number = actbox().rw_combo(GM_Instrument).GetStringIndex();
            SetInstrument(instrument_number);
            for (int i=0; i < DNUMS; ++i) dbox[i].RegenAccord();
          }
          break;

        case IDC_CHECK_Generate_Accord: // дублируется хоткеями (Insert, Control)
          {
            actbox().checks[Generate_Accord].getstate();
            int gen_state = actbox().checks[Generate_Accord].checked_state()? 1:3;
            actbox().MaybeGenerateAccord(gen_state); // возможное вкл. или откл. аккорда
          }
          break;

        case IDC_CHECK_Edit_Comment:
          actbox().checks[Edit_Comment].getstate();
          actbox().edits[Accords_Chain_Comment].readonly( actbox().checks[Edit_Comment].unchecked_state() );
          break;

        case IDC_CHECK_Big_Duration: actbox().ResetMaxDuration(); break;

        // действия с чекбоксами включения голосов
        case IDC_Switchon_Voice_1:  case IDC_Switchon_Voice_2:  case IDC_Switchon_Voice_3:
        case IDC_Switchon_Voice_4:  case IDC_Switchon_Voice_5:  case IDC_Switchon_Voice_6:
        case IDC_Switchon_Voice_7:  case IDC_Switchon_Voice_8:  case IDC_Switchon_Voice_9:
        case IDC_Switchon_Voice_10: case IDC_Switchon_Voice_11: case IDC_Switchon_Voice_12:
          index = actbox().voice_index(control_id); // получаем индекс голоса по Switchon_Voice.id
          if (index >= 0) // ok
          {
            actbox().dvoices[index].get_switchon_reg(); // апдейтим изменения
            actbox().RegenAccord(); // всегда перенажимаем аккорд, т.к. какой-то голос переключился
          }
          // else index < 0 значит что такого контрола нет!
          break;

        // действия с остальными чекбоксами
        case IDC_CHECK_One_Octave:
        case IDC_CHECK_Swap_Output:
        case IDC_CHECK_With_Unisons:
        case IDC_CHECK_Dichotic_Output:
        case IDC_CHECK_Switchon_Left_Pan:
        case IDC_CHECK_Switchon_Mid_Pan:
        case IDC_CHECK_Switchon_Right_Pan:
        case IDC_CHECK_Zero_Ground_Notes:
        case IDC_CHECK_Show_Sorted_Notes:
        case IDC_CHECK_Mute_Sound:
        case IDC_CHECK_No_Sound:
          regen_accord = true; // для чекбоксов выше этой строки надо перенажимать аккорд, а ниже - не надо
        case IDC_CHECK_Dont_Save_Empty_Voices:
        case IDC_CHECK_Edit_Sequence:
        case IDC_CHECK_Save_With_Manipuls:
        case IDC_CHECK_Sort_Mode:
        case IDC_CHECK_Loop_Chain:
        case IDC_CHECK_Dont_Change_GM_Instr:
        case IDC_CHECK_Converter_Mode:
        case IDC_CHECK_Converter_Auto:
        case IDC_CHECK_Converter_Mirror_Accords:
        case IDC_CHECK_Converter_With_Unisons:
          int index = actbox().checks_index(control_id);
          if (index >= 0)
          {
            // запоминаем оригинальный номер бокса, т.к. он меняется при обращении к другим боксам
            int a_d_i = act_dialog_index;

            int state = actbox().checks[index].getstate();
            bool checked = (state == BST_CHECKED);

            if (index != Mute_Sound) { if (regen_accord) actbox().RegenAccord(); }
            else // Mute_Sound
            {
              // этот контрол меняет громкость и перенажимает аккорд на всех диалог боксах!
              for (int i=0; i < DNUMS; ++i)
              {
                dbox[i].checks[index].setstate(state);
                dbox[i].RegenAccord();
              }
              act_dialog_index = a_d_i; // восстанавливаем оригинальный номер бокса!
            }

            bool convert = 0, Seq = 0;
            if (index == Converter_Auto) // этот чекбокс есть только на диалог боксе 0!
            {
              convert = 1;
              if (checked) Seq = 1;
              // показываем или скрываем бокс 2, тут меняется actbox() !!
              dbox[2].Show(checked);
              act_dialog_index = a_d_i; // восстанавливаем оригинальный номер бокса!
            }

            if (index == Converter_Mode) // этот чекбокс есть только на диалог боксе 0!
            {
              convert = 1;
              // меняем состояние зависимых контролов
              dbox[0].checks[Converter_Auto].enable(checked);
              // показываем или скрываем дополнительные диалогбоксы, тут меняется actbox() !!
              if (checked == false) // отключаем боксы
              {
                dbox[1].Show(0);
                dbox[2].Show(0);
              }
              else // true, включаем боксы
              {
                dbox[1].Show(1);
                // включаем бокс 2 только при включённой автоконвертации
                if ( dbox[0].checks[Converter_Auto].checked_state() )
                {
                  Seq = 1;
                  dbox[2].Show(1);
                }
              }
              act_dialog_index = a_d_i; // восстанавливаем оригинальный номер бокса!
            }

            if (index == Converter_Mirror_Accords || index == Converter_With_Unisons)
            {
              convert = 1;
              // только Converter_With_Unisons может изменить значение лучшего 2H аккорда,
              // поэтому делаем новое преобразование всей секвенции только в этом случае!
              Seq = (index == Converter_With_Unisons);
            }

            // при включении Edit_Sequence тоже делаем конвертацию
            if ( index == Edit_Sequence && dbox[0].checks[Converter_Mode].checked_state() ) convert = checked;

            // возможно делаем 2H конвертацию, затем перенажимаем аккорд в dbox[1]
            if (convert) dbox[0].ConverterModeMain(Seq);
          }
          break;
      }
      break;
    }
  }
  return FALSE;
}

void MusicDialogBoxCtrl::NotesSetText()
// выводим на диалогбоксе номера нот текущего аккорда gen_acc в соответствующих им позициях панорамы,
// также выводим суммарный диссонанс аккорда и др.
{
  int notes1[DichoticAccord::MAX_ACC_VOICES],
      notes2[DichoticAccord::MAX_ACC_VOICES],
      notes3[DichoticAccord::MAX_ACC_VOICES];
  int num1=0, num2=0, num3=0;
  wstring2 s;
  int i;

  // анализируем пространственные позиции голосов и записываем ноты в 3 соотв. массива
  int add = 0;
  if ( checks[Zero_Ground_Notes].checked_state() )
  {
    // нужен показ нот с 0-й минимальной нотой вместо оригинального
    pair<int,int> index_note = DichoticAccord::accord_notes_min( gen_acc );
    if ( index_note.first >= 0 ) add = -index_note.second;
  }
  // в индикаторах к нотам добавляем add = -(номер минимальной ноты аккорда)
  DichoticAccord::NotesGetPos(gen_acc, notes1, num1, notes2, num2, notes3, num3, add);

  // стираем s, выводим список относит-х номеров нот голосов с левой позицией панорамы
  s.clear();
  for (i = 0; i < num1; ++i) s << notes1[i] << L", ";
  textctrls[Left_Notes].add_text( s );

  // то же самое для центра панорамы
  s.clear();
  for (i = 0; i < num2; ++i) s << notes2[i] << L", ";
  textctrls[Mid_Notes].add_text( s );

  // то же для правой позиции панорамы
  s.clear();
  for (i = 0; i < num3; ++i) s << notes3[i] << L", ";
  textctrls[Right_Notes].add_text( s );

  // определяем и выводим суммарный диссонанс аккорда с учётом панорамы и конфликтов

  // определяем конфликты определения диссонанса, аккорд не меняем!
  bool conflict_diss = DichoticAccord::test_conflicts(gen_acc, false);

  int diss = gen_acc.dissonance();

  s.clear();
  s << diss;
  if (conflict_diss) s += L" ?";
  textctrls[Total_Diss].add_text( s );

  // выводим число уникальных нот в аккорде, игнорируя панораму
  int unum = DichoticAccord::unique_notes_num(gen_acc);
  s.clear();
  s << unum;
  textctrls[Uniq_Notes].add_text( s );
}

bool MusicDialogBoxCtrl::LoadAccords(int format, const wchar_t *file)
// загружаем секвенцию из файла, тип которого зависит от format:
// 0: родной .daccords формат, 1: .PAS формат файлов редактора MUSE
// если есть строка file, то формат 0 грузим молча из этого файла!
{
  // уничтожаем старый массив аккордов, сбрасываем параметры и загреиваем регулятор позиции аккорда и кнопки
  accords_in_chain = 0;
  sliders[Accord_Number].setpos(1);
  sliders[Accord_Number].reset_minmax(1,1);
  sliders[Accord_Number].enable(0);

  sliders[Chain_Speed].setpos(10);
  sliders[Chain_Speed].enable(0);

  checks[Loop_Chain].enable(0);
  checks[Dont_Change_GM_Instr].enable(0);

  buttons[Save_Accords_Chain].enable(0);
  buttons[Play_Stop].enable(0);
  buttons[Rewind].enable(0);

  int num_accords = MAX_ACCORDS;

  MusicFile mfile( hwnd() );
  bool res;

  if (format == 0) res = mfile.Open(cheader, accords_chain, num_accords, file);
  else //    == 1 .PAS формат
  {
    // в качестве дихотического сдвига берём значение регулятора относит-й ноты последнего видимого голоса
    int dtrans = dvoices[dialog_voices-1].get_note();
    res = mfile.OpenPAS(dtrans, cheader, accords_chain, num_accords);
  }
  if (!res) return false;

  SetupAccords(num_accords); // подготовка контролов для новой секвенции в accords_chain и cheader

  return true;
}

void MusicDialogBoxCtrl::SetupAccords(int num_accords)
{
  // устанавливаем параметры и отгреиваем регулятор позиции аккорда и кнопки
  accords_in_chain = num_accords;
  sliders[Accord_Number].reset_minmax(1, accords_in_chain);
  sliders[Accord_Number].enable(1);

  sliders[Chain_Speed].enable(1);
  checks[Loop_Chain].enable(1);
  checks[Dont_Change_GM_Instr].enable(1);

  buttons[Save_Accords_Chain].enable(1);
  buttons[Play_Stop].enable(1);
  buttons[Rewind].enable(1);

  // копируем общие параметры секвенции из хедера куда надо
  sliders[Transposition].setpos( cheader.transposition );
  sliders[Chain_Speed].setpos( sliders[Chain_Speed].get_pos_value(cheader.chain_speed) );
  checks[Dont_Change_GM_Instr].set_checked_state( cheader.dont_change_gm_instrument != 0 );

  // однократно устанавливаем инструмент если он не будет извлекаться из каждого аккорда
  if ( checks[Dont_Change_GM_Instr].checked_state() ) SetInstrument(cheader.instrument_number);

  // готовим проигрывание первого аккорда секвенции
  RewindAccords();
}

bool MusicDialogBoxCtrl::ConverterModeExecute(DichoticAccord acc1h)
// return false если невозможна 2H конвертация аккорда acc1h
{
  acc1h.remove_pauses(); // уничтожаем голоса-паузы
  acc1h.zero_pans(); // обнуляем все панорамы (они будут по центру)

  // если не выбран чекбокс With_Unisons из группы "манипуляторов" текущего аккорда бокса 0 
  // то удаляем все лишние голоса-унисоны (с любой панорамой) и голоса-паузы, сжимая аккорд
  if ( dbox[0].get_check(With_Unisons).unchecked_state() ) acc1h.remove_unisons();

  if ( acc1h.voices_number > MAX_CONVERTER_VOICES ) return false;

  // готовим все 2H варианты текущего 1H аккорда (его панорамы будут проигнорированы)
  vector<DichoticAccord> acc2h; // массив вариантов 2H аккордов
  // полное количество вариантов аккордов >= 0
  int vars_num = DichoticAccord::DichoticConverter(acc1h, acc2h);

  // ранжирование массива аккордов acc2h согласно их диссонансам
  for (int n = 0; n < vars_num; ++n)
    acc2h[n].spare1 = acc2h[n].dissonance(); // запоминаем диссонанс каждого аккорда в spare1
  sort(acc2h.begin(), acc2h.end(), DichoticAccord::Less_spare1); // сортируем все аккорды по spare1

  bool need_compress = false; // true = необходимость сжатия массива acc2h

  // если не выбран чекбокс Converter_Mirror_Accords, то все зеркальные аккорды (отличающиеся только
  // инверсией панорам голосов) будут удалены из массива вариантов 2H аккордов
  if ( dbox[0].get_check(Converter_Mirror_Accords).unchecked_state() )
  {
    // используем факт сортировки - у зеркальных аккордов одинаковый диссонанс!
    for (int i = 0; i < vars_num; ++i)
    {
      DichoticAccord &ai = acc2h[i];
      if (ai.voices_number == 0) continue; // аккорд уже стёрт - берём следующий аккорд i
    	int diss = ai.spare1;
    	for (int j = i+1; j < vars_num; ++j)
    	{
        DichoticAccord &aj = acc2h[j];
        if (aj.voices_number == 0) continue; // аккорд уже стёрт - берём следующий аккорд j
    		if (diss != aj.spare1) break; // другой диссонанс - берём следующий аккорд i
    		// else диссонансы совпадают, проверяем зеркальность
    		if ( DichoticAccord::mirror_accords(ai, aj) )
        {
    		  aj.voices_number = 0; // делаем из аккорда паузу
          need_compress = true;
        }
    	}
    	// не пропускаем аккорды по i, т.к. стёртые по j аккорды могут быть не рядом с i!
    }
  }

  // если в исходном аккорде были унисоны, то нужна дальнейшая обработка
  if ( acc1h.test_unisons() )
  {
    // удаляем все полностью одинаковые аккорды
    // однообразная сортировка голосов всех аккордов
    for (int i = 0; i < vars_num; ++i)
    {
      int vnum = acc2h[i].voices_number;
      if (vnum == 0) continue;
      // копируем ноты аккорда в вектор: 2-й аргумент конструктора должен быть за концом массива!!
      vector<DichoticNote> dn( &acc2h[i].dn[0], &acc2h[i].dn[vnum] ); // [vnum] = (vnum-1)+1!
      // сортируем голоса аккорда в порядке возрастания номера нот с учётом панорамы
      for (int n = 0; n < vnum; ++n)
        dn[n].spare1 = 2*dn[n].note + dn[n].pan;
      sort(dn.begin(), dn.end(), DichoticNote::Less_spare1); // сортируем голоса по spare1
      // записываем отсортированные голоса обратно в аккорд
      for (int n = 0; n < vnum; ++n) acc2h[i].dn[n] = dn[n];
    }
    // удаление одинаковых аккордов
    for (int i = 0; i < vars_num; ++i)
    {
      DichoticAccord &ai = acc2h[i];
      if (ai.voices_number == 0) continue; // аккорд уже стёрт - берём следующий аккорд i
    	for (int j = i+1; j < vars_num; ++j)
    	{
        DichoticAccord &aj = acc2h[j];
        if (aj.voices_number == 0) continue; // аккорд уже стёрт - берём следующий аккорд j
        if ( ai.identic_voices(aj) )
        {
          // полное совпадение всех голосов аккордов ai и aj
    		  aj.voices_number = 0; // делаем из аккорда паузу
          need_compress = true;
        }
    	}
    }

    // если не выбран чекбокс Converter_With_Unisons, то все аккорды с унисонами (с совпадающими нотами
    // в одной точке панорамы или в центре и на краю) будут удалены из массива вариантов 2H аккордов
    if ( dbox[0].get_check(Converter_With_Unisons).unchecked_state() )
    {
      for (int i = 0; i < vars_num; ++i)
      {
  		  if ( DichoticAccord::test_conflicts( acc2h[i] ) )
        {
  		    acc2h[i].voices_number = 0; // "стираем" аккорд - делаем из него паузу
          need_compress = true;
        }
      }
    }
  }

  if (need_compress)
  {
    // удаляем все аккорды-паузы ("стёртые" аккорды) из массива acc2h - пере копированием
    vector<DichoticAccord> a2;
    for (int i = 0; i < vars_num; ++i) if (acc2h[i].voices_number != 0) a2.push_back( acc2h[i] );
    acc2h = a2;
  }

  Ar <DichoticAccord> &Acc2h = dbox[1].accords_chain; // массив вариантов - 2H аккордов в боксе 1

  // копируем массив acc2h в Acc2h
  int accords_num = min( Acc2h.elements(), (int)acc2h.size() );
  for (int i = 0; i < accords_num; ++i) Acc2h[i] = acc2h[i];
  // когда нет 2H вариантов - делаем один аккорд-паузу
  if (accords_num == 0)
  {
    accords_num = 1;
    Acc2h[0].voices_number = 0;
  }

  dbox[1].cheader = cheader; // копируем хедер секвенции this-бокса 0 в бокс 1
  // подготовка контролов для аккордов в accords_chain и cheader бокса 1
  dbox[1].SetupAccords(accords_num);

  dbox[1].RegenAccord(); // если аккорд звучал - перенажимаем аккорд
  return true;
}

void MusicDialogBoxCtrl::ConverterModeMain(bool Seq)
// управление конвертацией, при Seq = true конвертация всей секвенции
{
  // только при операциях в боксе 0
  if (act_dialog_index != 0) return;
  // только если установлен чекбокс Converter_Mode
  if ( dbox[0].get_check(Converter_Mode).unchecked_state() ) return;

  if ( dbox[0].get_check(Converter_Auto).unchecked_state() )
  {
    // отлючён чекбокс Converter_Auto
    DichoticAccord acc1h;
    // делаем 2H конвертацию:
    if ( dbox[0].checks[Edit_Sequence].checked_state() ) // если выбран чекбокс Edit_Sequence
    {
      // создаем текущий аккорд из контролов
      cheader = ChainHeaderFromControls();
      acc1h = dbox[0].GetAccord();
      // удаление унисонов согласно манипулятору With_Unisons происходит в ConverterModeExecute()
    }
    else
    {
      // берём текущий аккорд секвенции (он будет паузой если нет секвенции)
      // cheader будет из секвенции (или он будет чистым --//--)
      acc1h = accords_chain[accord_act_index];
    }
    // 1H аккорд секвенции обрабатывается в боксе 1, формируя множество 2H вариантов
    if ( !ConverterModeExecute(acc1h) ) goto error;
  }
  else // включён чекбокс Converter_Auto
  {
    // защита от преобразования целой секвенции там где имеет смысл преобразование только одного аккорда
    if (!Seq) return;

    // для каждого аккорда секвенции бокса 0 делается массив вариантов в боксе 1, берётся лучший 2H вариант
    // (самый первый - с меньшим диссонансом), и все эти лучшие аккорды суммируются в секвенцию в боксе 2
    dbox[2].Enable(0); // отключаем бокс 2 на период конвертации
    for (int i = 0; i < accords_in_chain; ++i)
    {
      DichoticAccord &acc1h = accords_chain[i];
      if ( !ConverterModeExecute(acc1h) ) goto error;
      dbox[2].accords_chain[i] = dbox[1].accords_chain[0];
    }
    dbox[2].Enable(1);

    dbox[2].cheader = cheader; // копируем хедер секвенции this-бокса 0 в бокс 2
    // подготовка контролов для аккордов в accords_chain и cheader бокса 2
    dbox[2].SetupAccords(accords_in_chain);

    // копируем комментарий аккорда из бокса 0 в бокс 2
    wchar_t str[1024];
    dbox[0].edits[Accords_Chain_Comment].get_text(str, 1024);
    dbox[2].edits[Accords_Chain_Comment].set_text(str);
  }
  return;

error:
  Mbox("Accord's voices number greater than", MAX_CONVERTER_VOICES);
}

