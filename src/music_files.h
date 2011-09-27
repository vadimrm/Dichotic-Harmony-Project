
#pragma once

#pragma pack(push, 1) // иначе по умолчанию выравнивание по 4 байта...

class MusicFile // файл .daccords - секвенция аккордов
{
  // по умолчанию
  static const wchar_t *dflt_dir; // директория файлов относительно программы
  static wchar_t *dflt_filename; // имя файла (без расширения) - надо выделять массив литер!
  static const wchar_t *format_version_header; // первое слово в начале файла
  static const int dflt_version; // версия формата файла, который воспринимает код
  int version; // версия формата, прочитанная из файла, должна совпадать с dflt_version!

  HWND hwnd; // файловые диалоги будут модальны окну hwnd

public:
  MusicFile(HWND hwnd_parent) : hwnd(hwnd_parent) {}

  // открываем диалог чтения из файла или делаем чтение "молча" если file непустое
  // DirName - папка для файла, если 0 то "текущая" - из GetCurrentDirectory()
  // данные читаются в массив аккордов, max_accords макс. число аккордов в массиве,
  // а на выходе из функции в случае успеха max_accords - число прочитанных аккордов
  bool Open(SequenceHeader &ch, DichoticAccord *acc_sequence, int &max_accords, const wchar_t *file=UNI_NULL_STR, const wchar_t *DirName=dflt_dir);
  // вариант для PAS файлов редактора Muse
  bool OpenPAS(int dtrans, SequenceHeader &ch, DichoticAccord *acc_sequence, int &max_accords, const wchar_t *file=UNI_NULL_STR, const wchar_t *DirName=L"pas");
  // диалог записи аккордов в файл, подробности о mode см. описание SaveMusic()
  bool SaveAs(SequenceHeader ch, const DichoticAccord *acc_sequence, int num_accords, int mode, const wchar_t *DirName=dflt_dir);

  friend void MakeAccordsStrings(SequenceHeader ch, const DichoticAccord *acc_sequence, int num_accords, wstring2 &s0, wstring2 &s);
};

#pragma pack(pop)

