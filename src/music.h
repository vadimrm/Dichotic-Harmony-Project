
#pragma once


enum CheckCtrlIndex
{
  Generate_Accord,          Switchon_Left_Pan,       Switchon_Mid_Pan, Switchon_Right_Pan, Loop_Sequence, 
  Edit_Sequence,            Save_With_Manipuls,      Sort_Mode,        Big_Duration,       One_Octave,
  Dont_Change_GM_Instr,     Edit_Comment,            With_Unisons,     Dichotic_Output,    Swap_Output,
  Zero_Ground_Notes,        Show_Sorted_Notes,       Mute_Sound,       Converter_Mode,     Converter_Auto,
  Converter_Mirror_Accords, Converter_With_Unisons,  No_Sound,
};

enum ButtonCtrlIndex { Play_Stop, Rewind, Save_Accords_Sequence, Make_Voices_Combinations, Press_Sequence, Sort_Sequence };

enum TextSliderCtrlIndex
{ Accord_Number, Sequence_Speed, Accord_Volume, Transposition, Accord_Duration, Accord_Temp, Stereo_Base, Notes_Shift };

enum ComboBoxCtrlIndex { GM_Instrument, MidiOutDevice, };

enum EditCtrlIndex { Accords_Sequence_Comment, };

enum SpinEditCtrlIndex { Voices_Combinations, };

enum TextCtrlIndex { Left_Notes, Mid_Notes, Right_Notes, Total_Diss, Uniq_Notes, };


// конкретный диалог бокс для нужд данной программы: встраиваем в него только одинаковые функции
// для всех боксов, остальные должны быть внешними (возможно друзьями)!
class MusicDialogBoxCtrl : public DialogBoxCtrl
{
  static const int timer_msec = 10; // мсек, период генерации WM_TIMER, точность времени нот! < 10 улучшений не даёт!
  static const int MAX_CONVERTER_VOICES = 15; // аккорды с большим числом голосов подвешивают код при ConverterMode

  static const int MAX_ACCORDS = 7000; // размер массива аккордов accords_sequence (макс. число аккордов в секвенции)
  Ar <DichoticAccord> accords_sequence; // секвенция аккордов
  SequenceHeader cheader; // хедер секвенции, из файла или сделанный вручную

  int accords_in_sequence; // количество аккордов в загруженной секвенции
  int accord_act_index; // текущий индекс проигрываемого (или редактируемого) аккорда секвенции
  int play_accords_sequence; // флаг проигрывания секвенции: 0=стоп, 1=старт, 2=продолжение проигрывания

  // флаг для правильного "перенажатия" аккорда (нужен из-за наличия и кнопки и хоткея генерации)
  int gen_state; // 0 нет генерации, 1 начало генерации, 2 генерация, 3 конец генерации аккорда

  int dialog_voices; // число голосов с контролами на диалогбоксе, остальные голоса будут "невидимы"
  Ar <DichoticVoice,SLOW_ARRAY> dvoices; // голоса 2H аккорда, максимальное количество MAX_ACC_VOICES

  DichoticAccord gen_acc; // текущий (последний) звучащий аккорд

  friend int WINAPI WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR Cmd, int CmdShow);

