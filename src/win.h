
#pragma once

#include "win_tools.h"

enum CheckCtrlIndex { Generate_Accord, Enable_Left_Pan, Enable_Mid_Pan, Enable_Right_Pan, Loop_Chain, 
  Edit_Sequence, AM_Octave_Shift, File_2H_Converter, Sort_Mode, Big_Duration, Notes_12,
  Dont_Change_GM_Instrument, Edit_Comment, Smart_Voices, Dichotic_Output, Swap_Output, };

enum ButtonCtrlIndex { PlayStop, Rewind, SaveAccordsChain, };

enum SliderCtrlIndex { AccordNumber, ChainSpeed, Volume, Transposition, Duration, Temp, };

enum ComboBoxCtrlIndex { GM_Instrument, MidiOutDevice, };

enum EditCtrlIndex { Accord_Comment, };

enum StaticCtrlIndex { Left_Notes, Mid_Notes, Right_Notes, Total_Diss, Uniq_Notes, };


inline bool IsDialogItem(HWND dialog_hwnd, int item_id)
{
  HWND item_hwnd = GetDlgItem(dialog_hwnd, item_id);
  return item_hwnd != NULL;
}

inline bool DialogItemEnable(HWND dialog_hwnd, int item_id, bool enable = true)
{
  if ( !IsDialogItem(dialog_hwnd, item_id) ) return false;
  HWND item_hwnd = GetDlgItem(dialog_hwnd, item_id);
  EnableWindow(item_hwnd, enable);
  return true;
}

inline LRESULT SendDialogItemMessage(HWND dialog_hwnd, int item_id, UINT msg, WPARAM wp, LPARAM lp)
{
  if ( IsDialogItem(dialog_hwnd, item_id) ) // если контрол есть
    return SendDlgItemMessage(dialog_hwnd, item_id, msg, wp, lp);
  return 0;
}


struct SliderCtrl // (слайдер контрол + 2 текст контрола) диалог бокса
{
  // мин/макс позиции слайдера (включая отрицательные), "строчный" и "страничный" шаг
  int min, max, linesz, pagesz;
  int id;  // идентификатор слайдера
  int id_text; // идентификатор строки текста для вывода текущей позиции слайдера в виде числа
  const wchar_t *prefix; // начало строки текста, после которого выводится числовое значение регулируемой величины
  int actpos; // текущая позиция слайдера
  double kmul; // коэффициент, на который надо умножить текущую позицию слайдера для получения регулируемой величины
  HWND hdlg; // хендл окна диалог-бокса, внутри которого сидят элементы

  void init(HWND dialog_hwnd)
  {
    hdlg = dialog_hwnd;
    if ( IsDialogItem(hdlg, id) ) // если контрол есть
    {
      set_minmax();
      SendDialogItemMessage(hdlg, id, TBM_SETLINESIZE, 0, linesz);
      SendDialogItemMessage(hdlg, id, TBM_SETPAGESIZE, 0, pagesz);
      SendDialogItemMessage(hdlg, id, TBM_SETPOS,      1, actpos);
      update_text();
    }
  }

  void reset_minmax(int new_min, int new_max) { min = new_min; max = new_max; set_minmax(); }
  void set_minmax() { SendDialogItemMessage(hdlg, id, TBM_SETRANGE, 1, MAKELONG(min, max)); }

  void enable(bool enable)
  {
    enable_item(enable, id);
    enable_item(enable, id_text);
  }
  void enable_text(bool enable) { enable_item(enable, id_text); }

  int setpos(int pos)
  {
    if ( IsDialogItem(hdlg, id) ) // если контрол есть
    {
      SendDialogItemMessage(hdlg, id, TBM_SETPOS, 1, pos);
      actpos = getpos();
      update_text();
    }
    else // эквивалентный код для невидимых контролов, id которых есть в массиве, но нет на диалог боксе
    {
      actpos = minxmax(min, pos, max);
    }
    return actpos;
  }

