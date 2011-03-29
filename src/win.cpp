
#include "stdafx.h"

// при старте программы:
int midi_instrument_number = 1; // номер инструмента для всех голосов аккорда, 1 = Bright Acoustic Piano
int midi_device_number = 0; // номер midi девайса: 0 (без синтюка) = программный синтезатор звуковой

// массив идентификаторов для массива контролов MusicDialogBoxCtrl::Ar <DichoticVoice> *dvoices
int control_id_array[][3] =
{
  // численные значения любых из этих идентификаторов произвольны!!
  { IDC_Note_Voice_1, IDC_Pan_Voice_1, IDC_Enable_Voice_1, },
  { IDC_Note_Voice_2, IDC_Pan_Voice_2, IDC_Enable_Voice_2, },
  { IDC_Note_Voice_3, IDC_Pan_Voice_3, IDC_Enable_Voice_3, },
  { IDC_Note_Voice_4, IDC_Pan_Voice_4, IDC_Enable_Voice_4, },
  { IDC_Note_Voice_5, IDC_Pan_Voice_5, IDC_Enable_Voice_5, },
  { IDC_Note_Voice_6, IDC_Pan_Voice_6, IDC_Enable_Voice_6, },
  { IDC_Note_Voice_7, IDC_Pan_Voice_7, IDC_Enable_Voice_7, },
  { IDC_Note_Voice_8, IDC_Pan_Voice_8, IDC_Enable_Voice_8, },
  { IDC_Note_Voice_9, IDC_Pan_Voice_9, IDC_Enable_Voice_9, },
  { IDC_Note_Voice_10, IDC_Pan_Voice_10, IDC_Enable_Voice_10, },
  { IDC_Note_Voice_11, IDC_Pan_Voice_11, IDC_Enable_Voice_11, },
  { IDC_Note_Voice_12, IDC_Pan_Voice_12, IDC_Enable_Voice_12, },
  { IDC_Note_Voice_13, IDC_Pan_Voice_13, IDC_Enable_Voice_13, },
  { IDC_Note_Voice_14, IDC_Pan_Voice_14, IDC_Enable_Voice_14, },
  { IDC_Note_Voice_15, IDC_Pan_Voice_15, IDC_Enable_Voice_15, },
};

CheckCtrl check0 [] =
{
  { IDC_CHECK_Generate_Accord, BST_UNCHECKED, UNI_NULL_STR },// [0] генерировать аккорд основной секвенции
  { IDC_CHECK_Enable_Left_Pan, BST_CHECKED, UNI_NULL_STR },  // [1] включение левой панорамы
  { IDC_CHECK_Enable_Mid_Pan, BST_CHECKED, UNI_NULL_STR },   // [2] включение средней панорамы
  { IDC_CHECK_Enable_Right_Pan, BST_CHECKED, UNI_NULL_STR }, // [3] включение правой панорамы
  { IDC_CHECK_Loop_Chain, BST_UNCHECKED, UNI_NULL_STR },     // [4] зациклить секвенцию

  { IDC_CHECK_Edit_Sequence, BST_UNCHECKED, UNI_NULL_STR }, // разрешение редактирования текущего аккорда секвенции
  { IDC_CHECK_AM_Octave_Shift, BST_UNCHECKED, L"AM +Octave Shift", L"AM -Octave Shift", L"AM NO Shift" }, // TRI-STATE чекбокс-кнопка
  { IDC_CHECK_File_2H_Converter, BST_UNCHECKED, L"Dichotic File", L"Swap Dichotic File", L"Monoaural File" }, // TRI-STATE чекбокс-кнопка
  { IDC_CHECK_Sort_Mode, BST_UNCHECKED, L"Sort Mode 3+3", L"Sort Mode Mono", UNI_NULL_STR },
  { IDC_CHECK_Big_Duration, BST_CHECKED, UNI_NULL_STR }, // меняет предел слайдера Duration

  // "Notes%12": приведение всех номеров нот к диапазону 0...11, как статически, так и динамически!
  { IDC_CHECK_Notes_12, BST_UNCHECKED, UNI_NULL_STR },
  // игнорирование инструментов секвенции (вместо них звучит текущий инструмент)
  { IDC_CHECK_Dont_Change_GM_Instrument, BST_UNCHECKED, UNI_NULL_STR },
  { IDC_CHECK_Edit_Comment, BST_UNCHECKED, UNI_NULL_STR }, // флаг пересылки клавиатурных сообщений в диалогбокс!
  { IDC_CHECK_Smart_Voices, BST_CHECKED, UNI_NULL_STR },   // незвучание совпадающих нот
  { IDC_CHECK_Dichotic_Output, BST_CHECKED, UNI_NULL_STR },// стерео-моно
  { IDC_CHECK_Swap_Output, BST_UNCHECKED, UNI_NULL_STR },  // [15] обмен звук. каналов местами
};

// для диалог бокса 1 некоторые чекбоксы по-видимому не нужны, но выяснять это имеет очень мало смысла...
CheckCtrl check1 [] = {  check0[0], check0[1], check0[2], check0[3], check0[4],
  check0[5], check0[6], check0[7], check0[8], check0[9],
  check0[10], check0[11], check0[12], check0[13], check0[14], check0[15], };

ButtonCtrl button0 [] =
{
  { IDC_Play_Stop, L"Play Chain", L"Stop Chain", 0, 0 }, // кнопка Play/Stop
  { IDC_Rewind, L"Rewind", UNI_NULL_STR, 0, 0 },         // кнопка Rewind
  { IDC_Save_Accords_Chain, L"Save Accords Chain", UNI_NULL_STR, 0, 0 }, // кнопка Save Accords Chain
};

ButtonCtrl button1 [] = { button0[0], button0[1], button0[2], };

SliderCtrl slider0 [] =
{
  // номер аккорда, индикатор-регулятор при проигрывании секвенции аккордов
  { 1, 1, 1, 10, IDC_SLIDER_Accord_number, IDC_STATIC_Accord_number, L"Accord №  ", 1, 1 }, // [0]
  // скорость проигрывания секвенции 0.1...10
  { 1, 100, 1, 10, IDC_SLIDER_Chain_Speed, IDC_STATIC_Chain_Speed, L"Chain Speed  ", 10, 0.1 },
  // скорость нажатия миди ноты
  { 1, 127, 1, 10, IDC_SLIDER_Volume, IDC_STATIC_Volume, L"Volume    ", 100, 1 },
  // параметр транспозиции: это абсолютный миди номер ноты с относительным номером 0
  { 0, 120, 1, 12, IDC_SLIDER_Transposition, IDC_STATIC_Transposition, L"Transposition  ", 60, 1 },
  // длительность аккорда (ударов метронома), для записи в файл и как индикатор при проигрывании
  { 1, 1000, 1, 10, IDC_SLIDER_Duration, IDC_STATIC_Duration, L"Duration    ", 1, 1 }, // зависит от Big_Duration
  // темп (ударов метронома в секунду), для записи в файл и как индикатор при проигрывании
  { 1, 1000, 1, 10, IDC_SLIDER_Temp, IDC_STATIC_Temp, L"Temp (bps)  ", 1, 1 }, // [5]
};

// слайдеры [>=2] отсутствуют на диалог боксе 1, но их значения используются в коде функций бокса 1
SliderCtrl slider1 [] = { slider0[0], slider0[1], slider0[2], slider0[3], slider0[4], slider0[5], };

ComboBoxCtrl combo0 [] =
{
  { IDC_GM_Instrument }, // midi инструмент всех голосов
  { IDC_MidiOutDevice }, // устройство midi вывода
};

ComboBoxCtrl combo1 [] = { combo0[0], combo0[1] };

EditCtrl edit0 [] =
{
  { IDC_EDIT_Accord_Comment, 1, 1 }, // строка комментария аккорда
},

edit1 [] = { edit0[0] };

StaticCtrl static0 [] =
{
  { IDC_STATIC_LEFT,  L"Left:     " }, // ноты аккорда в левой точке панорамы
  { IDC_STATIC_MID,   L"Mid:     " },  // ноты аккорда в центре панорамы
  { IDC_STATIC_RIGHT, L"Right:  " },   // ноты аккорда в правой точке панорамы
  { IDC_STATIC_Total_Diss, L"Dichotic Diss  " }, // полный диссонанс аккорда
  { IDC_STATIC_Uniq_Notes, L"Uniq Notes  " },    // число уникальных нот аккорда
},

static1 [] = { static0[0], static0[1], static0[2], static0[3], static0[4], };


// ===============================  DialogBoxCtrl  ==========================================

int DialogBoxCtrl::dialog_count = 0;

