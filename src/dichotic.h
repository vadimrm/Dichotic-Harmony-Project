

#pragma once


class DichoticVoice // дихотический голос, связанный с элементами управления (GUI)
{
  HWND hwnd; // хендл диалогового окна с элементами управления голосами

  int index; // индекс голоса (выводится на контролах) в массиве голосов: 1-й голос [0], 2-й [1] и т.д.
  int midi_channel; // номер midi канала голоса, возможно аппаратно отсутствующий (виртуальный)

  int Note_id, Pan_id, Enable_id; // идентификаторы элементов управления данного голоса
  bool okid; // true: есть элементы управления голосом; false: нет, голос работает "в тёмную"

  int note; // относительный номер ноты голоса в пределах одной октавы (0...11), полутона РТС12
  int octave; // номер октавы этой note, может быть отрицательным!

  bool smart_voices; // от этого флага зависит вывод совпадающих номеров нот

  // недостигаемые крайние позиции регулятора соотв-т note = -1 и 12
  // нормальный диапазон регулировки позиции  соотв-т note =  0...11
  void set_note_reg() { if (okid) SendDialogItemMessage(hwnd, Note_id, TBM_SETPOS, 1, note); }

  int pan; // первичная панорама голоса: -1 слева, 0 по центру, +1 справа
  void set_pan_reg() { if (okid) SendDialogItemMessage(hwnd, Pan_id, TBM_SETPOS, 1, pan); }

  bool enable; // включатель голоса, без учёта возможного конфликта зашкаливания
  void set_enable_reg()
  { 
    if ( okid && IsDialogItem(hwnd, Enable_id) ) CheckDlgButton(hwnd, Enable_id, enable? BST_CHECKED:BST_UNCHECKED);
  }

public: // ====================================
  bool conflict_scale; // флаг конфликта зашкаливания голоса за границы миди нот (0...127)
  bool conflict_diss;  // флаг конфликта определения диссонанса голоса из-за наличия 2-х и более
                       // одинаковых нот в одном и том же канале вывода звука (левом, правом)
  bool true_enable; // флаг включения голоса с учётом конфликта зашкаливания
  int midi_note; // абсолютный миди номер ноты в момент включения голоса

  // по умолчанию голос голос работает "в тёмную", без контролов
  DichoticVoice() : conflict_scale(0), conflict_diss(0)
  {
    okid = false; // голоса по умолчанию не отображаются
    set_note(0);
    set_pan(0);
    // голоса по умолчанию не работают!
    set_enable(0); // чтобы звучали невидимые голоса надо поставить 1 вместо 0 !!
  }

  void init(HWND dialog_hwnd,int note_id,int pan_id,int enable_id,int voice_index,int dialog_voices,bool need_smart_voices)
  {
    hwnd = dialog_hwnd;
    Note_id = note_id;
    Pan_id = pan_id;
    Enable_id = enable_id;
    index = voice_index;
    int m0   = MidiInterface::MIDI_CHANNEL_MIN,          m1 = MidiInterface::MIDI_CHANNEL_MAX,
        m0nu = MidiInterface::NOT_USED_MIDI_CHANNEL_MIN, m1nu = MidiInterface::NOT_USED_MIDI_CHANNEL_MAX;

    // номера midi_channel должны начинаться с m0, обходить диапазон m0nu...m1nu и кончатся на m1
    midi_channel = index + m0;
    if (midi_channel >= m0nu) midi_channel += (m1nu-m0nu+1);
    midi_channel = min(midi_channel, m1);

    // контролы меняются только у первых dialog_voices голосов, это "видимые" голоса
    okid = (index < dialog_voices);

    smart_voices = need_smart_voices;
    set_all_regs(true); // сетап контролов всех "видимых" голосов
  }

  int get_midi_channel() { return midi_channel; } // только через эту функцию midi_channel попадает наружу

