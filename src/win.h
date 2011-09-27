
#pragma once


// все контролы обращаются к оконным ф-ям win32 только через эти функции:

inline bool IsDialogItem(HWND dialog_hwnd, int item_id) { return NULL != GetDlgItem(dialog_hwnd, item_id); }

inline void EnableDialogItem(HWND dialog_hwnd, int item_id, bool enable = true)
{
  HWND item_hwnd = GetDlgItem(dialog_hwnd, item_id);
  if ( NULL != item_hwnd ) EnableWindow(item_hwnd, enable);
}

inline LRESULT SendDialogItemMessage(HWND dialog_hwnd, int item_id, UINT msg, WPARAM wp, LPARAM lp)
{
  HWND item_hwnd = GetDlgItem(dialog_hwnd, item_id);
  if ( NULL == item_hwnd ) return 0; // в диалог боксе нет такого итема
  return SendMessage(item_hwnd, msg, wp, lp);
}

//  некот. ф-и класса DialogBoxCtrl также обращаются напрямую к оконным ф-ям win32...



struct TextCtrl // (статик) текст контрол диалог бокса  - строка с переменным текстом
{
  int id; // идентификатор контрола
  const wchar_t *text_str; // полный текст контрола или его префикс...
  HWND hdlg;

  void init(HWND dialog_hwnd)
  {
    hdlg = dialog_hwnd;
    text(text_str);
  }

  void enable(bool enable) { EnableDialogItem(hdlg, id, enable); }

  // выводим str
  void text(const wchar_t *str) { SendDialogItemMessage(hdlg, id, WM_SETTEXT, 0, LPARAM(str)); }

  void add_text(const wchar_t *add_str) // выводим (text_str + add_str)
  {
    wstring str(text_str);
    str += add_str;
    text( str.c_str() );
  }
};

struct SliderCtrl // слайдер контрол диалог бокса
{
  // мин/макс позиции слайдера (включая отрицательные), "строчный" и "страничный" шаг
  int pmin, pmax, linesz, pagesz;
  int id;  // идентификатор слайдера
  int actpos; // текущая позиция слайдера
  double kmul; // коэффициент, на который надо умножить текущую позицию слайдера для получения регулируемой величины
  int print_max_value; // если !0, то после actpos надо печатать pmax на индикаторе слайдера
  HWND hdlg; // хендл окна диалог-бокса, внутри которого сидят элементы

  void init(HWND dialog_hwnd)
  {
    hdlg = dialog_hwnd;
    set_minmax();
    SendDialogItemMessage(hdlg, id, TBM_SETPOS,      1, actpos);
    SendDialogItemMessage(hdlg, id, TBM_SETLINESIZE, 0, linesz);
    SendDialogItemMessage(hdlg, id, TBM_SETPAGESIZE, 0, pagesz);
  }

  void reset_minmax(int new_min, int new_max) { pmin = new_min; pmax = new_max; set_minmax(); }
  void set_minmax() { SendDialogItemMessage(hdlg, id, TBM_SETRANGE, 1, MAKELONG(pmin, pmax)); }

  void enable(bool enable) { EnableDialogItem(hdlg, id, enable); }

  int setpos(int pos)
  {
    if ( IsDialogItem(hdlg, id) ) // если контрол есть
    {
      SendDialogItemMessage(hdlg, id, TBM_SETPOS, 1, pos);
      actpos = getpos();
    }
    else // эквивалентный код для невидимых контролов, id которых есть в массиве, но нет на диалог боксе
    {
      actpos = minxmax(pmin, pos, pmax);
    }
    return actpos;
  }

  int getpos() // выдаёт позицию слайдера, но не регулируемую величину (если kmul!=1.)
  {
    if ( IsDialogItem(hdlg, id) )
    {
      // как правило эта ф-я вызывается при изменении позиции слайдера, поэтому меняем и текст
      actpos = SendDialogItemMessage(hdlg, id, TBM_GETPOS, 0, 0);
    }
    return actpos;
  }

  // преобразование текущей позиции слайдера в регулируемую величину
  double get_reg_value() { return kmul*actpos; }
  // преобразование максимальной позиции слайдера в регулируемую величину
  double get_max_value() { return kmul*pmax; }

  // преобразование регулируемой величины в позицию слайдера
  int get_pos_value(double reg_value) { return float2int(reg_value/kmul); }
};