  int getpos() // выдаёт позицию слайдера, но не регулируемую величину (если kmul!=1.)
  {
    if ( IsDialogItem(hdlg, id) )
    {
      // как правило эта ф-я вызывается при изменении позиции слайдера, поэтому меняем и текст
      actpos = SendDialogItemMessage(hdlg, id, TBM_GETPOS, 0, 0);
      update_text();
    }
    return actpos;
  }

  // преобразование позиции слайдера в регулируемую величину
  double get_reg_value() { return kmul*actpos; }

  // преобразование регулируемой величины в позицию слайдера
  int get_pos_value(double reg_value) { return float2int(reg_value/kmul); }

  void enable_item(bool enable, int item_id) { DialogItemEnable(hdlg, item_id, enable); }
  void update_text() // выводим текст согласно текущей позиции слайдера
  {
    if ( IsDialogItem(hdlg, id) ) // если контрол есть
    {
      wstring2 str(prefix);
      str << get_reg_value();
      SendDialogItemMessage(hdlg, id_text, WM_SETTEXT, 0, LPARAM(str.c_str()));
    }
  }
};


struct CheckCtrl // чекбокс контрол диалог бокса, в т.ч TRI-STATE (3-state)
{
  int id; // идентификатор контрола, присваивается в объявлении объекта конструктором копирования
  int state; // текущее состояние чекбокса BST_UNCHECKED, BST_CHECKED, BST_INDETERMINATE (=0,1,2)
             // оно меняется произвольно только в обработчике сообщений диалогового окна,
             // а в остальном коде может быть считано из этой переменной
  const wchar_t *name[3]; // тексты контрола в состоянии state (0,1,2)

  HWND hdlg; // хендл окна диалогбокса
  // если name[0] нет, текст контрола вообще не устанавливается (остаётся таким как в ресурсах)!
  // если есть name[0], но нет name[1], то текст устанавливается один раз, а затем не меняется!
  int name_num; // 0, 1, 2, 3 - количество текстов контрола

  void init(HWND dialog_hwnd) // инициализация контрола
  {
    hdlg = dialog_hwnd;

    bool null_str = false;
    name_num = 3; // если все тексты есть - в цикле name_num не изменится
    for (int i = 0; i < 3; ++i)
    {
      // на вс. случай обнуляем указатели на остальные строки имён
      if (null_str) name[i] = UNI_NULL_STR;
      // ищем пустую строку имени с минимальным индексом
      if ( !null_str && 0 == wcscmp(name[i], UNI_NULL_STR) )
      {
        null_str = true;
        name_num = i;
      }
    }

    // уст. первоначального состояния и вывод текста контрола
    setstate(state, true);
  }

  void setstate(int check_state, bool setup_text = false)
  // уст. состояния и вывод текста контрола
  { 
    state = minxmax(0, check_state, 2);
    if ( IsDialogItem(hdlg, id) ) // если контрол есть
    {
      CheckDlgButton(hdlg, id, check_state);
      state2text(setup_text);
    }
  }

  void state2text(bool setup_text = false)
  // если setup_text = true делаем (первоначальную) установку текста контрола
  // всегда делаем установку текста контрола, если он переменный (name_num >= 2)
  // никогда не делаем установку текста если его нет (name_num = 0)
  { 
    if (name_num == 0) setup_text = false;
 // if (name_num == 1) setup_text не меняется!
    if (name_num >= 2) setup_text = true;
    if (setup_text) text( name[state] );
  }

  // эта функция используется для определения состояния контрола в момент его смены "вручную",
  // поэтому меняем также текст контрола!
  int getstate()
  {
    if ( IsDialogItem(hdlg, id) ) // если контрол есть
    {
      state = IsDlgButtonChecked(hdlg, id);
      state2text();
    }
    return state; // 0,1,2 = BST_UNCHECKED, BST_CHECKED, BST_INDETERMINATE
  }

  int getstate3() const { return state; } // 0,1,2 = BST_UNCHECKED, BST_CHECKED, BST_INDETERMINATE
  bool checked_state() const { return state == BST_CHECKED; }
  bool unchecked_state() const { return state == BST_UNCHECKED; }