template<class T, int ctrl_numbers> bool DialogBoxCtrl::InitControls(const T (&ctrl_array)[ctrl_numbers])
{
  // Mbox(ctrl_numbers, hwnd() );

  // полиморфизм не применяем, т.к. это усложняет инициализацию контролов в вызывающем коде...
  // вместо него перебираем все возможные типы аргументов...
  if ( typeid(T) == typeid(CheckCtrl) )
  {
    checks.renew(ctrl_numbers);
    for (int i = 0; i < ctrl_numbers; ++i)
    {
      checks[i] = * (const CheckCtrl *) (ctrl_array + i) ;
//    checks[i] = * reinterpret_cast< const CheckCtrl * >(ctrl_array + i) ; // стиль С++
      checks[i].init( hwnd() ); // передаём хендл диалог бокса в контрол
    }
  }
  else if ( typeid(T) == typeid(SliderCtrl) )
  {
    sliders.renew(ctrl_numbers);
    for (int i = 0; i < ctrl_numbers; ++i)
    {
      sliders[i] = *(SliderCtrl*)&ctrl_array[i];
      sliders[i].init( hwnd() );
    }
  }
  else if ( typeid(T) == typeid(ButtonCtrl) )
  {
    buttons.renew(ctrl_numbers);
    for (int i = 0; i < ctrl_numbers; ++i)
    {
      buttons[i] = *(ButtonCtrl*)&ctrl_array[i];
      buttons[i].init( hwnd() );
    }
  }
  else if ( typeid(T) == typeid(EditCtrl) )
  {
    edits.renew(ctrl_numbers);
    for (int i = 0; i < ctrl_numbers; ++i)
    {
      edits[i] = *(EditCtrl*)&ctrl_array[i];
      edits[i].init( hwnd() );
    }
  }
  else if ( typeid(T) == typeid(ComboBoxCtrl) )
  {
    combos.renew(ctrl_numbers);
    for (int i = 0; i < ctrl_numbers; ++i)
    {
      combos[i] = *(ComboBoxCtrl*)&ctrl_array[i];
      combos[i].init( hwnd() );
    }
  }
  else if ( typeid(T) == typeid(StaticCtrl) )
  {
    statics.renew(ctrl_numbers);
    for (int i = 0; i < ctrl_numbers; ++i)
    {
      statics[i] = *(StaticCtrl*)&ctrl_array[i];
      statics[i].init( hwnd() );
    }
  }
  else return false;

  return true;
}

// ===============================  MusicDialogBoxCtrl  =====================================

int MusicDialogBoxCtrl::diss_rating[MAX_INTERVAL];

inline double MusicDialogBoxCtrl::AccordDtime()
{
  return sliders[Duration].actpos /
       ( sliders[ChainSpeed].get_reg_value() * (double)sliders[Temp].actpos );
}

int MusicDialogBoxCtrl::Dissonances(int *notes, int num)
{
  int dis = 0;
  for (int i = 0; i < num; ++i)
  {
    // чтобы не брать каждый интервал дважды - внутр. цикл по верхним индексам
    for (int j = i+1; j < num; ++j)
    {
      // интервал между голосами нот [i], [j]
      int iv = abs(notes[i] - notes[j]);
      dis += diss_rating[iv];
    }
  }
  return dis;
}