// слайдер контрол диалог бокса + переменная строка текста
struct TextSliderCtrl : public SliderCtrl, public TextCtrl
{
  struct Members // структура для инициализации членов TextSliderCtrl списком констант
  {
    // мин/макс позиции слайдера (включая отрицательные), "строчный" и "страничный" шаг
    int pmin, pmax, linesz, pagesz;
    int id;  // идентификатор слайдера
    int id_text; // идентификатор строки текста для вывода текущей позиции слайдера в виде числа
    const wchar_t *prefix; // начало строки текста, после которого выводится числовое значение регулируемой величины
    int actpos; // текущая позиция слайдера
    double kmul; // коэффициент, на который надо умножить текущую позицию слайдера для получения регулируемой величины
    int print_max_value; // если !0, то после actpos надо печатать pmax на индикаторе слайдера
  };

  TextSliderCtrl& operator=(const Members &src)
  {
    this->pmin    = src.pmin;
    this->pmax    = src.pmax;
    this->linesz  = src.linesz;
    this->pagesz  = src.pagesz;
    this->SliderCtrl::id = src.id;
    this->TextCtrl::id = src.id_text;
    this->text_str     = src.prefix;
    this->actpos  = src.actpos;
    this->kmul    = src.kmul;
    this->print_max_value = src.print_max_value;
    return *this;
  }

  void init(HWND dialog_hwnd)
  {
    SliderCtrl::init(dialog_hwnd);
    TextCtrl::init(dialog_hwnd);
    update_text();
  }

  void enable(bool enable)
  {
    SliderCtrl::enable(enable);
    TextCtrl::enable(enable);
  }

  int setpos(int pos)
  {
    SliderCtrl::setpos(pos);
    update_text();
    return actpos;
  }

  int getpos() // выдаёт позицию слайдера, но не регулируемую величину (если kmul!=1.)
  {
    SliderCtrl::getpos();
    // как правило эта ф-я вызывается при изменении позиции слайдера, поэтому меняем и текст
    update_text();
    return actpos;
  }

  // выводим текст из префикса и числа согласно текущей и макс. позиции слайдера
  void update_text()
  {
    wstring2 str;
    str << get_reg_value();
    if (print_max_value != 0) str << L" of " << get_max_value();
    TextCtrl::add_text( str.c_str() );
  }
};


struct SimpleCheckCtrl // простой чекбокс с 3-мя состояниями и одинаковым префиксом текста для них
{
  int id; // идентификатор контрола
  int state; // текущее состояние чекбокса BST_UNCHECKED, BST_CHECKED, BST_INDETERMINATE (=0,1,2)
  const wchar_t *text_str; // префикс текста контрола в любом состоянии (возможно пустой)
  HWND hdlg; // хендл окна диалогбокса

  void init(HWND dialog_hwnd)
  {
    hdlg = dialog_hwnd;
    // уст. первоначального состояния и вывод текста контрола
    setstate(state);
    text(text_str);
  }

  int getstate()
  {
    if ( IsDialogItem(hdlg, id) ) state = SendDialogItemMessage(hdlg, id, BM_GETCHECK, 0, 0);
    // else не меняем state
    return state; // 0,1,2 = BST_UNCHECKED, BST_CHECKED, BST_INDETERMINATE
  }

  // тут не делается getstate():
  int get_state_3() const { return state; } // 0,1,2 = BST_UNCHECKED, BST_CHECKED, BST_INDETERMINATE
  bool checked_state() const { return state == BST_CHECKED; }
  bool unchecked_state() const { return state == BST_UNCHECKED; }

  void setstate(int check_state) // уст. состояния контрола
  {
    state = minxmax(0, check_state, 2);
    SendDialogItemMessage(hdlg, id, BM_SETCHECK, state, 0);
  }

  void enable(bool enable) { EnableDialogItem(hdlg, id, enable); }

  // выводим str
  void text(const wchar_t *str) { SendDialogItemMessage(hdlg, id, WM_SETTEXT, 0, LPARAM(str)); }

  void add_text(const wchar_t *add_str) // выводим (text_str + add_str)
  {
    wstring str(text_str);
    str += add_str;
    text( str.c_str() );
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

  void setstate(int check_state, bool setup_text = false) // уст. состояния и вывод текста контрола
  {
    state = minxmax(0, check_state, 2);
    SendDialogItemMessage(hdlg, id, BM_SETCHECK, state, 0);
    state2text(setup_text);
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
      state = SendDialogItemMessage(hdlg, id, BM_GETCHECK, 0, 0);
      state2text();
    }
    // else не меняем state!
    return state; // 0,1,2 = BST_UNCHECKED, BST_CHECKED, BST_INDETERMINATE
  }
  // тут сначала делается getstate():
  bool getstate_checked() { return getstate() == BST_CHECKED; }
  bool getstate_unchecked() { return getstate() == BST_UNCHECKED; }

