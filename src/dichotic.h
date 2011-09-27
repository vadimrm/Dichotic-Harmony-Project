

#pragma once


class DichoticVoice // дихотический голос, связанный с элементами управления (GUI)
{
  HWND hdlg; // хендл диалогового окна с элементами управления голосами

  int index; // индекс голоса (выводится на контролах) в массиве голосов: 1-й голос [0], 2-й [1] и т.д.
  int midi_channel; // номер midi канала голоса, возможно аппаратно отсутствующий (виртуальный)

  bool controls; // true: есть контролы для ручного управления голосом; false: нет, голос работает "в тёмную"
  SimpleCheckCtrl Switchon_Voice; // чекбокс включения голоса и индикации его параметров
  SliderCtrl Pan_Voice, Note_Voice; // слайдеры панорамы и номера ноты голоса

  int note; // относительный номер ноты голоса в пределах одной октавы (0...11), полутона РТС12
  int octave; // номер октавы этой note, может быть отрицательным!

  // недостигаемые крайние позиции регулятора соотв-т note = -1 и 12
  // нормальный диапазон регулировки позиции  соотв-т note =  0...11
  void set_note_reg() { if (controls) Note_Voice.setpos( note ); }

  int pan; // первичная панорама голоса: -1 слева, 0 по центру, +1 справа
  void set_pan_reg() { if (controls) Pan_Voice.setpos( pan ); }

  bool switchon; // флаг включения голоса, без учёта возможного конфликта зашкаливания
  void set_switchon_reg() { if ( controls ) Switchon_Voice.setstate( switchon? BST_CHECKED:BST_UNCHECKED ); }

public:
  DichoticVoice()
  {
    controls = false; // по умолчанию голос работает "в тёмную", без контролов
    switchon = false;
    note = 0;
    octave = 0;
    pan = 0;
  }

  void init(HWND dialog_hwnd, int Note_Voice_id, int Pan_Voice_id, int Switchon_Voice_id,
            int voice_index, int dialog_voices)
  {
    hdlg = dialog_hwnd;

    Switchon_Voice.id = Switchon_Voice_id;
    Note_Voice.id = Note_Voice_id;
    Pan_Voice.id  = Pan_Voice_id;

    index = voice_index;
    // контролы меняются только у первых dialog_voices голосов, это "видимые" голоса
    controls = (index < dialog_voices);

    int m0   = MidiInterface::MIDI_CHANNEL_MIN,          m1 = MidiInterface::MIDI_CHANNEL_MAX,
        m0nu = MidiInterface::NOT_USED_MIDI_CHANNEL_MIN, m1nu = MidiInterface::NOT_USED_MIDI_CHANNEL_MAX;

    // номера midi_channel должны начинаться с m0, обходить диапазон m0nu...m1nu и кончатся на m1
    midi_channel = index + m0;
    if (midi_channel >= m0nu) midi_channel += (m1nu-m0nu+1);
    midi_channel = min(midi_channel, m1);

    set_all_regs(true); // сетап контролов всех "видимых" голосов
  }

  int get_switchon_voice_id() const { return Switchon_Voice.id; }

  int get_midi_channel() const { return midi_channel; } // только через эту функцию midi_channel попадает наружу

  // вывод внешнего вида всех регулирующих элементов голоса согласно значению параметров
  // при наличии аргумента не по умолчанию производится также первоначальный сетап регуляторов
  void set_all_regs(bool setup=false)
  {
    if (setup)
    {
      // сетап чекбокса включения голоса, устанавливаем неинициализированные члены в нач. состояние
      // Switchon_Voice.id а также id других контролов - см. в init()
      Switchon_Voice.text_str = 0;
      // голоса по умолчанию не работают, чтобы звучали невидимые голоса надо state = 1 вместо 0 !!
      Switchon_Voice.state = 0;
      Switchon_Voice.init(hdlg);

      // сетап слайдера регулировки номера ноты
      Note_Voice.actpos = 0;
      Note_Voice.kmul = 1.; // этот член не используется...
      Note_Voice.pmin = -1;
      Note_Voice.pmax = 12;
      Note_Voice.linesz = 1;
      Note_Voice.pagesz = 12;
      Note_Voice.init(hdlg);

      // сетап слайдера регулировки панорамы
      Pan_Voice.actpos = 0;
      Pan_Voice.kmul = 1.; // этот член не используется...
      Pan_Voice.pmin = -1;
      Pan_Voice.pmax =  1;
      Pan_Voice.linesz = 1;
      Pan_Voice.pagesz = 2;
      Pan_Voice.init(hdlg);
    }

    if (!controls) return;

    set_switchon_reg();
    set_note_reg();
    set_pan_reg();
    set_title();
  }