public:
  static const int MAX_INTERVAL = 128; // 1+максимальный интервал нот аккорда, полутонов РТС12
  static int diss_rating[MAX_INTERVAL]; // массив диссонансов для каждого интервала созвучия

  MusicDialogBoxCtrl()
  {
    cheader.clear();
    accords_in_sequence = 0;
    accord_act_index = 0;
    play_accords_sequence = 0;
    gen_state = 0;
    accords_sequence.renew(MAX_ACCORDS, false);
    dvoices.renew(DichoticAccord::MAX_ACC_VOICES, false);
  }

  int voice_index(int switchon_voice_id) const // находим индекс голоса, Switchon_Voice.id которого = switchon_voice_id
  {
    for (int i = 0; i < dvoices.elements(); ++i)
      if ( switchon_voice_id == dvoices[i].get_switchon_voice_id() ) return i;
    return -1; // это значит что такого контрола нет!
  }

  // заполняем список midi девайсов, открываем девайс
  void InitMidiDevices(int device_number);
  // заполняем список midi инструментов, установим номер инструмента во всех midi каналах
  void InitInstruments(int instrument_number);
  static void SetInstrument(int instrument_number); // установим номер инструмента во всех диалог боксах

  // инициал-м голоса dvoices из массива идентифик-в их контролов [number][Note_Voice_id,Pan_Voice_id,Switchon_Voice_id]
  // при max_number == 0 используется весь массив, иначе используется не более max_number его членов
  template<int number> void InitDichoticVoices(const int (&id_array)[number][3], int max_number = 0);

  // какая из этих функций будет работать для конкретного экземпляра бокса определяется в DialogBoxCtrl::Create() 
  static int CALLBACK DialogProc0(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp); // обработчик сообщений диалог бокса 0
  static int CALLBACK DialogProc1(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp); // --//-- диалог бокса 1
  static int CALLBACK DialogProc2(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp); // --//-- --//-- 2

  static int CommonDialogProc(UINT message, WPARAM wparam, LPARAM lparam); // общая часть всех DialogProc

  void CALLBACK InitDialog(int dnum); // отработка WM_INITDIALOG для диалог бокса номер dnum = 0...(DNUMS-1)

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

  // если аргумент = false: выводим текст числа сочетаний голосов на кнопке Make_Voices_Combinations
  // если аргумент = true : дополнительно делаем секвенцию сочетаний голосов аккорда из их регуляторов
  bool MakeVoicesCombinations(bool make = false); // возвращает false если секвенция не создана

  void SortSequenceDiss(); // сортировка аккордов секвенции в порядке возрастания полных диссонансов аккорда
  void PressSequenceAccords(); // сжатие секвенции путём удаления голосов-пауз из аккордов

  void ConverterModeMain(bool Seq = false); // управление конвертацией, при true конвертация всей секвенции
  bool ConverterModeExecute(DichoticAccord acc1h); // false если невозможна 2H конвертация аккорда acc1h

  // аккорд секвенции Accord_Number становится 1-м, а остальные аккорды выстраиваются по степени
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

  // при mode=0 (Save_Accord       ) записываем текущий аккорд в файл, если файл есть - стираем и перезаписываем его
  // при      1 (Append_Accord     ) добавляем аккорд к сушествующему файлу, а если файла нет - пишем его с нуля!
  // при      2 (Save_Accords_Sequence) записываем секвенцию аккордов в файл, если файл есть - стираем и перезаписываем его
  // при      3 (запись секвенции после сортировки) то же что и при 2, кроме замены текущего аккорда!
  // при (2) сначала текущий звучащий аккорд копируется в текущее место секвенции, т.о. возможно её редактирование!
  // при (0,1,3) секвенцию не меняем!
  // если чекбокс Edit_Sequence не выбран - секвенцию не меняем и при mode=2!
  void SaveMusic(int mode);

  // считываем контролы общих параметров, возвращаем их в виде SequenceHeader
  SequenceHeader SequenceHeaderFromControls();

  // кнопка циклического переключения КНОПОК всех панорам лево-центр-право
  // возвращает true если хотя бы один голос был включен (звучал)
  bool AllPanSwitch();

  // кнопка сброса включателей всех голосов в откл., затем циклически включение-выключение первых 6 голосов
  void AllVoicesSwitch();

  // в группе голосов делаем одинаковую (РУЧНУЮ!) операцию: -1, +1 сдвиг высоты; 0 сброс высоты в 0
  // возвращает true если хотя бы один голос группы был включен (звучал)
  bool VoicesGroup(int first_voice, int voices_num, int operation);

  DichoticAccord GetAccord(); // считываем все контролы, формируем аккорд (до манипуляторов)
  void GenerateAccord(); // включаем или выключаем аккорд

  void TimerMessage(); // обработчик сообщений таймера
  void MaybeGenerateAccord(int new_state); // возможное вкл. или откл. аккорда
  void RegenAccord(); // если аккорд звучал - перенажимаем аккорд
  void RewindAccords(); // копируем начальный аккорд секвенции в аккорд на диалогпанели
  void Play_StopAccords(); // включаем или останавливаем проигрывание секвенции
  bool IsPlaying() const { return (play_accords_sequence == 2); }
  void SetupAccord(int index); // копируем аккорд из массива с индексом index в аккорд на диалогпанели
  void ResetMaxDuration(); // устанавливаем предел слайдера Accord_Duration
  void SetupAccords(int num_accords); // подготовка контролов для новой секвенции в accords_sequence и cheader

  // выводим на диалогбоксе номера нот в соответствующих им позициях панорамы,
  // также выводим суммарный диссонанс аккорда и др.
  void NotesSetText();

  // включаем генерацию голоса voice_index с нужной midi_note, панорамой и громкостью
  inline void NewNoteOn(int voice_index, int midi_note, double pan, int volume);

  inline void NewNoteOff(int voice_index, int midi_note); // отключаем генерацию голоса
};