  // тут не делается getstate():
  int return_state() const { return state; } // 0,1,2 = BST_UNCHECKED, BST_CHECKED, BST_INDETERMINATE
  bool checked_state() const { return state == BST_CHECKED; }
  bool unchecked_state() const { return state == BST_UNCHECKED; }

  void set_checked_state(bool checked) { setstate( checked? BST_CHECKED:BST_UNCHECKED ); }
  void enable(bool enable) { EnableDialogItem(hdlg, id, enable); }
  void text(const wchar_t *str) { SendDialogItemMessage(hdlg, id, WM_SETTEXT, 0, LPARAM(str)); }  // выводим str
  void inverse_state() { set_checked_state( !checked_state() ); } // 0/1 = BST_UNCHECKED/BST_CHECKED
};


struct ButtonCtrl // Button контрол диалог бокса, в т.ч. т.н. "би-кнопки" (2 функциональных состояния)
{
  int id; // идентификатор контрола
  const wchar_t *name0; // неизменный текст контрола, для "би-кнопок" - текст в функциональном состоянии 0
  const wchar_t *name1; // текст "би-кнопок" в функциональном состоянии 1 (см. напр. би-кнопку Play_Stop)
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

  // выводим str
  void text(const wchar_t *str) { SendDialogItemMessage(hdlg, id, WM_SETTEXT, 0, LPARAM(str)); }
  void button_text() { func_state==0? text(name0):text(name1); }
  void button_text(int new_func_state)
  {
    func_state = new_func_state;
    button_text();
  }

  void enable() { EnableDialogItem(hdlg, id, enable_state); }
  void enable(bool new_enable_state)
  {
    enable_state = new_enable_state;
    enable();
  }
};


struct EditCtrl // Edit контрол диалог бокса
{
  int id; // идентификатор контрола
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

  // выводим str
  void text(const wchar_t *str) { set_text(str); } // синоним для общности
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

  void enable() { EnableDialogItem(hdlg, id, enable_state); }
  void enable(bool new_enable_state)
  {
    enable_state = new_enable_state;
    enable();
  }
};

struct SpinEditCtrl // Spin контрол, прикрученный к Edit контролу диалог бокса (только числа)
{
  int id; // идентификатор spin контрола
  bool alignleft; // 1 = спин слева редактора, 0 = справа
  int pmin, pmax, actpos; // макс, мин и текущее значение спина (от -32768 до 32767, спин - 16-бит функция)
  int id_edit; // идентификатор edit контрола
  bool readonly_edit; // 1 ридонли редактор
  EditCtrl edit;
  HWND hdlg;

  void init(HINSTANCE hInstance, HWND dialog_hwnd)
  {
    hdlg = dialog_hwnd;
    edit.id = id_edit;
    edit.readonly_state = readonly_edit;
    edit.enable_state = 1;
    edit.init(hdlg);
    // создаем спин, прикрученный к окну редактирования
    HWND hEdit = GetDlgItem(dialog_hwnd, id_edit);
    // UDS_NOTHOUSANDS - не отделяем "тысячи" дополнительным пробелом!
    DWORD dwStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | UDS_NOTHOUSANDS | UDS_SETBUDDYINT | UDS_ARROWKEYS;
    dwStyle |= alignleft? UDS_ALIGNLEFT : UDS_ALIGNRIGHT;
    // относительные координаты левоверхнего угла и размеры спина внутри окна edit
    int x = 0, y = 0, cx = 40, cy = 40; // похоже эти параметры игнорируются - спин делается под размер окна!
    CreateUpDownControl(dwStyle, x, y, cx, cy, dialog_hwnd, id, hInstance, hEdit, pmax, pmin, actpos);
  }

  int return_pos() const { return actpos; }

  void setpos(int pos) { SendDialogItemMessage(hdlg, id, UDM_SETPOS, 0, pos); }

  int getpos()
  {
    actpos = SendDialogItemMessage(hdlg, id, UDM_GETPOS, 0, 0);
    // выход за пределы диапазона возможен при помощи ручного редактирования цифр!
    if ( !in_range(pmin, actpos, pmax) )
    {
      testminxmax(pmin, actpos, pmax);
      setpos(actpos);
    }
    return actpos;
  }
};


struct ComboBoxCtrl // комбобокс контрол диалог бокса
{
  int id; // идентификатор контрола
  HWND hdlg;
  int index; // индекс выбранной строки, >=0

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


// обобщённый диалог бокс для нужд любой программы
class DialogBoxCtrl
{
  int id; // (ресурсный) идентификатор диалог бокса