  int get_note_reg() // определяем позицию регулятора номера ноты и апдейтим изменения
  {
    if (!controls) return get_note();
    int update = 0;
    int n = Note_Voice.getpos(); // определяем позицию регулятора

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
      set_title();
    }
    return get_note();
  }

  // делаем новый относительный номер ноты, также меняем регулятор ноты и отображаем титул голоса
  void set_note(int new_note) // new_note может быть отрицательной!
  {
    note = modulo(new_note, 12);
    octave = num_shifts(new_note, 12);
    set_note_reg();
    set_title();
  }

  int get_note() { return note + octave*12; } // выдаёт полный относительный номер ноты, может быть отрицательным!

  // не меняя внутренний номер ноты (0...11) заменяем внутренний номер октавы на oct, апдейтим титул голоса!
  void set_octave(int oct) { int n = note + oct*12; set_note(n); }

  int get_pan_reg() // определяем позицию регулятора панорамы и апдейтим изменения
  {
    if (!controls) return pan;
    int p = Pan_Voice.getpos(); // определяем позицию регулятора
    // если регулятор передвинулся меняем pan и отображаем титул
    if (pan != p)
    {
      pan = p;
      set_pan_reg();
      set_title();
    }
    return pan;
  }

  void set_pan(int new_pan) { pan = new_pan; set_pan_reg(); set_title(); }

  int get_pan() { return pan; } // панорама голоса после последнего изменения

  bool get_switchon_reg() // определяем позицию регулятора Switchon_Voice и апдейтим изменения
  {
    if (!controls) return switchon;
    bool swon = BST_CHECKED==Switchon_Voice.getstate(); // определяем регулятор
    // если регулятор изменился меняем switchon и отображаем титул
    if (switchon != swon)
    {
      switchon = swon;
      set_switchon_reg();
      set_title();
    }
    return switchon;
  }

  void switchon_note(int note, int pan = 0)
  {
    set_note(note);
    set_pan(pan);
    set_switchon(true);
  }

  void set_switchon(bool new_switchon) { switchon = new_switchon; set_switchon_reg(); set_title(); }

  bool get_switchon() { return switchon; }

  void set_title()
  {
    if (!controls) return;

    wstring2 str;
    // выводим знак панорамы
    if (get_pan() <  0) str += L"-";
    if (get_pan() == 0) str += L" ";
    if (get_pan() >  0) str += L"+";

    str << L"v" << index+1 << L" note " << get_note();

    Switchon_Voice.text( str.c_str() );
  }
};

struct SequenceHeader // общие параметры цепоцки аккордов
{
  int transposition; // абсолютный миди номер относительной ноты, равной 0
  double sequence_speed; // скорость проигрывания секвенции, нормальная = 1.0
  int dont_change_gm_instrument; // если 1, то инструмент не берётся из каждлого аккорда, а
  int instrument_number; // берётся отсюда - общий миди номер инструмента на всю секвенцию

  void clear()
  {
    transposition = 0;
    sequence_speed = 1.0;
    dont_change_gm_instrument = 0;
    instrument_number = 0;
  }
};

struct DichoticNote // одна дихотическая нота
{
  int pause; // если 1, то это не нота, а пауза!
  int note; // относительный номер ноты (м.б. отрицательный)
  int pan; // панорама: -1 левый край, 0 центр, 1 правый край
  // это добавлено в версии 302:
  int midi_note; // абсолютный миди номер ноты, устанавливается только при нажатии ноты, нужен для отпускания!
  // это добавлено в версии 303:
  int spare1; // "запасное число 1", используется обычно при сортировки...

  // возвращает true если ноты *this и note одинаковы с учётом паузы и панорамы
  bool identic_note(const DichoticNote &dn) const
  {
    if (dn.pause != pause) return false;
    if (pause) return true;
    return dn.note == note && dn.pan == pan;
  }

  static int CmpNote(const void *p1, const void *p2)
  {
    DichoticNote *n1 = (DichoticNote *)p1;
    DichoticNote *n2 = (DichoticNote *)p2;
    int res = n1->note - n2->note;
    return res;
  }

  // функция для сортировки vector<DichoticNote> по параметру spare1
  static bool Less_spare1(DichoticNote n1, DichoticNote n2) { return n1.spare1 < n2.spare1; }
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

  // макс. число голосов аккорда, которые поддерживаются (но не обязательно озвучиваются) данной версией программы
  static const int MAX_ACC_VOICES = 128; // на сегодня аппаратный максимум = 128 (для XG level 3)
  // в этом массиве должно быть место для максимально возможного числа голосов в аккорде
  DichoticNote dn[MAX_ACC_VOICES]; // дихотические ноты [0]...[voices_num-1] аккорда

  // возвращает true если все голоса *this и acc аккордов одинаковы с учётом панорамы
  bool identic_voices(const DichoticAccord &acc) const;

  bool add_note(DichoticNote note) // добавляет ноту в аккорд
  {
    if (voices_number >= MAX_ACC_VOICES) return false;
    dn[voices_number++] = note;
    return true;
  }

  void accord_manipulator(); // this аккорд пропускается через все контролы диалог бокса, влияющие на его звучание

  // аккорд транспонируется т.о. чтобы нижняя его нота стала равна base_note, возвращает false если аккорд - пауза
  bool transpose_accord(int base_note);

  // ищет минимальный номер ноты аккорда: возвращает её индекс и сам номер ноты, для паузы индекс < 0
  pair<int,int> accord_notes_min() const;

  int unique_notes_num() const; // возвращает число уникальных нот аккорда, игнорируя панораму