  void set_checked_state(bool checked) { setstate( checked? BST_CHECKED:BST_UNCHECKED ); }
  void enable(bool enable) { DialogItemEnable(hdlg, id, enable); }
  void text(const wchar_t *str) { SendDialogItemMessage(hdlg, id, WM_SETTEXT, 0, LPARAM(str)); }
  void inverse_state() { set_checked_state( !checked_state() ); }
};


struct ButtonCtrl // Button контрол диалог бокса, в.ч. триггерные би-кнопки (2 состояния)
{
  int id; // идентификатор контрола
  const wchar_t *name0; // неизменный текст контрола, для би-кнопок - текст в состоянии 0
  const wchar_t *name1; // текст би-кнопок в состоянии 1
  int func_state; // функциональное состояние 0/1
  bool enable_state; // 0 загреенная кнопка
  HWND hdlg;

  void init(HWND dialog_hwnd)
  {
    hdlg = dialog_hwnd;
    // делаем нужное загреивание и выводим текст контрола
    enable();
    button_text();
  }

  void text(const wchar_t *str) { SendDialogItemMessage(hdlg, id, WM_SETTEXT, 0, LPARAM(str)); }
  void button_text() { func_state==0? text(name0):text(name1); }
  void button_text(int new_func_state)
  {
    func_state = new_func_state;
    button_text();
  }
  
  void enable() { DialogItemEnable(hdlg, id, enable_state); }
  void enable(bool new_enable_state)
  {
    enable_state = new_enable_state;
    enable();
  }
};


struct EditCtrl // Edit контрол диалог бокса
{
  int id;
  bool readonly_state; // 1 ридонли редактор
  bool enable_state; // 0 неактивный контрол
  HWND hdlg;

  void init(HWND dialog_hwnd)
  {
    hdlg = dialog_hwnd;
    // делаем нужные состояния контрола
    readonly();
    enable();
  }

  void set_text(const wchar_t *str) { SendDialogItemMessage(hdlg, id, WM_SETTEXT, 0, LPARAM(str)); }
  void get_text(wchar_t *str, int maxlen) const
  {
    str[0] = str[maxlen-1] = UNI_NULL;
    SendDialogItemMessage(hdlg, id, WM_GETTEXT, maxlen, LPARAM(str));
  }

  void readonly() { SendDialogItemMessage(hdlg, id, EM_SETREADONLY, readonly_state, 0); }
  void readonly(bool new_readonly_state)
  {
    readonly_state = new_readonly_state;
    readonly();
  }

  void enable() { DialogItemEnable(hdlg, id, enable_state); }
  void enable(bool new_enable_state)
  {
    enable_state = new_enable_state;
    enable();
  }
};


struct ComboBoxCtrl // комбобокс контрол диалог бокса
{
  int id;
  HWND hdlg;
  int index;

  void init(HWND dialog_hwnd) { hdlg = dialog_hwnd; }

  void AddString(const wchar_t *str) { SendDialogItemMessage(hdlg, id, CB_ADDSTRING, 0, (LPARAM)str); }

  void SetStringIndex(int act_index) // выбирается строка с индексом act_index >= 0
  {
    index = act_index;
    SendDialogItemMessage(hdlg, id, CB_SETCURSEL, index, 0);
  }

  int GetStringIndex() // определяется индекс выбранной строки, >=0
  {
    if ( IsDialogItem(hdlg, id) ) index = SendDialogItemMessage(hdlg, id, CB_GETCURSEL, 0, 0);
    return index;
  }
};


struct StaticCtrl // статик контрол диалог бокса
{
  int id; // идентификатор контрола
  const wchar_t *text0; // текст контрола или его начало...
  HWND hdlg;

  void init(HWND dialog_hwnd)
  {
    hdlg = dialog_hwnd;
    text(text0);
  }
  // выводим str
  void text(const wchar_t *str) { SendDialogItemMessage(hdlg, id, WM_SETTEXT, 0, LPARAM(str)); }

  void add_text(const wchar_t *str) // выводим (text0 + str)
  {
    wstring str2 = text0;
    str2 += str;
    text( str2.c_str() );
  }
};


// обобщённый диалог бокс для нужд любой программы
class DialogBoxCtrl
{
  int id; // (ресурсный) идентификатор диалог бокса