  // вывод внешнего вида всех регулирующих элементов голоса согласно значению параметров
  // при наличии аргумента не по умолчанию производится также первоначальный сетап регуляторов
  void set_all_regs(bool setup=false)
  {
    if (!okid) return;
    if (setup)
    {
      // сетап слайдера регулировки номера ноты
      SendDialogItemMessage(hwnd, Note_id, TBM_SETRANGE,    1, MAKELONG(-1, 12)); // -1, 12
      SendDialogItemMessage(hwnd, Note_id, TBM_SETLINESIZE, 0, 1);
      SendDialogItemMessage(hwnd, Note_id, TBM_SETPAGESIZE, 0, 12);

      // сетап слайдера регулировки панорамы
      SendDialogItemMessage(hwnd, Pan_id, TBM_SETRANGE,    1, MAKELONG(-1, 1));
      SendDialogItemMessage(hwnd, Pan_id, TBM_SETLINESIZE, 0, 1);
      SendDialogItemMessage(hwnd, Pan_id, TBM_SETPAGESIZE, 0, 2);
    }
    set_note_reg();
    set_pan_reg();
    set_enable_reg();
    set_title(smart_voices);
  }

  int get_note_reg() // определяем позицию регулятора ноты и апдейтим изменения
  {
    if (!okid) return get_note();
    int update = 0;
    int n = SendDialogItemMessage(hwnd, Note_id, TBM_GETPOS, 0, 0); // определяем позицию регулятора
    if (n == -1) // перемещение в левую октаву
    {
      note = 11;
      testmin(--octave, -9);
      update = 1;
    }
    else
    if (n == 12) // перемещение в правую октаву
    {
      note = 0;
      testmax(++octave, 9);
      update = 1;
    }
    else // n = 0...11
    if (note != n)
    {
      note = n; 
      update = 1;
    }
    // если регулятор передвинулся меняем ноту и отображаем титул
    if (update)
    {
      set_note_reg();
      set_title(smart_voices);
    }
    return get_note();
  }

  // уст-ка новой ноты, также меняем регулятор ноты и отображаем титул голоса
  void set_note(int new_note) // new_note может быть отрицательной!
  {
    note = modulo(new_note, 12);
    octave = num_shifts(new_note, 12);
    set_note_reg();
    set_title(smart_voices);
  }

  int get_note() { return note + octave*12; } // полный относительный номер ноты

  // не меняя внутренний номер ноты (0...11) заменяем внутренний номер октавы на oct, апдейтим титул голоса!
  void set_octave(int oct) { int n = note + oct*12; set_note(n); }

  int get_pan_reg() // определяем позицию регулятора pan и апдейтим изменения
  {
    if (!okid) return pan;
    int p = SendDialogItemMessage(hwnd, Pan_id, TBM_GETPOS, 0, 0); // определяем позицию регулятора
    // если регулятор передвинулся меняем pan и отображаем титул
    if (pan != p)
    {
      pan = p;
      set_pan_reg();
      set_title(smart_voices);
    }
    return pan;
  }

  void set_pan(int new_pan) { pan = new_pan; set_pan_reg(); set_title(smart_voices); }

  int get_true_pan(bool not_dichotic_output, bool swap_output) // панорама голоса с учетом общих манипуляторов
  {
    int true_pan = pan;
    // меняем панораму
    if ( not_dichotic_output ) true_pan = 0;
    if ( swap_output )         true_pan = -true_pan;
    return true_pan;
  }

  int get_pan() { return pan; } // панорама без учёта общих манипуляторов

  bool get_enable_reg() // определяем позицию регулятора enable и апдейтим изменения
  {
    if (!okid) return enable;
    bool ena = BST_CHECKED==IsDlgButtonChecked(hwnd, Enable_id); // определяем регулятор
    // если регулятор изменился меняем enable и отображаем титул
    if (enable != ena)
    {
      enable = ena;
      set_enable_reg();
      set_title(smart_voices);
    }
    return enable;
  }

  void set_enable(bool new_enable) { enable = new_enable; set_enable_reg(); set_title(smart_voices); }

  bool get_enable() { return enable; }

  void set_title(bool need_smart_voices)
  {
    if (!okid) return;
    smart_voices = need_smart_voices;

    wstring2 str(L"v");
    str << index+1 << L" note " << get_note();
    // выводим панораму БЕЗ учёта общих манипуляторов
    if (get_pan() < 0) str += L"-";
    if (get_pan() > 0) str += L"+";
    // если есть conflict_scale, то знак флага conflict_diss не выводится, т.к. голос не звучит!
    if (conflict_scale) str += L" X";
    if (conflict_diss)  str += smart_voices? L" X":L" ?";
    SendDialogItemMessage( hwnd, Enable_id, WM_SETTEXT, 0, (LPARAM)str.c_str() );
  }
};