  // обнуляет все панорамы
  void zero_pans() { for (int i = 0; i < voices_number; ++i) dn[i].pan = 0; }

  // возвращает true если аккорд - это пауза (вариант когда voices_number!=0 но каждый голос - пауза не учитываем)
  bool is_pause() const { return (voices_number == 0); }

  void make_pause() { voices_number = 0; } // превращает аккорд в паузу

  void remove_pauses(); // удаляем все голоса-паузы, сжимая аккорд

  bool test_unisons() const; // возвращает true если в аккорде есть унисон(ы) (хоть и с разной панорамой)

  // удаляет все лишние голоса-унисоны (с любой панорамой) и голоса-паузы, сжимая аккорд
  void remove_unisons();

  // сортируем голоса аккорда в порядке возрастания номеров нот (больше голос - выше нота)
  void sort_notes() { qsort(dn, voices_number, sizeof(DichoticNote), DichoticNote::CmpNote); }

  // анализ аккорда и возврат наличия конфликтов диссонанса (совпадение нот с учётом панорамы)
  // если make_no_conflicts=true устраняет конфликты путём отключения совпадающих нот
  bool test_conflicts(bool make_no_conflicts);

  // возвращает true если все ноты 2-х аккордов и их панорамы одинаковы
  static bool identic_accords(const DichoticAccord &ac1, const DichoticAccord &ac2);

  // возвращает true если все ноты 2-х аккордов одинаковы, но имеют зеркальные панорамы (т.е. с инверсией знака)
  // причём номера голосов одинаковых нот тоже должны быть одинаковы (или они должны быть паузами голоса)
  static bool mirror_accords(const DichoticAccord &ac1, const DichoticAccord &ac2);

  // возвращает суммарный диссонанс аккорда, не совсем верный если есть совпадающие голоса: см. test_conflicts()
  int dissonance();

  void clear_comment() { comment[0] = UNI_NULL; }
  bool ok_comment() { return comment[0] != UNI_NULL; }
  // копируем src аккорд в объект без изменения исходного комментария объекта
  void copy_wo_comment(const DichoticAccord &src)
  {
    wstring2 comm( comment );
    *this = src;
    wcsncpy(comment, comm.c_str(), COMMLEN-1);
    comment[COMMLEN-1] = UNI_NULL;
  }

  // анализирует позиции звучащих голосов и записывает их ноты в 3 массива:
  // notes1[num1] для левого края панорамы, notes2[num2] для центра, notes3[num3] для правого края
  // при записи к номерам всех нот прибавляется константа add
  void NotesGetPos(int*notes1,int&num1,int*notes2,int&num2,int*notes3,int&num3,int add=0) const;

  // суммируем диссонансы всех интервалов внутри группы голосов notes[0...num-1]
  static int Dissonances(int *notes, int num);
  // суммируем диссонансы всех интервалов между двумя группами голосов
  static int CrossDissonances(int *notes1, int num1, int *notes2, int num2);

  // функция получения всех 2h вариантов панорамы одного 1h аккорда, возвращает количество вариантов
  static int DichoticConverter(DichoticAccord acc1h, vector<DichoticAccord> &acc2h);
  static int WriteCombinationPans(int N, int K, DichoticAccord acc1h, vector<DichoticAccord> &acc2h);
  static int WriteCombinationVoices(int N, int K, DichoticAccord acc1, vector<DichoticAccord> &acc2);

  // возвращает "целое значение" аккорда для функции сравнения аккордов CmpAcc() при их сортировке
  // вариант для 4 голосных аккордов!
  static int ValAcc4(const DichoticAccord &acc)
  {
    const int base = 12; // все ноты в пределах октавы 0...11, т.о. "множитель разряда" = 12
    int val = acc.dn[3].note + base*(acc.dn[2].note + base*(acc.dn[1].note + base*acc.dn[0].note));
    return val;
  }

  // возвращает "целое значение" аккорда для функции сравнения аккордов CmpAcc() при их сортировке
  // вариант для 6 голосных аккордов!
  static int ValAcc6(const DichoticAccord &acc)
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

  // функция для сортировки qsort() по параметру spare1
  static int CmpAcc2(const void *p1, const void *p2)
  {
    DichoticAccord *a1 = (DichoticAccord *)p1;
    DichoticAccord *a2 = (DichoticAccord *)p2;
    int res = a1->spare1 - a2->spare1;
    return res;
  }

  // функция для сортировки vector<DichoticAccord> по параметру spare1
  static bool Less_spare1(DichoticAccord a1, DichoticAccord a2) { return a1.spare1 < a2.spare1; }

  // сортируем ноты в acc голосов с индексами ind+0,+1 по возрастанию
  static void Sort2notes(DichoticAccord &acc, int ind) { sort2(acc.dn[ind].note, acc.dn[ind+1].note); }

  // сортируем ноты в acc голосов с индексами ind+0,+1,+2 по возрастанию
  static void Sort3notes(DichoticAccord &acc, int ind) { sort3(acc.dn[ind].note, acc.dn[ind+1].note, acc.dn[ind+2].note); }
};