  static int dialog_count; // общий счетчик класса для первоначального определения индекса экземпляра
protected:
  int dialog_index; // индекс экземпляра диалогбокса: 0, 1, 2 ... пока не используется!
  HWND hdlg; // хендл окна бокса

  // контролы бокса
  Ar <CheckCtrl> checks;
  Ar <SliderCtrl> sliders;
  Ar <ButtonCtrl> buttons;
  Ar <EditCtrl> edits;
  Ar <ComboBoxCtrl> combos;
  Ar <StaticCtrl> statics;

public:
  DialogBoxCtrl() { dialog_index = dialog_count++; }

  ~DialogBoxCtrl() { if (hdlg != 0) DestroyWindow (hdlg); }

  bool Create(HINSTANCE hInstance, WORD wInteger, HWND hWndParent, DLGPROC lpDialogFunc)
  {
    id = wInteger;
    hdlg = CreateDialog(hInstance, MAKEINTRESOURCE(wInteger), hWndParent, lpDialogFunc);
    return hdlg != 0;
  }

  // любые манипуляции с контролами делаются через эти функции
  CheckCtrl&    rw_check(int index)  { return checks[index]; }
  SliderCtrl&   rw_slider(int index) { return sliders[index]; }
  ButtonCtrl&   rw_button(int index) { return buttons[index]; }
  EditCtrl&     rw_edit(int index)   { return edits[index]; }
  ComboBoxCtrl& rw_combo(int index)  { return combos[index]; }
  StaticCtrl&   rw_static(int index) { return statics[index]; }

  // константные манипуляции с контролами делаются через эти функции
  const CheckCtrl    get_check(int index)  const { return checks[index]; }
  const SliderCtrl   get_slider(int index) const { return sliders[index]; }
  const ButtonCtrl   get_button(int index) const { return buttons[index]; }
  const EditCtrl     get_edit(int index)   const { return edits[index]; }
  const ComboBoxCtrl get_combo(int index)  const { return combos[index]; }
  const StaticCtrl   get_static(int index) const { return statics[index]; }

  void Show() { if (hdlg != 0) ShowWindow(hdlg, SW_SHOW); }

  HWND hwnd() const { return hdlg; }

  bool IsDialogMsg(LPMSG lpMsg) { return 0 != IsDialogMessage(hdlg, lpMsg); }

  LRESULT SendItemMessage(int item_id, UINT msg, WPARAM wp, LPARAM lp)
  {
    return SendDialogItemMessage(hdlg, item_id, msg, wp, lp);
  }

  // создаем титульную строку диалог бокса из строки str
  void SetTitle(const wchar_t *str) { SetWindowText(hdlg, str); }

  // инициализируем контролы бокса из массива контролов, передаём в них хендл диалог бокса...
  template<class T, int ctrl_numbers> bool InitControls(const T (&ctrl_array)[ctrl_numbers]);
  // размер массива определяется автоматически!
};


// конкретный диалог бокс для нужд данной программы: встраиваем в него только одинаковые функции
// для всех боксов, остальные должны быть внешними (возможно друзьями)!
class MusicDialogBoxCtrl : public DialogBoxCtrl
{
  static const int timer_msec = 10; // мсек, период генерации WM_TIMER, точность времени нот! < 10 улучшений не даёт!

  static const int MAX_ACCORDS = 5000; // размер массива аккордов accords_chain (макс. число аккордов в секвенции)
  Ar <struct DichoticAccord> *accords_chain;

  int accords_in_chain; // количество аккордов в загруженной секвенции
  int accord_act_index; // текущий индекс проигрываемого (и редактируемого) аккорда секвенции
  int play_accords_chain; // флаг проигрывания секвенции: 0=стоп, 1=старт, 2=продолжение проигрывания

  // флаг для правильного "перенажатия" аккорда (нужен из-за наличия и кнопки и хоткея генерации)
  int gen_state; // 0 нет генерации, 1 начало генерации, 2 генерация, 3 конец генерации аккорда

  int dialog_voices; // число голосов с контролами на диалогбоксе, остальные голоса будут "невидимы"
  Ar <class DichoticVoice> *dvoices; // голоса 2H аккорда, максимальное количество MAX_ACC_VOICES

