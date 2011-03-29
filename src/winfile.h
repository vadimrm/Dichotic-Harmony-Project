
#pragma once

void copy_wo_ext(wchar_t *dst, const wchar_t *src, int maxlen=0);
bool include_wildcards(wstring file);
wstring PathMinusFile(wstring path);
wstring FNameExtFromPath(wstring path);
wstring FNameFromPath(wstring path);
wstring FExtFromPath(wstring path);
wstring GetModulePath();
wstring AddModulePath(wstring2 file = UNI_NULL_STR);
bool ShellExecuteOpen(const wchar_t *url_or_file, const wchar_t *parameters = UNI_NULL_STR);
DWORD RecursiveFindFiles( wstring pathfilemask, bool recursive, int par2, int (FileFunc)(wstring, int) );

bool FileSaveAs(HWND hwnd, const void *buffer, int numbytes, const wchar_t *DlgTitle, wchar_t *File, const wchar_t *DefExt,
     const wchar_t *Filter, uint32 *bytes_write=0, const wchar_t *DirName=UNI_NULL_STR, bool true_save=true, bool overwriteprompt=true);
bool FileOpen(  HWND hwnd,       void *buffer, int numbytes, const wchar_t *DlgTitle, wchar_t *File, const wchar_t *DefExt,
     const wchar_t *Filter,  uint32 *bytes_read=0, const wchar_t *DirName=UNI_NULL_STR, bool true_load=true, bool need_dialog=true);

/*
#pragma pack(push, 1) // иначе по умолчанию выравнивание по 4 байта...
//...
#pragma pack(pop)
*/

// длину существующего мапфайла определить нельзя - нет функций!
class MapFile
{
  HANDLE fhandle; // хендл мапфайла или 0
  void *fmem; // указатель на начало RAM мапфайла или 0

  wstring fname; // имя мапфайла
  int fsize; // его длина в байтах, определяется только при CreateMapFile()!

public:
  MapFile():fhandle(0),fmem(0),fsize(0),fname() {}
  ~MapFile() { Close(); }

  // создаем и открываем мапфайл, запоминаем имя и размер мапфайла
  bool CreateMapFile(wstring name, int size); // возвращает true = мапфайл создан (один раз!), false = нет
  bool Open(wstring name); // открываем существующий мапфайл, возврат true = файл открыт
                           // при этом длина файла неизвестна!
  void Close(); // закрываем мапфайл: если все владельцы его закрыли - он уничтожаеся!

  uint8 *getmem() const { return (uint8 *)fmem; }
  // int getsize() const { return fsize; }
  wstring getname() const { return fname; }
};