struct ChainHeader // общие параметры цепоцки аккордов
{
  int transposition; // абсолютный миди номер относительной ноты, равной 0
  double chain_speed; // скорость проигрывания секвенции, нормальная = 1.0
  int dont_change_gm_instrument; // если 1, то инструмент не берётся из каждлого аккорда, а
  int instrument_number; // берётся отсюда - общий миди номер инструмента на всю секвенцию
};

struct DichoticNote // одна дихотическая нота
{
  int pause; // если 1, то это не нота, а пауза!
  int note; // относительный номер ноты
  int pan; // панорама: -1 левый, 0 центр, 1 правый край

  static int CmpNote(const void *p1, const void *p2)
  {
    DichoticNote *n1 = (DichoticNote *)p1;
    DichoticNote *n2 = (DichoticNote *)p2;
    int res = n1->note - n2->note;
    return res;
  }
};

struct DichoticAccord // структура дихотического аккорда 
{
  static const int COMMLEN = 16;
  int instrument; // номер midi инструмента (ранее был timbre - тембр)
  int duration; // длительность в ударах метронома
  int temp; // параметр темпа, число ударов метронома в секунду
  int voices_number; // количество голосов в аккорде, 1...MAX_ACC_VOICES, если 0 - это пауза
  wchar_t comment[COMMLEN]; // слово "комментария" к аккорду, завершающееся нулём, создаётся вручную в файле
  int spare1; // "запасное число 1", используется обычно как параметр сортировки...

  // если пауза, то эти параметры могут отсутствовать:
  int volume; // громкость, 1...127
  // в этом массиве должно быть место для максимально возможного числа голосов в аккорде
  DichoticNote dn[MusicDialogBoxCtrl::MAX_ACC_VOICES]; // дихотические ноты [0]...[voices_num-1] аккорда

  void clear_comment() { comment[0] = UNI_NULL; }
  bool ok_comment() { return comment[0] != UNI_NULL; }
  // копируем src аккорд в объект без изменения исходного комментария объекта
  void copy_wo_comment(DichoticAccord &src)
  {
    wstring2 comm( comment );
    *this = src;
    wcsncpy(comment, comm.c_str(), COMMLEN-1);
    comment[COMMLEN-1] = UNI_NULL;
  }

  // возвращает "целое значение" аккорда для функции сравнения аккордов CmpAcc() при их сортировке
  // вариант для 4 голосных аккордов!
  static int ValAcc4(DichoticAccord &acc)
  {
    const int base = 12; // все ноты в пределах октавы 0...11, т.о. "множитель разряда" = 12
    int val = acc.dn[3].note + base*(acc.dn[2].note + base*(acc.dn[1].note + base*acc.dn[0].note));
    return val;
  }

  // возвращает "целое значение" аккорда для функции сравнения аккордов CmpAcc() при их сортировке
  // вариант для 6 голосных аккордов!
  static int ValAcc6(DichoticAccord &acc)
  {
    const int base = 12; // все ноты в пределах октавы 0...11, т.о. "множитель разряда" = 12
    int val = acc.dn[5].note + base*(acc.dn[4].note + base*(acc.dn[3].note +
              base*(acc.dn[2].note + base*(acc.dn[1].note + base* acc.dn[0].note))));
    return val;
  }

  static int CmpAcc(const void *p1, const void *p2)
  {
    DichoticAccord *a1 = (DichoticAccord *)p1;
    DichoticAccord *a2 = (DichoticAccord *)p2;
    int voices = a1->voices_number;
    if (voices == 4) return ValAcc4(*a1) - ValAcc4(*a2);
    if (voices == 6) return ValAcc6(*a1) - ValAcc6(*a2);
    return 0;
  }

  static int CmpAcc2(const void *p1, const void *p2)
  {
    DichoticAccord *a1 = (DichoticAccord *)p1;
    DichoticAccord *a2 = (DichoticAccord *)p2;
    int res = a1->spare1 - a2->spare1;
    return res;
  }

  // сортируем ноты в acc голосов с индексами ind+0,+1 по возрастанию
  static void Sort2notes(DichoticAccord &acc, int ind) { sort2(acc.dn[ind].note, acc.dn[ind+1].note); }

  // сортируем ноты в acc голосов с индексами ind+0,+1,+2 по возрастанию
  static void Sort3notes(DichoticAccord &acc, int ind) { sort3(acc.dn[ind].note, acc.dn[ind+1].note, acc.dn[ind+2].note); }
};