  friend int WINAPI WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR Cmd, int CmdShow);

public:
  // макс. число голосов аккорда, которые поддерживаются (но не обязательно озвучиваются) данной версией программы
  static const int MAX_ACC_VOICES = 128; // на сегодня аппаратный максимум = 128 (для XG level 3)
  static const int MAX_INTERVAL = 128; // 1+максимальный интервал нот аккорда, полутонов РТС12
  static int diss_rating[MAX_INTERVAL]; // массив диссонансов для каждого интервала созвучия

  MusicDialogBoxCtrl()
  {
    accords_in_chain = 0;
    accord_act_index = 0;
    play_accords_chain = 0;
    gen_state = 0;
    accords_chain = new Ar <DichoticAccord> (MAX_ACCORDS);
    dvoices = new Ar <DichoticVoice> (MAX_ACC_VOICES);
  }

  ~MusicDialogBoxCtrl() { delete accords_chain; delete dvoices; }

  void InitInstruments(int instrument_number); // заполняем список инструментов + SetupInstrument()
  void SetupInstrument(int instrument_number); // установим номер инструмента во всех каналах
  // заполняем список midi девайсов, открываем девайс и установим инструмент во всех каналах
  void InitMidiDevices(int device_number, int instrument_number);

  // инициализируем голоса dvoices из массива идентификаторов их контролов [number][Note_id, Pan_id, Enable_id]
  // при max_number == 0 используется весь массив, иначе используется не более max_number его членов
  template<int number> void InitDichoticVoices(const int (&id_array)[number][3], int max_number = 0);

  // какая из этих функций будет работать для конкретного экземпляра бокса определяется в DialogBoxCtrl::Create() 
  static int CALLBACK DialogProc0(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp); // обработчик сообщений диалог бокса 0
  static int CALLBACK DialogProc1(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp); // --//-- диалог бокса 1

  void CALLBACK InitDialog0(); // отработка WM_INITDIALOG - подготовка всех контролов и др. диалог бокса 0
  void CALLBACK InitDialog1(); // --//-- диалог бокса 1

  // суммируем диссонансы всех интервалов внутри группы голосов notes[0...num-1]
  static int Dissonances(int *notes, int num);
  // суммируем диссонансы всех интервалов между двумя группами голосов
  static int CrossDissonances(int *notes1, int num1, int *notes2, int num2);

  static void LoadData(); // грузим параметры программы из файлов

  // выводим в титуле диалог бокса название файла с расширением, считанного при load=true, или записанного
  void PrintFileTitle(const wchar_t *filename, const wchar_t *ext, bool load);

  // создаем титульную строку диалог бокса из строки str, а если она пустая - то из титула программы!
  void SetTitle(const wchar_t *str = UNI_NULL_STR);

  inline double AccordDtime(); // длительность аккорда в секундах с учётом всех регуляторов

  // загружаем секвенцию из файла, тип которого зависит от format:
  // 0: родной .daccords формат, 1: .PAS формат файлов редактора MUSE
  // если есть строка file, то формат 0 грузим молча из этого файла!
  bool LoadAccords( int format, const wchar_t *file = UNI_NULL_STR );

  // аккорд секвенции AccordNumber становится 1-м, а остальные аккорды выстраиваются по степени
  // совпадения с ним (количество совпадающих нот в монофоническом варианте), без записи в файл!
  // все аккорды должны быть N-голосными (N = const, N >= 2) и не должны иметь голоса-паузы...
  void SortSimilarAccordsMono();

  // работает примерно как SortSimilarAccordsMono(), но отличия определяет по (дихотическим)
  // половинкам 6-голосных аккордов:
  // сравнивает ноты голосов 0-2 и 3-5, выбирая суммарно-минимально отличные комбинации половинок!
  void SortSimilarAccords3and3();

  // добавляет после каждого аккорда точно такой же, но с инверсной панорамой!
  void AddInversePanAccords();

  // функция для обработки "сырых" 6 голосных дихотик-секвенций (3+3голоса): 
  // отбрасывание лишних совпадающих аккордов и сортировка в порядке увеличения нот, запись в файл!
  void DeleteIdenticAccords6();

  // функция для обработки "сырых" 4 голосных (или 5 голосных) дихотик-секвенций (2+2голоса): 
  // отбрасывание лишних совпадающих аккордов и сортировка в порядке увеличения нот, запись в файл!
  // если число голосов равно 5, то сначала средний голос отбрасывается (считается что в нём пауза)
  void DeleteIdenticAccords4();

  // моно-фикация и размножение всех аккордов секвенции на points=2 или 3 точки панорамы с октавным сдвигом
  void AccordsMultiplicator(int points);

  // при mode=0 (Save_Accord       ) записываем текущий аккорд в файл, если файл есть - стираем и перезаписываем его
  // при      1 (Append_Accord     ) добавляем аккорд к сушествующему файлу, а если файла нет - пишем его с нуля!
  // при      2 (Save_Accords_Chain) записываем секвенцию аккордов в файл, если файл есть - стираем и перезаписываем его
  // при      3 (запись секвенции после сортировки) то же что и при 2, кроме замены текущего аккорда!
  // при (2) сначала текущий звучащий аккорд копируется в текущее место секвенции, т.о. возможно её редактирование!
  // при (0,1,3) секвенцию не меняем!
  // если чекбокс Edit_Sequence не выбран - секвенцию не меняем и при mode=2!
  void SaveAccord(int mode);

  // кнопка циклического переключения КНОПОК всех панорам лево-центр-право
  // возвращает true если хотя бы один голос был включен (звучал)
  bool PanAllVoices();

  // кнопка перенумерации нот ВКЛЮЧЕННЫХ голосов (всех, а не только диалоговых) чтобы мин. относит. нота стала = 0
  // с соотв-щим изменением общего номера 0-й ноты (транспозиции)
  // возвращает true если хотя бы один голос был включен (звучал)
  bool RenumberAllNotes();

  // кнопка сброса включателей всех голосов в откл., затем циклически включение-выключение первых 6 голосов
  void EnableAllVoices();

  // в группе голосов делаем одинаковую (РУЧНУЮ!) операцию: -1, +1 сдвиг высоты; 0 сброс высоты в 0
  // возвращает true если хотя бы один голос группы был включен (звучал)
  bool VoicesGroup(int first_voice, int voices_num, int operation);

  void TimerMessage(); // обработчик сообщений таймера
  void MaybeGenerateAccord(int new_state); // возможное вкл. или откл. аккорда
  void RegenAccord(); // если аккорд звучал - перенажимаем аккорд
  void RewindAccords(); // копируем начальный аккорд секвенции в аккорд на диалогпанели
  void PlayStopAccords(); // включаем или выключаем аккорд
  void GenerateAccord(); // включаем или выключаем аккорд
  void SetupAccord(int index); // копируем аккорд из массива с индексом index в аккорд на диалогпанели
  void ResetMaxDuration(); // устанавливаем предел слайдера Duration

  // анализирует позиции звучащих голосов и записывает их ноты в 3 массива:
  // notes1[num1] для левого края панорамы, notes2[num2] для центра, notes3[num3] для правого края
  // также анализирует недопустимое совпадение номеров нот в одном канале звука и устанавливает
  // соотв. флаги в конфликтующих голосах, а затем делает эти флаги видимыми
  bool NotesGetPos(int *notes1, int &num1, int *notes2, int &num2, int *notes3, int &num3);

  // ставим флаг конфликта диссонанса во всех конфликтующих голосах, кроме одного (первого)
  // чтобы затем в GenerateAccord() не включать голоса с поднятым флагом конфликта диссонанса
  void ConflictDiss();

  // выводим на диалогбоксе номера нот в соответствующих им позициях панорамы,
  // также выводим суммарный диссонанс аккорда и др.
  void NotesSetText();

  void NotesModulo12(); // приводим все ноты к диапазону 0...11
  void NewNoteOff(int voice_index); // отключаем генерацию голоса voice_index
  void NewNoteOn(int voice_index); // включаем генерацию голоса voice_index
};