  static int dialog_count; // общий счетчик класса для первоначального определения индекса экземпляра
protected:
  int dialog_index; // индекс экземпляра диалогбокса: 0, 1, 2 ... пока не используется!
  HWND hdlg; // хендл окна бокса
  HINSTANCE hInst;

  // массивы контролов бокса ("медленные", так безопаснее)
  Ar <EditCtrl,SLOW_ARRAY> edits;
  Ar <SpinEditCtrl,SLOW_ARRAY> spins;
  Ar <TextCtrl,SLOW_ARRAY> textctrls;
  Ar <CheckCtrl,SLOW_ARRAY> checks;
  Ar <ButtonCtrl,SLOW_ARRAY> buttons;
  Ar <ComboBoxCtrl,SLOW_ARRAY> combos;
  Ar <TextSliderCtrl,SLOW_ARRAY> sliders;

public:
  DialogBoxCtrl() { dialog_index = dialog_count++; }

  ~DialogBoxCtrl() { if (hdlg != 0) DestroyWindow (hdlg); }

  bool Create(HINSTANCE hInstance, WORD wInteger, HWND hWndParent, DLGPROC lpDialogFunc)
  {
    hInst = hInstance;
    id = wInteger;
    hdlg = CreateDialog(hInstance, MAKEINTRESOURCE(wInteger), hWndParent, lpDialogFunc);
    return hdlg != 0;
  }

  // любые манипуляции с контролами делаются через эти функции
  EditCtrl&     rw_edit(int index)   { return edits[index]; }
  SpinEditCtrl& rw_spin(int index)   { return spins[index]; }
  TextCtrl&     rw_static(int index) { return textctrls[index]; }
  CheckCtrl&    rw_check(int index)  { return checks[index]; }
  ButtonCtrl&   rw_button(int index) { return buttons[index]; }
  ComboBoxCtrl&   rw_combo(int index)  { return combos[index]; }
  TextSliderCtrl& rw_slider(int index) { return sliders[index]; }

  // константные манипуляции с контролами делаются через эти функции
  const EditCtrl     get_edit(int index)   const { return edits[index]; }
  const SpinEditCtrl get_spin(int index)   const { return spins[index]; }
  const TextCtrl     get_static(int index) const { return textctrls[index]; }
  const CheckCtrl    get_check(int index)  const { return checks[index]; }
  const ButtonCtrl   get_button(int index) const { return buttons[index]; }
  const ComboBoxCtrl   get_combo(int index)  const { return combos[index]; }
  const TextSliderCtrl get_slider(int index) const { return sliders[index]; }

  int checks_index(int checks_id) const // находим индекс чекбокса checks[index], id которого равен checks_id
  {
    for (int i = 0; i < checks.elements(); ++i)
      if ( checks_id == checks[i].id ) return i;
    return -1; // это значит что такого контрола нет!
  }

  HWND hwnd() const { return hdlg; }

  bool IsDialogMsg(LPMSG lpMsg) { return 0 != IsDialogMessage(hdlg, lpMsg); }

  void Show(bool show = true) { if (hdlg != 0) ShowWindow(hdlg, show? SW_SHOW:SW_HIDE); }

  // создаем титульную строку диалог бокса из строки str
  void SetTitle(const wchar_t *str) { SetWindowText(hdlg, str); }

  void Enable(bool enable) { EnableWindow(hdlg, enable); } // вкл-откл управляемости окна бокса

  // размер массива определяется автоматически!
  // инициализируем контролы бокса из массива контролов, передаём в них хендл диалог бокса...
  template<class T, int ctrl_numbers> bool InitControls(const T (&ctrl_array)[ctrl_numbers])
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
    else if ( typeid(T) == typeid(TextSliderCtrl::Members) )
    {
      sliders.renew(ctrl_numbers);
      for (int i = 0; i < ctrl_numbers; ++i)
      {
        sliders[i] = *(TextSliderCtrl::Members*)&ctrl_array[i];
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
    else if ( typeid(T) == typeid(SpinEditCtrl) )
    {
      spins.renew(ctrl_numbers);
      for (int i = 0; i < ctrl_numbers; ++i)
      {
        spins[i] = *(SpinEditCtrl*)&ctrl_array[i];
        spins[i].init( hInst, hwnd() );
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
    else if ( typeid(T) == typeid(TextCtrl) )
    {
      textctrls.renew(ctrl_numbers);
      for (int i = 0; i < ctrl_numbers; ++i)
      {
        textctrls[i] = *(TextCtrl*)&ctrl_array[i];
  //    *((TextCtrl*)&textctrls[i]) = *(TextCtrl*)&ctrl_array[i];
        textctrls[i].init( hwnd() );
      }
    }
    else return false;

    return true;
  }
};