int MusicDialogBoxCtrl::CrossDissonances(int *notes1, int num1, int *notes2, int num2)
{
  int dis = 0;
  for (int i1 = 0; i1 < num1; ++i1)
  {
    for (int i2 = 0; i2 < num2; ++i2)
    {
      int iv = abs(notes1[i1] - notes2[i2]);
      dis += diss_rating[iv];
    }
  }
  return dis;
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

bool MusicDialogBoxCtrl::LoadAccords(int format, const wchar_t *file)
// загружаем секвенцию из файла, тип которого зависит от format:
// 0: родной .daccords формат, 1: .PAS формат файлов редактора MUSE
// если есть строка file, то формат 0 грузим молча из этого файла!
{
  // уничтожаем старый массив аккордов, сбрасываем параметры и загреиваем регулятор позиции аккорда и кнопки
  accords_in_chain = 0;
  sliders[AccordNumber].setpos(1);
  sliders[AccordNumber].reset_minmax(1,1);
  sliders[AccordNumber].enable(0);

  sliders[ChainSpeed].setpos(10);
  sliders[ChainSpeed].enable(0);

  checks[Loop_Chain].enable(0);
  checks[Dont_Change_GM_Instrument].enable(0);

  buttons[SaveAccordsChain].enable(0);
  buttons[PlayStop].enable(0);
  buttons[Rewind].enable(0);

  int num_accords = MAX_ACCORDS;

  MusicFile mfile( hwnd() );
  ChainHeader ch;
  bool res;
  
  if (format == 0) res = mfile.Open(ch, *accords_chain, num_accords, file);
  else //    == 1
  {
    // в качестве дихотического сдвига берём значение регулятора относит-й ноты последнего видимого голоса
    int dtrans = (*dvoices)[dialog_voices-1].get_note();
    res = mfile.OpenPAS(dtrans, ch, *accords_chain, num_accords);
  }
  if (!res) return false;

  // устанавливаем параметры и отгреиваем регулятор позиции аккорда и кнопки
  accords_in_chain = num_accords;
  sliders[AccordNumber].reset_minmax(1,accords_in_chain);
  sliders[AccordNumber].enable(1);

  sliders[ChainSpeed].enable(1);
  checks[Loop_Chain].enable(1);
  checks[Dont_Change_GM_Instrument].enable(1);

  buttons[SaveAccordsChain].enable(1);
  buttons[PlayStop].enable(1);
  buttons[Rewind].enable(1);

  // копируем общие параметры секвенции куда надо
  sliders[Transposition].setpos( ch.transposition );
  sliders[ChainSpeed].setpos( sliders[ChainSpeed].get_pos_value(ch.chain_speed) );
  checks[Dont_Change_GM_Instrument].set_checked_state( ch.dont_change_gm_instrument != 0 );

  // однократно устанавливаем инструмент если он не будет извлекаться из каждого аккорда
  if ( checks[Dont_Change_GM_Instrument].checked_state() ) SetupInstrument(ch.instrument_number);

  // готовим проигрывание аккорда в начале новой секвенции
  RewindAccords();
  return true;
}

void MusicDialogBoxCtrl::SortSimilarAccordsMono()
// аккорд секвенции AccordNumber становится 1-м, а остальные аккорды выстраиваются по степени
// совпадения с ним (количество совпадающих нот в монофоническом варианте), без записи в файл!
// все аккорды должны быть N-голосными (N = const, N >= 2) и не должны иметь голоса-паузы...
{
  if (accords_in_chain < 2) return;
  int voices = (*accords_chain)[0].voices_number;
  if (voices < 2) return;

  DichoticAccord *accords1 = *accords_chain;
  Ar <DichoticAccord> accords2(accords_in_chain);

  int n, m;
  // копируем исходную секвенцию аккордов во 2-ю с сортировкой
  for (n = 0; n < accords_in_chain; ++n)
  {
    accords2[n] = accords1[n];
    // сортируем голоса аккорда по возрастанию нот
    qsort(accords2[n].dn, voices, sizeof(DichoticNote), DichoticNote::CmpNote);
  }

  // копируем "опорный аккорд" с индексом accord_act_index = AccordNumber.actpos-1
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
  qsort(accords1, accords_in_chain, sizeof(accords2[0]), DichoticAccord::CmpAcc2);

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
  int voices = (*accords_chain)[0].voices_number;
  if (voices != 6) return;
  int voices2 = voices/2; // 3 голоса в половинке аккорда

  DichoticAccord *accords1 = *accords_chain;
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

  // копируем "опорный аккорд" с индексом accord_act_index = AccordNumber.actpos-1
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
  qsort(accords1, accords_in_chain, sizeof(accords2[0]), DichoticAccord::CmpAcc2);

  // меняем индекс текущего аккорда на 0-й, в котором теперь находится опорный аккорд
  accord_act_index = 0;
  SetupAccord(accord_act_index);
}

void MusicDialogBoxCtrl::AddInversePanAccords()
// добавляет после каждого аккорда точно такой же, но с инверсной панорамой!
{
  if (accords_in_chain < 1 || accords_in_chain > MAX_ACCORDS/2) return;
  DichoticAccord *accords1 = *accords_chain;
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
  sliders[AccordNumber].reset_minmax(1, accords_in_chain);
}

void MusicDialogBoxCtrl::DeleteIdenticAccords6()
// функция для обработки "сырых" 6 голосных дихотик-секвенций (3+3голоса): 
// отбрасывание лишних совпадающих аккордов и сортировка в порядке увеличения нот, запись в файл!
{
  int n, m;

  // переставляем голоса в 1-й и 2-й тройке голосов по возрастанию номеров нот
  for (n = 0; n < accords_in_chain; ++n)
  {
    DichoticAccord &acc = (*accords_chain)[n];
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
    DichoticAccord &acc1 = (*accords_chain)[n];
    if (acc1.voices_number != 6) continue;
    for (m = n+1; m < accords_in_chain; ++m)
    {
      DichoticAccord &acc2 = (*accords_chain)[m];
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
    DichoticAccord &acc1 = (*accords_chain)[n];
    if (acc1.voices_number == 0) // найдена пауза
    {
      // ищем аккорд (не паузу), ставим его на место паузы
      for (m = n+1; m < accords_in_chain; ++m)
      {
        DichoticAccord &acc2 = (*accords_chain)[m];
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
  for (n = 0; n < accords_in_chain; ++n) if ( (*accords_chain)[n].voices_number == 0 ) ++npause;
  accords_in_chain -= npause;

  // сортируем секвенцию по возрастанию номеров нот, 1-й голос старший, 2-й следующий и т.д.
  if (accords_in_chain > 0)
  {
    qsort( accords_chain->memory(), accords_in_chain, sizeof(DichoticAccord), DichoticAccord::CmpAcc );
    // записываем отсортированную секвенцию без дополнительных изменений в файл
    SaveAccord(3);
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
    DichoticAccord &acc = (*accords_chain)[n];
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
    DichoticAccord &acc = (*accords_chain)[n];
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
    DichoticAccord &acc1 = (*accords_chain)[n];
    if (acc1.voices_number != 4) continue;
    for (m = n+1; m < accords_in_chain; ++m)
    {
      DichoticAccord &acc2 = (*accords_chain)[m];
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
    DichoticAccord &acc1 = (*accords_chain)[n];
    if (acc1.voices_number == 0) // найдена пауза
    {
      // ищем аккорд (не паузу), ставим его на место паузы
      for (m = n+1; m < accords_in_chain; ++m)
      {
        DichoticAccord &acc2 = (*accords_chain)[m];
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
  for (n = 0; n < accords_in_chain; ++n) if ( (*accords_chain)[n].voices_number == 0 ) ++npause;
  accords_in_chain -= npause;

  // сортируем секвенцию по возрастанию номеров нот, 1-й голос старший, 2-й следующий и т.д.
  if (accords_in_chain > 0)
  {
    qsort( accords_chain->memory(), accords_in_chain, sizeof(DichoticAccord), DichoticAccord::CmpAcc );
    // записываем отсортированную секвенцию без дополнительных изменений в файл
    SaveAccord(3);
  }
}

void MusicDialogBoxCtrl::AccordsMultiplicator(int points)
// моно-фикация и размножение всех аккордов секвенции на points=2 или 3 точки панорамы с октавным сдвигом
{
  // state=0: +1 октава, state=1: -1 октава, state=2: 0 октава (нет сдвига)
  int state = checks[AM_Octave_Shift].getstate();
  int shift = 0;
  if (state == 0) shift = +1;
  if (state == 1) shift = -1;

  testminxmax(2, points, 3);

/*
каждый исходный аккорд A1 делается монофоническим (стирается панорама),
затем делается два таких же аккорда A0 (на октаву ниже, если чекбокс не 0) и
                            аккорд  A2 (на октаву выше, если чекбокс не 0).
В режиме "2 точки, +1 или 0" делаем панораму A1 влево, A2 вправо,
В режиме "2 точки, -1"       делаем панораму A0 влево, A1 вправо,
В режиме "3 точки, +1 или 0" делаем панораму A0 влево, A1 в центр, A2 вправо,
В режиме "3 точки, -1"       делаем панораму A2 влево, A1 в центр, A0 вправо,
*/
  for (int a = 0; a < accords_in_chain; ++a)
  {
    DichoticAccord &acc1 = (*accords_chain)[a];
    int vn = acc1.voices_number;
    if (vn == 0) continue; // пауза

    // после размножения аккорда количество голосов в нём не должно превышать максимум!
    testmax(vn, MAX_ACC_VOICES/points);

    DichoticAccord acc0, acc2;
    int n;

    // копируем исходный аккорд в два новых
    acc0 = acc2 = acc1;

    // сдвигаем ноты в новых аккордах если надо
    if (shift != 0)
      for(n = 0; n < vn; ++n)
      {
        acc0.dn[n].note -= 12;
        acc2.dn[n].note += 12;
      }

    int pan0 = 0, pan1 = 0, pan2 = 0; // панорамы для аккордов 0,1,2
    if (points == 2)
    {
      if (shift >= 0) { pan1 = -1; pan2 = 1; }
      else            { pan0 = -1; pan1 = 1; }
    }
    else // points = 3
    {
      if (shift >= 0) { pan0 = -1; pan1 = 0; pan2 =  1; }
      else            { pan0 =  1; pan1 = 0; pan2 = -1; }
    }

    // меняем панорамы аккордов
    for(n = 0; n < vn; ++n)
    {
      acc0.dn[n].pan = pan0;
      acc1.dn[n].pan = pan1;
      acc2.dn[n].pan = pan2;
    }

    // прибавляем к исходному аккорду голоса 1-2-х дополнительных аккордов: сначала из b1, затем возможно из b2
    DichoticAccord b1, b2;
    if (points == 2)
    {
      if (shift >= 0) b1 = acc2;
      else            b1 = acc0;
    }
    else // points = 3
    {
      b1 = acc0, b2 = acc2;
    }

    // прибавление голосов из аккорда b1
    for(n = 0; n < vn; ++n) acc1.dn[n+vn] = b1.dn[n];

    // прибавление голосов из аккорда b2
    if (points == 3)
    for(n = 0; n < vn; ++n) acc1.dn[n+2*vn] = b2.dn[n];

    acc1.voices_number = vn*points;
  }
}

void MusicDialogBoxCtrl::SaveAccord(int mode)
// при mode=0 (Save_Accord       ) записываем текущий аккорд в файл, если файл есть - стираем и перезаписываем его
// при      1 (Append_Accord     ) добавляем аккорд к сушествующему файлу, а если файла нет - пишем его с нуля!
// при      2 (Save_Accords_Chain) записываем секвенцию аккордов в файл, если файл есть - стираем и перезаписываем его
// при      3 (запись секвенции после сортировки) то же что и при 2, кроме замены текущего аккорда!
// при (2) сначала текущий звучащий аккорд копируется в текущее место секвенции, т.о. возможно её редактирование!
// при (0,1,3) секвенцию не меняем!
// если чекбокс Edit_Sequence не выбран - секвенцию не меняем и при mode=2!
{
  // создаем данные текущего аккорда из регуляторов
  ChainHeader ch;
  DichoticAccord acc;

  // копируем общие параметры в ch
  ch.transposition = sliders[Transposition].actpos;
  ch.chain_speed = sliders[ChainSpeed].get_reg_value();
  ch.dont_change_gm_instrument = checks[Dont_Change_GM_Instrument].checked_state();
  ch.instrument_number = combos[GM_Instrument].GetStringIndex();

  // копируем параметры звучащих голосов аккорда в acc
  acc.instrument = ch.instrument_number;
  acc.temp =     sliders[Temp].actpos;
  acc.duration = sliders[Duration].actpos;
  acc.volume =   sliders[ Volume ].actpos;

  int n = -1; // номер последнего включённого голоса
  for (int i = 0; i < MAX_ACC_VOICES; ++i)
  {
    if ( (*dvoices)[i].get_enable() )
    {
      acc.dn[i].pause = 0;
      acc.dn[i].note = (*dvoices)[i].get_note();
      acc.dn[i].pan = (*dvoices)[i].get_true_pan( checks[Dichotic_Output].unchecked_state(), checks[Swap_Output].checked_state() );
      n = i;
    }
    else // отключённый голос: сохраняем голос в виде "паузы"
    {
      acc.dn[i].pause = 1;
    }
  }
  acc.voices_number = n+1;

  MusicFile mfile( hwnd() );

  if (mode == 0 || mode == 1)
  {
    // стираем комментарий одиночного аккорда!
    acc.clear_comment();
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
      if (ch.dont_change_gm_instrument) acc.instrument = (*accords_chain)[accord_act_index].instrument;

      // копируем аккорд в секвенцию, без изменения исходного комментария секвенции!
      (*accords_chain)[accord_act_index].copy_wo_comment( acc );
    }
    // else не меняем секвенцию!

    // записываем секвенцию аккордов в новый файл
    mfile.SaveAs(ch, *accords_chain, accords_in_chain, mode);
  }
}

void MusicDialogBoxCtrl::ResetMaxDuration()
{
  // устанавливаем предел слайдера Duration
  checks[Big_Duration].getstate();
  int new_max = checks[Big_Duration].checked_state()? 10000:1000;
  sliders[Duration].reset_minmax(1, new_max);
  int pos = sliders[Duration].getpos();
  sliders[Duration].setpos(pos);
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
        PlayStopAccords();
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
  if (play == 2) PlayStopAccords();

  accord_act_index = 0;
  SetupAccord(accord_act_index);

  if (play == 2) PlayStopAccords();
}

void MusicDialogBoxCtrl::PlayStopAccords()
{
  if (play_accords_chain == 0)
    play_accords_chain = 1;
  else // play_accords_chain = 1 или 2
    play_accords_chain = 0;

  buttons[PlayStop].button_text(play_accords_chain==0? 0:1);

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
    if ( checks[Notes_12].checked_state() ) NotesModulo12(); // приводим все ноты к диапазону 0...11
    if ( checks[Smart_Voices].checked_state() ) ConflictDiss(); // устраняем конфликты диссонансов

    // включаем ноты аккорда
    for (int i = 0; i < MAX_ACC_VOICES; ++i)
    {
      // сначала сбрасываем флаги истинного включения и конфликта зашкала у всех голосов
      (*dvoices)[i].true_enable = 0;
      (*dvoices)[i].conflict_scale = 0;
      // (*dvoices)[i].conflict_diss = 0; это делается не здесь!
      if ((*dvoices)[i].get_enable())
      {
        // если панорама ноты запрещена чекбоксом - она не будет звучать
        int pan = (*dvoices)[i].get_true_pan( checks[Dichotic_Output].unchecked_state(), checks[Swap_Output].checked_state() );
        bool okpan = 1;
        if ( pan == -1 && checks[Enable_Left_Pan].unchecked_state() ) okpan = 0;
        if ( pan ==  0 && checks[Enable_Mid_Pan].unchecked_state() ) okpan = 0;
        if ( pan == +1 && checks[Enable_Right_Pan].unchecked_state() ) okpan = 0;
        if (okpan)
        {
          // запоминаем миди ноту голоса
          int midi_note0 = sliders[Transposition].actpos;
          int note = (*dvoices)[i].midi_note = midi_note0 + (*dvoices)[i].get_note();
          // определяем конфликт зашкаливания ноты и если оного нету - включаем голос
          if ( !in_range(MidiInterface::MIDI_NOTE_MIN, note, MidiInterface::MIDI_NOTE_MAX) ) (*dvoices)[i].conflict_scale = 1;
          else // зашкаливания нет
          if ( checks[Smart_Voices].checked_state() && (*dvoices)[i].conflict_diss ) ; // пропускаем конфликтующие голоса
          else // их нет или их не надо пропускать
          {
            (*dvoices)[i].true_enable = true;
            NewNoteOn(i); // включаем ноту голоса i
          }
          // флаг конфликта диссонанса обновляется и выводится позднее в NotesSetText() -> NotesGetPos()
        }
      }
      // выводим титул строки всех голосов, возможно с флагом conflict_scale
      (*dvoices)[i].set_title( checks[Smart_Voices].checked_state() );
    }

    NotesSetText(); // выводим список нот аккорда по каналам звука

    gen_state = 2;
  }

  if (gen_state == 3) // выключаем аккорд
  {
    // выключаем все звучавшие голоса аккорда:
    for (int i = 0; i < MAX_ACC_VOICES; ++i) if ((*dvoices)[i].true_enable) NewNoteOff(i);
    gen_state = 0;
  }
}

void MusicDialogBoxCtrl::SetupAccord(int index)
// копируем аккорд из массива с индексом index в аккорд на диалогпанели
{
  // номер аккорда в индикторе на 1 больше индекса
  sliders[AccordNumber].setpos(index+1);

  DichoticAccord acc = (*accords_chain)[index];

  if ( checks[Dont_Change_GM_Instrument].unchecked_state() &&
       combos[GM_Instrument].GetStringIndex() != acc.instrument &&
       acc.voices_number > 0 ) // при паузе номер инструмента не меняется!
  {
    SetupInstrument(acc.instrument);
  }

  sliders[Temp].setpos( acc.temp );
  sliders[Duration].setpos( acc.duration );
  sliders[ Volume ].setpos( acc.volume );

  for (int n = 0; n < MAX_ACC_VOICES; ++n)
  {
    (*dvoices)[n].set_enable(false);
    if ( n < acc.voices_number && 0 == acc.dn[n].pause )
    {
      (*dvoices)[n].set_enable(true);
      (*dvoices)[n].set_note(acc.dn[n].note);
      (*dvoices)[n].set_pan(acc.dn[n].pan);
    }
  }
}

void MusicDialogBoxCtrl::EnableAllVoices()
// кнопка сброса включателей всех голосов в откл., затем циклически включение-выключение первых 6 голосов
{
  static int state = 0; // 0: все в 0; 1: первые 6 в 1;

  // здесь нужно отключать все голоса, а не только регулируемые на диалогпанели: при проигрывании файлов
  // "невидимые" голоса могут остаться включенными и будут звучать и дальше при создании аккорда вручную!
  for (int i = 0; i < MAX_ACC_VOICES; ++i)
  {
    bool enable = false;
    // имеем ввиду, что dialog_voices в общем случае может быть меньше 6 !
    if (state == 1 && i < 6 && i < dialog_voices) enable = true;
    (*dvoices)[i].set_enable(enable);
  }

  state = state==0? 1:0;
}

bool MusicDialogBoxCtrl::PanAllVoices()
// кнопка циклического переключения КНОПОК всех панорам лево-центр-право
// возвращает true если хотя бы один голос был включен (звучал)
{
  static int state = 0; // 0,1,2
  int pan[3] = {-1,0,1};
  bool enable = false;

  for (int i = 0; i < dialog_voices; ++i)
  {
    (*dvoices)[i].set_pan(pan[state]);
    enable |= (*dvoices)[i].get_enable();
  }

  if (++state > 2) state = 0;
  return enable;
}

bool MusicDialogBoxCtrl::RenumberAllNotes()
// кнопка перенумерации нот ВКЛЮЧЕННЫХ голосов (всех, а не только диалоговых) чтобы мин. относит. нота стала = 0
// с соотв-щим изменением общего номера 0-й ноты (транспозиции)
// возвращает true если хотя бы один голос был включен (звучал)
{
  bool enable = false;

  // определяем минимум из всех включенных относ-х номеров нот
  int nmin = 777;
  for (int i = 0; i < MAX_ACC_VOICES; ++i)
  {
    if (!(*dvoices)[i].get_enable()) continue; // отключенные голоса не смотрим
    nmin = min(nmin, (*dvoices)[i].get_note());
    enable = true;
  }

  // если все голоса отключены или минимум = 0 не перенумеруем
  if (!enable || nmin == 0) return false;

  // корректируем все включённые ноты и общий номер 0-й ноты
  for (int i = 0; i < MAX_ACC_VOICES; ++i)
  {
    if (!(*dvoices)[i].get_enable()) continue; // отключенные голоса не меняем
    int note = (*dvoices)[i].get_note();
    (*dvoices)[i].set_note(note - nmin);
  }

  int midi_note0 = sliders[Transposition].actpos;
  midi_note0 += nmin;
  sliders[Transposition].setpos(midi_note0);

  return true;
}

bool MusicDialogBoxCtrl::VoicesGroup(int first_voice, int voices_num, int operation)
// в группе голосов делаем одинаковую (РУЧНУЮ!) операцию: -1, +1 сдвиг высоты; 0 сброс высоты в 0
// возвращает true если хотя бы один голос группы был включен (звучал)
{
  int last_voice = first_voice + voices_num - 1;
  test_max(last_voice, dialog_voices-1); // здесь dialog_voices - т.к. это ручная операция

  bool enable = false;

  for (int i = first_voice; i <= last_voice; ++i)
  {
    int note = (*dvoices)[i].get_note();
    if (operation == 0) note = 0;
    else note = (operation < 0)? note-1:note+1;
    (*dvoices)[i].set_note(note);
    enable |= (*dvoices)[i].get_enable();
  }

  return enable;
}

bool MusicDialogBoxCtrl::NotesGetPos(int *notes1, int &num1, int *notes2, int &num2, int *notes3, int &num3)
// анализирует позиции звучащих голосов и записывает их ноты в 3 массива:
// notes1[num1] для левого края панорамы, notes2[num2] для центра, notes3[num3] для правого края
// также анализирует недопустимое совпадение номеров нот в одном канале звука и устанавливает
// соотв. флаги в конфликтующих голосах, а затем делает эти флаги видимыми
{
  bool conflict_diss_present = false; // true если есть хотя бы один конфликт!
  int i,j;

  // сначала готовим массивы по точкам панорамы
  num1 = num2 = num3 = 0;
  for (i = 0; i < MAX_ACC_VOICES; ++i)
  {
    if (!(*dvoices)[i].true_enable) continue;

    int note = (*dvoices)[i].get_note();
    int pan = (*dvoices)[i].get_true_pan( checks[Dichotic_Output].unchecked_state(), checks[Swap_Output].checked_state() );

    if (pan <  0) notes1[num1++] = note;
    if (pan == 0) notes2[num2++] = note;
    if (pan >  0) notes3[num3++] = note;
  }

  // сбрасываем флаг конфликта диссонанса у всех голосов!
  for (i = 0; i < MAX_ACC_VOICES; ++i) (*dvoices)[i].conflict_diss = 0;

  // анализ конфликтов диссонанса
  for (i = 0; i < MAX_ACC_VOICES; ++i)
  {
    if (!(*dvoices)[i].true_enable) continue;
    int note = (*dvoices)[i].get_note();
    int pan = (*dvoices)[i].get_true_pan( checks[Dichotic_Output].unchecked_state(), checks[Swap_Output].checked_state() );
    // находим все совпадающие ноты _других_ голосов и анализируем их позиции
    for (j = 0; j < MAX_ACC_VOICES; ++j)
    {
      if (!(*dvoices)[j].true_enable) continue;
      if (i == j) continue;
      int note2 = (*dvoices)[j].get_note();
      int pan2 = (*dvoices)[j].get_true_pan( checks[Dichotic_Output].unchecked_state(), checks[Swap_Output].checked_state() );
      if (note != note2) continue;
      // note == note2, анализируем соотношение панорам обоих голосов
      switch (pan)
      {
        // левый край конфликтует с левым и с центром
        case -1: if (pan2 == -1 || pan2 == 0) { (*dvoices)[i].conflict_diss = 1; goto nexti; } break;
        // центр конфликтует со всеми тремя точками
        case  0: (*dvoices)[i].conflict_diss = 1; goto nexti;
        // правый край конфликтует с правым и с центром
        case +1: if (pan2 == +1 || pan2 == 0) { (*dvoices)[i].conflict_diss = 1; goto nexti; } break;
      }
    }
nexti: ;
  }

  // апдейт титулов голосов с учётом конфликтов
  for (i = 0; i < MAX_ACC_VOICES; ++i)
    if ((*dvoices)[i].true_enable && (*dvoices)[i].conflict_diss)
    {
      conflict_diss_present = true;
      (*dvoices)[i].set_title( checks[Smart_Voices].checked_state() );
    }

  return conflict_diss_present;
}

void MusicDialogBoxCtrl::ConflictDiss()
// ставим флаг конфликта диссонанса во всех конфликтующих голосах, кроме одного (первого)
// чтобы затем в GenerateAccord() не включать голоса с поднятым флагом конфликта диссонанса
{
  int i, j;

  // сначала сбрасываем флаг конфликта диссонанса у всех голосов!
  for (i = 0; i < MAX_ACC_VOICES; ++i) (*dvoices)[i].conflict_diss = 0;

  // основная работа функции тут
  for (i = 0; i < MAX_ACC_VOICES; ++i)
  {
    // не анализируем отключённые кнопкой голоса и те у которых уже поднят конфликт диссонанса
    if (!(*dvoices)[i].get_enable() || (*dvoices)[i].conflict_diss) continue;

    int note = (*dvoices)[i].get_note();
    int pan = (*dvoices)[i].get_true_pan( checks[Dichotic_Output].unchecked_state(),
                                          checks[Swap_Output].checked_state() );

    // если панорама ноты запрещена чекбоксом - она не будет звучать
    if ( pan == -1 && checks[Enable_Left_Pan].unchecked_state() ) continue;
    if ( pan ==  0 && checks[Enable_Mid_Pan].unchecked_state() ) continue;
    if ( pan == +1 && checks[Enable_Right_Pan].unchecked_state() ) continue;

    // находим все совпадающие ноты _других_ голосов и анализируем их позиции
    for (j = 0; j < MAX_ACC_VOICES; ++j)
    {
      // не анализируем отключённые кнопкой голоса и те у которых уже поднят конфликт диссонанса
      if (!(*dvoices)[j].get_enable() || (*dvoices)[j].conflict_diss) continue;
      if (i == j) continue;
      int note2 = (*dvoices)[j].get_note();
      int pan2 = (*dvoices)[j].get_true_pan( checks[Dichotic_Output].unchecked_state(), checks[Swap_Output].checked_state() );
      if (note != note2) continue;
      // else note == note2
      // анализируем соотношение панорам голосов i,j и поднимаем конфликт диссонанса у j-го
      switch (pan)
      {
        // левый край конфликтует с левым и с центром
        case -1: if (pan2 == -1 || pan2 == 0) (*dvoices)[j].conflict_diss = 1; break;
        // центр конфликтует со всеми тремя точками
        case  0: (*dvoices)[j].conflict_diss = 1; break;
        // правый край конфликтует с правым и с центром
        case +1: if (pan2 == +1 || pan2 == 0) (*dvoices)[j].conflict_diss = 1; break;
      }
    }
  }
}

void MusicDialogBoxCtrl::NotesSetText()
// выводим на диалогбоксе номера нот в соответствующих им позициях панорамы,
// также выводим суммарный диссонанс аккорда и др.
{
  int notes1[MAX_ACC_VOICES], notes2[MAX_ACC_VOICES], notes3[MAX_ACC_VOICES];
  int num1=0, num2=0, num3=0;
  wstring2 s;
  int i;

  // анализируем пространственные позиции голосов и записываем ноты в 3 соотв. массива
  bool total_conflict_diss = NotesGetPos(notes1, num1, notes2, num2, notes3, num3);

  // стираем s, выводим список относит-х номеров нот голосов с левой позицией панорамы
  s.clear();
  for (i = 0; i < num1; ++i) s << notes1[i] << L", ";
  statics[Left_Notes].add_text( s );

  // то же самое для центра панорамы
  s.clear();
  for (i = 0; i < num2; ++i) s << notes2[i] << L", ";
  statics[Mid_Notes].add_text( s );

  // то же для правой позиции панорамы
  s.clear();
  for (i = 0; i < num3; ++i) s << notes3[i] << L", ";
  statics[Right_Notes].add_text( s );

  // определяем и выводим суммарный диссонанс аккорда с учётом панорамы и конфликтов

  // суммируем диссонансы всех интервалов внутри каждой группы голосов
  int dis = Dissonances(notes1, num1) + Dissonances(notes2, num2) + Dissonances(notes3, num3);

  // крайние голоса панорамы между собой не дают диссонансов,
  // а голос в середине панорамы дает 1/2 диссонансов с голосами на краях панорамы
  int dis2 = CrossDissonances(notes1, num1, notes2, num2) + CrossDissonances(notes3, num3, notes2, num2);

  dis += dis2/2; // т.к. все диссонансы чётные - дробной части не будет!

  s.clear();
  s << dis;
  if (total_conflict_diss) s += L" ?";
  statics[Total_Diss].add_text( s );

  // копируем все ноты аккорда в массив с исключением совпадающих нот
  int notes[MAX_ACC_VOICES];
  int num = 0;
  for (i = 0; i < MAX_ACC_VOICES; ++i)
  {
    if (!(*dvoices)[i].true_enable) continue;
    int note = (*dvoices)[i].get_note();
    for (int n = 0; n < num; ++n) if (note == notes[n]) goto next;
    notes[num++] = note;
next: ;
  }
  s.clear();
  s << num;
  statics[Uniq_Notes].add_text( s );
}

void MusicDialogBoxCtrl::NotesModulo12()
// приводим все ноты к диапазону 0...11
{
  for (int i = 0; i < MAX_ACC_VOICES; ++i)
  {
    // у каждого включённого кнопкой голоса номер октавы ноты устанавливаем в 0
    if ( (*dvoices)[i].get_enable() ) (*dvoices)[i].set_octave(0);
  }
}

void MusicDialogBoxCtrl::NewNoteOff(int voice_index)
// отключаем генерацию голоса voice_index
{
  int midi_chan = (*dvoices)[voice_index].get_midi_channel();
  int midi_note = (*dvoices)[voice_index].midi_note;

  // выключаем ноту c 0-й громкостью
  MIDII.MidiNoteOff(midi_chan, midi_note, 0);
}

void MusicDialogBoxCtrl::NewNoteOn(int voice_index)
// включаем генерацию голоса voice_index
{
  int midi_chan = (*dvoices)[voice_index].get_midi_channel();
  int midi_note = (*dvoices)[voice_index].midi_note;

  int true_pan = (*dvoices)[voice_index].get_true_pan( checks[Dichotic_Output].unchecked_state(),
                                                       checks[Swap_Output].checked_state() );
  MIDII.ChanPan(midi_chan, true_pan); // панорамируем канал

  // включаем ноту c текущей громкостью регулятора
  int volume = sliders[Volume].actpos;
  MIDII.MidiNoteOn(midi_chan, midi_note, volume);
}

template<int number> void MusicDialogBoxCtrl::InitDichoticVoices(const int (&id_array)[number][3], int max_number)
// инициализируем голоса dvoices из массива идентификаторов их контролов [number][Note_id, Pan_id, Enable_id]
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
    (*dvoices)[i].init( hwnd(), id_array[i][0], id_array[i][1], id_array[i][2], i, dialog_voices, checks[Smart_Voices].checked_state() );
    //                          Note_id         Pan_id          Enable_id
  }
}

void MusicDialogBoxCtrl::SetupInstrument(int instrument_number)
{
  combos[GM_Instrument].SetStringIndex(instrument_number);
  MIDII.AllChanPatchChange(instrument_number); // сменим инструмент во всех midi каналах
}

void MusicDialogBoxCtrl::InitInstruments(int instrument_number)
{
  for (int i = 0; i < MidiInterface::GM_INSTRUMENTS_NUMBER; ++i)
  {
    combos[GM_Instrument].AddString( MidiInterface::PatchNames[i] );
  }
  SetupInstrument(instrument_number);
}

void MusicDialogBoxCtrl::InitMidiDevices(int device_number, int instrument_number)
{
  // заполняем список midi девайсов
  int dnums = midiOutGetNumDevs();
  for (int i = 0; i < dnums; i++)
  {
    MIDIOUTCAPS DevCaps;
    midiOutGetDevCaps(i, &DevCaps, sizeof (DevCaps));
    combos[MidiOutDevice].AddString( DevCaps.szPname );
  }

  // первоначальное открытие midi девайса и установка нужного инструмента во всех каналах
  bool res = MIDII.OpenDeviceOut(device_number, instrument_number);
  if ( !res ) Mbox(L"Error in MIDII.OpenDeviceOut(), device_number=", device_number);
  combos[MidiOutDevice].SetStringIndex( device_number ); // показываем имя девайса
}

int CALLBACK MusicDialogBoxCtrl::DialogProc0(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
// обработчик сообщений диалог бокса 0
{
  act_dialog_index = 0; // определяем индекс активного диалогбокса!

  // сообщения клавиатуры здесь не появляются, они попадают в IsDialogMessage() и транслируются
  // в специальные диалоговые сообщения типа изменения фокуса ввода по клавише Tab
  switch (message)
  {
    case WM_TIMER: act_dbox().TimerMessage(); EatMessages(hwnd, message); break;

    // этот мессаг приходит ДО возврата из CreateDialog(), поэтому хендл диалог бокса берём из hwnd !!
    case WM_INITDIALOG: act_dbox().hdlg = hwnd; act_dbox().InitDialog0(); return TRUE; // ?
    // return TRUE to set the keyboard focus to the control specified by wParam, otherwise return FALSE

    case WM_HSCROLL: case WM_VSCROLL: // сообщения от любых регулируемых слайдеров
      {
        // если любой регулятор звучащего голоса передвинулся - перенажимаем аккорд
        int regen = 0, pos;

        // читаем позиции всех регуляторов слайдеров (т.е. видимых) голосов
        for (int i = 0; i < act_dbox().dialog_voices; ++i)
        {
          int pan = (*act_dbox().dvoices)[i].get_pan();
          // если панорама меняется у голоса, который был включен кнопкой диалога (enable=1), но отключен
          // флагом класса (true_enable=0), то всё равно перенажимаем аккорд, т.к. может исчезнуть причина
          // отключения флагом (напр. исчезнет конфликт с другим голосом)
          if (pan != (*act_dbox().dvoices)[i].get_pan_reg() && (*act_dbox().dvoices)[i].get_enable()) regen = 1;

          int note = (*act_dbox().dvoices)[i].get_note();
          // т.к. при изменении включённой кнопкой ноты может измениться конфликт - перенажимаем аккорд!
          if (note != (*act_dbox().dvoices)[i].get_note_reg() && (*act_dbox().dvoices)[i].get_enable()) regen = 1;
        }

        pos = act_dbox().sliders[ChainSpeed].actpos;
        if (pos != act_dbox().sliders[ChainSpeed].getpos()) regen = 1;

        pos = act_dbox().sliders[ Volume ].actpos;
        if (pos != act_dbox().sliders[ Volume ].getpos()) regen = 1;

        pos = act_dbox().sliders[Transposition].actpos;
        if (pos != act_dbox().sliders[Transposition].getpos()) regen = 1; // тут апдейтится Transposition.actpos!

        // если номер аккорда изменился - обновляем аккорд
        // при автопроигрывании секвенции длительность первого нового аккорда будет неправильной!
        pos = act_dbox().sliders[AccordNumber].actpos;
        if (pos != act_dbox().sliders[AccordNumber].getpos())
        {
          act_dbox().accord_act_index = act_dbox().sliders[AccordNumber].actpos-1; // индекс аккорда на 1 меньше позиции слайдера
          act_dbox().SetupAccord( act_dbox().accord_act_index );
          regen = 1;
        }

        act_dbox().sliders[Duration].getpos();
        act_dbox().sliders[Temp].getpos();

        if (regen) act_dbox().RegenAccord();
      }
      break;

    case WM_COMMAND: // командные сообщения Windows
      {
      UINT CtrlId = LOWORD(wparam);
      UINT CtrlMsg = HIWORD(wparam);
      switch (CtrlId)
      {
        case IDC_BUTTON_HELP: ShellExecuteOpen( AddModulePath(L"help.txt").c_str() ); break; // показать документацию

        case IDC_BUTTON_WWW: ShellExecuteOpen(website); break; // выход на WEB SITE

        case IDC_BUTTON_CLOSE: case IDCANCEL: // кнопки закрытия окна
          PostMessage(hwnd, WM_QUIT, 0, 0);
          return TRUE; // ?

        case IDC_Del_Identic3and3: act_dbox().DeleteIdenticAccords6(); break; // кнопка "Del Identic (3+3)"

        case IDC_Del_Identic2and2: act_dbox().DeleteIdenticAccords4(); break; // кнопка "Del Identic (2+2)"

        case IDC_Add_Inverse_Pan_Accords: act_dbox().AddInversePanAccords(); break; // кнопка "Add Inverse Pan"

        case IDC_Sort_Similar_Accords: // кнопка "Sort Similar Accords"
          if ( act_dbox().checks[Sort_Mode].checked_state() ) act_dbox().SortSimilarAccordsMono();
          else                                                act_dbox().SortSimilarAccords3and3();
          break;

        // кнопки файловых операций и проигрывания секвенции:
        case IDC_Save_Accord:        act_dbox().SaveAccord(0); break;
        case IDC_Append_Accord:      act_dbox().SaveAccord(1); break;
        case IDC_Save_Accords_Chain: act_dbox().SaveAccord(2); break;
        case IDC_Load_Accords: if ( act_dbox().LoadAccords(0) ) act_dbox().RegenAccord(); break;

        case IDC_Play_Stop: act_dbox().PlayStopAccords(); break;
        case IDC_Rewind:    act_dbox().RewindAccords(); break;

        case IDC_Accords_Multi_2: act_dbox().AccordsMultiplicator(2); break; // моно-фикация и размножение аккорда на 2 точки панорамы
        case IDC_Accords_Multi_3: act_dbox().AccordsMultiplicator(3); break; // моно-фикация и размножение аккорда на 3 точки панорамы

        case IDC_Enable_All_Voices: act_dbox().EnableAllVoices(); act_dbox().RegenAccord(); break; // кнопка включателей всех голосов
        case IDC_Pan_All_Voices: if (act_dbox().PanAllVoices()) act_dbox().RegenAccord(); break; // кнопка для всех панорам
        case IDC_Renumber_All_Notes: if (act_dbox().RenumberAllNotes()) act_dbox().RegenAccord(); break; // кнопка перенумерации нот
        
        // кнопки синхронного сдвига +-1 и сброса в 0 высот троек голосов 1-3, 4-6, 7-9, 10-12, 13-15
        case IDC_V1V15_0: if ( act_dbox().VoicesGroup(0, 15, 0) ) act_dbox().RegenAccord(); break;

        case IDC_V1V3_INC: if ( act_dbox().VoicesGroup(0, 3, +1) ) act_dbox().RegenAccord(); break;
        case IDC_V1V3_0:   if ( act_dbox().VoicesGroup(0, 3,  0) ) act_dbox().RegenAccord(); break;
        case IDC_V1V3_DEC: if ( act_dbox().VoicesGroup(0, 3, -1) ) act_dbox().RegenAccord(); break;

        case IDC_V4V6_INC: if ( act_dbox().VoicesGroup(3, 3, +1) ) act_dbox().RegenAccord(); break;
        case IDC_V4V6_0:   if ( act_dbox().VoicesGroup(3, 3,  0) ) act_dbox().RegenAccord(); break;
        case IDC_V4V6_DEC: if ( act_dbox().VoicesGroup(3, 3, -1) ) act_dbox().RegenAccord(); break;

        case IDC_V7V9_INC: if ( act_dbox().VoicesGroup(6, 3, +1) ) act_dbox().RegenAccord(); break;
        case IDC_V7V9_0:   if ( act_dbox().VoicesGroup(6, 3,  0) ) act_dbox().RegenAccord(); break;
        case IDC_V7V9_DEC: if ( act_dbox().VoicesGroup(6, 3, -1) ) act_dbox().RegenAccord(); break;

        case IDC_V10V12_INC: if ( act_dbox().VoicesGroup(9, 3, +1) ) act_dbox().RegenAccord(); break;
        case IDC_V10V12_0:   if ( act_dbox().VoicesGroup(9, 3,  0) ) act_dbox().RegenAccord(); break;
        case IDC_V10V12_DEC: if ( act_dbox().VoicesGroup(9, 3, -1) ) act_dbox().RegenAccord(); break;

        case IDC_V13V15_INC: if ( act_dbox().VoicesGroup(12, 3, +1) ) act_dbox().RegenAccord(); break;
        case IDC_V13V15_0:   if ( act_dbox().VoicesGroup(12, 3,  0) ) act_dbox().RegenAccord(); break;
        case IDC_V13V15_DEC: if ( act_dbox().VoicesGroup(12, 3, -1) ) act_dbox().RegenAccord(); break;

        case IDC_MidiOutDevice: // действие со списком устройств
          if (CtrlMsg == CBN_SELENDOK) // завершен выбор из списка устройств (см также CBN_SELCHANGE)
          {
            // определяем номер выбранного устройства в списке
            int device_number = act_dbox().rw_combo(MidiOutDevice).GetStringIndex();
            int instrument_number = act_dbox().rw_combo(GM_Instrument).GetStringIndex(); // это не менялось
            MIDII.CloseDevicesOut(); // закроем текущее устройство
            bool res = MIDII.OpenDeviceOut(device_number, instrument_number);  // откроем новое устройство
            if ( !res ) Mbox(L"Error in MIDII.OpenDeviceOut(), device_number=", device_number);
            act_dbox().RegenAccord();
          }
          break;

        case IDC_GM_Instrument: // действие со списком инструментов
          if (CtrlMsg == CBN_SELCHANGE) // выбран новый инструмент в списке инструментов (см также CBN_SELENDOK)
          {
            int instrument_number = act_dbox().rw_combo(GM_Instrument).GetStringIndex();
            act_dbox().SetupInstrument(instrument_number); // установим нужный номер инструмента и отобразим его на диалогпанели
            act_dbox().RegenAccord();
          }
          break;

        case IDC_CHECK_Generate_Accord: // дублируется хоткеями (Insert, Control)
          {
            act_dbox().checks[Generate_Accord].getstate();
            int gen_state = act_dbox().checks[Generate_Accord].checked_state()? 1:3;
            act_dbox().MaybeGenerateAccord(gen_state); // возможное вкл. или откл. аккорда
          }
          break;

        case IDC_CHECK_Big_Duration: act_dbox().ResetMaxDuration(); break;

        case IDC_CHECK_Notes_12:     act_dbox().checks[Notes_12].getstate(); act_dbox().RegenAccord(); break;
        case IDC_CHECK_Swap_Output:  act_dbox().checks[Swap_Output].getstate(); act_dbox().RegenAccord(); break;
        case IDC_CHECK_Smart_Voices: act_dbox().checks[Smart_Voices].getstate(); act_dbox().RegenAccord(); break;
        case IDC_CHECK_Dichotic_Output:  act_dbox().checks[Dichotic_Output].getstate(); act_dbox().RegenAccord(); break;
        case IDC_CHECK_Enable_Left_Pan:  act_dbox().checks[Enable_Left_Pan].getstate();  act_dbox().RegenAccord(); break;
        case IDC_CHECK_Enable_Mid_Pan:   act_dbox().checks[Enable_Mid_Pan].getstate();   act_dbox().RegenAccord(); break;
        case IDC_CHECK_Enable_Right_Pan: act_dbox().checks[Enable_Right_Pan].getstate(); act_dbox().RegenAccord(); break;

        case IDC_CHECK_Edit_Comment:
          act_dbox().checks[Edit_Comment].getstate();
          act_dbox().edits[Accord_Comment].readonly( act_dbox().checks[Edit_Comment].unchecked_state() );
          break;
        case IDC_CHECK_Edit_Sequence:     act_dbox().checks[Edit_Sequence].getstate(); break;
        case IDC_CHECK_AM_Octave_Shift:   act_dbox().checks[AM_Octave_Shift].getstate(); break;
        case IDC_CHECK_File_2H_Converter: act_dbox().checks[File_2H_Converter].getstate(); break;
        case IDC_CHECK_Sort_Mode:      act_dbox().checks[Sort_Mode].getstate(); break;
        case IDC_CHECK_Loop_Chain:     act_dbox().checks[Loop_Chain].getstate(); break;
        case IDC_CHECK_Dont_Change_GM_Instrument: act_dbox().checks[Dont_Change_GM_Instrument].getstate(); break;

        case IDC_Enable_Voice_1: // действия с чекбоксами включения голосов
        case IDC_Enable_Voice_2:
        case IDC_Enable_Voice_3:
        case IDC_Enable_Voice_4:
        case IDC_Enable_Voice_5:
        case IDC_Enable_Voice_6:
        case IDC_Enable_Voice_7:
        case IDC_Enable_Voice_8:
        case IDC_Enable_Voice_9:
        case IDC_Enable_Voice_10:
        case IDC_Enable_Voice_11:
        case IDC_Enable_Voice_12:
        case IDC_Enable_Voice_13:
        case IDC_Enable_Voice_14:
        case IDC_Enable_Voice_15:
          {
            int i = CtrlId - IDC_Enable_Voice_1;
            (*act_dbox().dvoices)[i].get_enable_reg(); // апдейтим изменения
            // всегда перенажимаем аккорд, т.к. какой-то голос переключился
            // (если был конфликт зашкала, то может и не так, но это не учитываем)
            act_dbox().RegenAccord();
            break;
          }
      }
      break;
    }
  }
  return FALSE;
}

int CALLBACK CALLBACK MusicDialogBoxCtrl::DialogProc1(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
// обработчик сообщений диалог бокса 1
{
  act_dialog_index = 1;

  // далее точная копия кода из DialogProc0() с заменой InitDialog0() на InitDialog1() !!

  // сообщения клавиатуры здесь не появляются, они попадают в IsDialogMessage() и транслируются
  // в специальные диалоговые сообщения типа изменения фокуса ввода по клавише Tab
  switch (message)
  {
    case WM_TIMER: act_dbox().TimerMessage(); EatMessages(hwnd, message); break;

    // этот мессаг приходит ДО возврата из CreateDialog(), поэтому хендл диалог бокса берём из hwnd !!
    case WM_INITDIALOG: act_dbox().hdlg = hwnd; act_dbox().InitDialog1(); return TRUE; // ?
    // return TRUE to set the keyboard focus to the control specified by wParam, otherwise return FALSE

    case WM_HSCROLL: case WM_VSCROLL: // сообщения от любых регулируемых слайдеров
      {
        // если любой регулятор звучащего голоса передвинулся - перенажимаем аккорд
        int regen = 0, pos;

        // читаем позиции всех регуляторов слайдеров (т.е. видимых) голосов
        for (int i = 0; i < act_dbox().dialog_voices; ++i)
        {
          int pan = (*act_dbox().dvoices)[i].get_pan();
          // если панорама меняется у голоса, который был включен кнопкой диалога (enable=1), но отключен
          // флагом класса (true_enable=0), то всё равно перенажимаем аккорд, т.к. может исчезнуть причина
          // отключения флагом (напр. исчезнет конфликт с другим голосом)
          if (pan != (*act_dbox().dvoices)[i].get_pan_reg() && (*act_dbox().dvoices)[i].get_enable()) regen = 1;

          int note = (*act_dbox().dvoices)[i].get_note();
          // т.к. при изменении включённой кнопкой ноты может измениться конфликт - перенажимаем аккорд!
          if (note != (*act_dbox().dvoices)[i].get_note_reg() && (*act_dbox().dvoices)[i].get_enable()) regen = 1;
        }

        pos = act_dbox().sliders[ChainSpeed].actpos;
        if (pos != act_dbox().sliders[ChainSpeed].getpos()) regen = 1;

        pos = act_dbox().sliders[ Volume ].actpos;
        if (pos != act_dbox().sliders[ Volume ].getpos()) regen = 1;

        pos = act_dbox().sliders[Transposition].actpos;
        if (pos != act_dbox().sliders[Transposition].getpos()) regen = 1; // тут апдейтится Transposition.actpos!

        // если номер аккорда изменился - обновляем аккорд
        // при автопроигрывании секвенции длительность первого нового аккорда будет неправильной!
        pos = act_dbox().sliders[AccordNumber].actpos;
        if (pos != act_dbox().sliders[AccordNumber].getpos())
        {
          act_dbox().accord_act_index = act_dbox().sliders[AccordNumber].actpos-1; // индекс аккорда на 1 меньше позиции слайдера
          act_dbox().SetupAccord( act_dbox().accord_act_index );
          regen = 1;
        }

        act_dbox().sliders[Duration].getpos();
        act_dbox().sliders[Temp].getpos();

        if (regen) act_dbox().RegenAccord();
      }
      break;

    case WM_COMMAND: // командные сообщения Windows
      {
      UINT CtrlId = LOWORD(wparam);
      UINT CtrlMsg = HIWORD(wparam);
      switch (CtrlId)
      {
        case IDC_BUTTON_HELP: ShellExecuteOpen( AddModulePath(L"help.txt").c_str() ); break; // показать документацию

        case IDC_BUTTON_WWW: ShellExecuteOpen(website); break; // выход на WEB SITE

        case IDC_BUTTON_CLOSE: case IDCANCEL: // кнопки закрытия окна
          PostMessage(hwnd, WM_QUIT, 0, 0);
          return TRUE; // ?

        case IDC_Del_Identic3and3: act_dbox().DeleteIdenticAccords6(); break; // кнопка "Del Identic (3+3)"

        case IDC_Del_Identic2and2: act_dbox().DeleteIdenticAccords4(); break; // кнопка "Del Identic (2+2)"

        case IDC_Add_Inverse_Pan_Accords: act_dbox().AddInversePanAccords(); break; // кнопка "Add Inverse Pan"

        case IDC_Sort_Similar_Accords: // кнопка "Sort Similar Accords"
          if ( act_dbox().checks[Sort_Mode].checked_state() ) act_dbox().SortSimilarAccordsMono();
          else                                                act_dbox().SortSimilarAccords3and3();
          break;

        // кнопки файловых операций и проигрывания секвенции:
        case IDC_Save_Accord:        act_dbox().SaveAccord(0); break;
        case IDC_Append_Accord:      act_dbox().SaveAccord(1); break;
        case IDC_Save_Accords_Chain: act_dbox().SaveAccord(2); break;
        case IDC_Load_Accords: if ( act_dbox().LoadAccords(0) ) act_dbox().RegenAccord(); break;

        case IDC_Play_Stop: act_dbox().PlayStopAccords(); break;
        case IDC_Rewind:    act_dbox().RewindAccords(); break;

        case IDC_Accords_Multi_2: act_dbox().AccordsMultiplicator(2); break; // моно-фикация и размножение аккорда на 2 точки панорамы
        case IDC_Accords_Multi_3: act_dbox().AccordsMultiplicator(3); break; // моно-фикация и размножение аккорда на 3 точки панорамы

        case IDC_Enable_All_Voices: act_dbox().EnableAllVoices(); act_dbox().RegenAccord(); break; // кнопка включателей всех голосов
        case IDC_Pan_All_Voices: if (act_dbox().PanAllVoices()) act_dbox().RegenAccord(); break; // кнопка для всех панорам
        case IDC_Renumber_All_Notes: if (act_dbox().RenumberAllNotes()) act_dbox().RegenAccord(); break; // кнопка перенумерации нот
        
        // кнопки синхронного сдвига +-1 и сброса в 0 высот троек голосов 1-3, 4-6, 7-9, 10-12, 13-15
        case IDC_V1V15_0: if ( act_dbox().VoicesGroup(0, 15, 0) ) act_dbox().RegenAccord(); break;

        case IDC_V1V3_INC: if ( act_dbox().VoicesGroup(0, 3, +1) ) act_dbox().RegenAccord(); break;
        case IDC_V1V3_0:   if ( act_dbox().VoicesGroup(0, 3,  0) ) act_dbox().RegenAccord(); break;
        case IDC_V1V3_DEC: if ( act_dbox().VoicesGroup(0, 3, -1) ) act_dbox().RegenAccord(); break;

        case IDC_V4V6_INC: if ( act_dbox().VoicesGroup(3, 3, +1) ) act_dbox().RegenAccord(); break;
        case IDC_V4V6_0:   if ( act_dbox().VoicesGroup(3, 3,  0) ) act_dbox().RegenAccord(); break;
        case IDC_V4V6_DEC: if ( act_dbox().VoicesGroup(3, 3, -1) ) act_dbox().RegenAccord(); break;

        case IDC_V7V9_INC: if ( act_dbox().VoicesGroup(6, 3, +1) ) act_dbox().RegenAccord(); break;
        case IDC_V7V9_0:   if ( act_dbox().VoicesGroup(6, 3,  0) ) act_dbox().RegenAccord(); break;
        case IDC_V7V9_DEC: if ( act_dbox().VoicesGroup(6, 3, -1) ) act_dbox().RegenAccord(); break;

        case IDC_V10V12_INC: if ( act_dbox().VoicesGroup(9, 3, +1) ) act_dbox().RegenAccord(); break;
        case IDC_V10V12_0:   if ( act_dbox().VoicesGroup(9, 3,  0) ) act_dbox().RegenAccord(); break;
        case IDC_V10V12_DEC: if ( act_dbox().VoicesGroup(9, 3, -1) ) act_dbox().RegenAccord(); break;

        case IDC_V13V15_INC: if ( act_dbox().VoicesGroup(12, 3, +1) ) act_dbox().RegenAccord(); break;
        case IDC_V13V15_0:   if ( act_dbox().VoicesGroup(12, 3,  0) ) act_dbox().RegenAccord(); break;
        case IDC_V13V15_DEC: if ( act_dbox().VoicesGroup(12, 3, -1) ) act_dbox().RegenAccord(); break;

//      case IDC_MidiOutDevice: // тут не надо!

        case IDC_GM_Instrument: // действие со списком инструментов
          if (CtrlMsg == CBN_SELCHANGE) // выбран новый инструмент в списке инструментов (см также CBN_SELENDOK)
          {
            int instrument_number = act_dbox().rw_combo(GM_Instrument).GetStringIndex();
            act_dbox().SetupInstrument(instrument_number); // установим нужный номер инструмента и отобразим его на диалогпанели
            act_dbox().RegenAccord();
          }
          break;

        case IDC_CHECK_Generate_Accord: // дублируется хоткеями (Insert, Control)
          {
            act_dbox().checks[Generate_Accord].getstate();
            int gen_state = act_dbox().checks[Generate_Accord].checked_state()? 1:3;
            act_dbox().MaybeGenerateAccord(gen_state); // возможное вкл. или откл. аккорда
          }
          break;

        case IDC_CHECK_Big_Duration: act_dbox().ResetMaxDuration(); break;

        case IDC_CHECK_Notes_12:     act_dbox().checks[Notes_12].getstate(); act_dbox().RegenAccord(); break;
        case IDC_CHECK_Swap_Output:  act_dbox().checks[Swap_Output].getstate(); act_dbox().RegenAccord(); break;
        case IDC_CHECK_Smart_Voices: act_dbox().checks[Smart_Voices].getstate(); act_dbox().RegenAccord(); break;
        case IDC_CHECK_Dichotic_Output:  act_dbox().checks[Dichotic_Output].getstate(); act_dbox().RegenAccord(); break;
        case IDC_CHECK_Enable_Left_Pan:  act_dbox().checks[Enable_Left_Pan].getstate();  act_dbox().RegenAccord(); break;
        case IDC_CHECK_Enable_Mid_Pan:   act_dbox().checks[Enable_Mid_Pan].getstate();   act_dbox().RegenAccord(); break;
        case IDC_CHECK_Enable_Right_Pan: act_dbox().checks[Enable_Right_Pan].getstate(); act_dbox().RegenAccord(); break;

        case IDC_CHECK_Edit_Comment:
          act_dbox().checks[Edit_Comment].getstate();
          act_dbox().edits[Accord_Comment].readonly( act_dbox().checks[Edit_Comment].unchecked_state() );
          break;
        case IDC_CHECK_Edit_Sequence:     act_dbox().checks[Edit_Sequence].getstate(); break;
        case IDC_CHECK_AM_Octave_Shift:   act_dbox().checks[AM_Octave_Shift].getstate(); break;
        case IDC_CHECK_File_2H_Converter: act_dbox().checks[File_2H_Converter].getstate(); break;
        case IDC_CHECK_Sort_Mode:      act_dbox().checks[Sort_Mode].getstate(); break;
        case IDC_CHECK_Loop_Chain:     act_dbox().checks[Loop_Chain].getstate(); break;
        case IDC_CHECK_Dont_Change_GM_Instrument: act_dbox().checks[Dont_Change_GM_Instrument].getstate(); break;

        case IDC_Enable_Voice_1: // действия с чекбоксами включения голосов
        case IDC_Enable_Voice_2:
        case IDC_Enable_Voice_3:
        case IDC_Enable_Voice_4:
        case IDC_Enable_Voice_5:
        case IDC_Enable_Voice_6:
        case IDC_Enable_Voice_7:
        case IDC_Enable_Voice_8:
        case IDC_Enable_Voice_9:
        case IDC_Enable_Voice_10:
        case IDC_Enable_Voice_11:
        case IDC_Enable_Voice_12:
        case IDC_Enable_Voice_13:
        case IDC_Enable_Voice_14:
        case IDC_Enable_Voice_15:
          {
            int i = CtrlId - IDC_Enable_Voice_1;
            (*act_dbox().dvoices)[i].get_enable_reg(); // апдейтим изменения
            // всегда перенажимаем аккорд, т.к. какой-то голос переключился
            // (если был конфликт зашкала, то может и не так, но это не учитываем)
            act_dbox().RegenAccord();
            break;
          }
      }
      break;
    }
  }
  return FALSE;
}

void CALLBACK MusicDialogBoxCtrl::InitDialog0()
// отработка WM_INITDIALOG - подготовка всех контролов и др. данных диалогбокса 0
{
  SetTitle(); // титул диалог бокса

  // инициализируем все контролы диалог бокса 0 из массивов структур

  InitControls(check0);
  InitControls(button0);
  InitControls(slider0);
  InitControls(combo0);
  InitControls(edit0);
  InitControls(static0);

  // заполняем список midi девайсов, открываем девайс и установим инструмент во всех каналах
  InitMidiDevices(midi_device_number, midi_instrument_number);

  // заполняем список midi инструментов, установим нужный инструмент во всех каналах
  InitInstruments(midi_instrument_number);

  // загреиваем это пока не загружен аккорд или секвенция
  sliders[AccordNumber].enable(0);
  sliders[ChainSpeed].enable(0);
  checks[Loop_Chain].enable(0);
  checks[Dont_Change_GM_Instrument].enable(0);

  ResetMaxDuration(); // устанавливаем предел слайдера Duration

  // сетап голосов - в самом конце!
  InitDichoticVoices( control_id_array ); // число видимых голосов = числу контролов!

  // включаем три первых голоса и делаем аккорд до-мажор
  (*dvoices)[0].set_enable(true);
  (*dvoices)[1].set_enable(true);
  (*dvoices)[2].set_enable(true);
  (*dvoices)[0].set_note(0);
  (*dvoices)[1].set_note(4);
  (*dvoices)[2].set_note(7);
}

void CALLBACK MusicDialogBoxCtrl::InitDialog1()
{
  SetTitle(L"Second Dialog Box"); // титул диалог бокса 1 @@

  // инициализируем все контролы диалог бокса 1 из массивов структур

  InitControls(check1);
  InitControls(button1);
  InitControls(slider1);
  InitControls(combo1);
  InitControls(edit1);
  InitControls(static1);

  // заполняем список midi инструментов, установим нужный инструмент во всех каналах
  InitInstruments(midi_instrument_number);

  // загреиваем это пока не загружен аккорд или секвенция
  sliders[AccordNumber].enable(0);
  sliders[ChainSpeed].enable(0);
  checks[Loop_Chain].enable(0);
  checks[Dont_Change_GM_Instrument].enable(0);

  // в диалог боксе 1 некоторые контролы используются "в тёмную"
  ResetMaxDuration();

  // сетап голосов - в самом конце!
  InitDichoticVoices( control_id_array, 6 ); // в боксе 1 видимы и работают только 6 голосов

  // включаем три первых голоса и делаем аккорд до-минор
  (*dvoices)[0].set_enable(true);
  (*dvoices)[1].set_enable(true);
  (*dvoices)[2].set_enable(true);
  (*dvoices)[0].set_note(0);
  (*dvoices)[1].set_note(3);
  (*dvoices)[2].set_note(7);
}

