// файловые классы и функции

#include "stdafx.h"
#include "stdafx2.h"

// копирование строки (имени файла с расширением) и отбрасывание расширения
void copy_wo_ext(wchar_t *dst, const wchar_t *src, int maxlen)
{
  if (maxlen <= 0) wcscpy(dst, src);
  else // >= 1
  {
    wcsncpy(dst, src, maxlen);
    dst[maxlen-1] = 0;
  }
  // убираем расширение
  wchar_t *point = wcsrchr(dst, L'.');
  if (point) *point = 0;
}

wstring PathMinusFile(wstring path)
// возвращает путь без файла - до последнего слеша включительно
{
  size_t ilastlett = path.rfind(L'\\');
  if (ilastlett != npos) path.resize(ilastlett+1);
  else                   path.clear(); // если нет слеша - то нет и пути!
  return path;
}

wstring FNameExtFromPath(wstring path)
// возвращает имя файла с расширением - после последнего слеша
{
  size_t ilastlett = path.rfind(L'\\');
  if (ilastlett != npos) path = path.substr(ilastlett+1);
  return path;
}

wstring FExtFromPath(wstring path)
// возвращает расширение файла - после последней точки
{
  size_t ilastlett = path.rfind(L'.');
  if (ilastlett != npos) path = path.substr(ilastlett+1);
  return path;
}

wstring FNameFromPath(wstring path)
// возвращает имя файла без расширения - между последним слешем и последней точкой
{
  size_t ilastslash = path.rfind(L'\\');
  if (ilastslash != npos) path = path.substr(ilastslash+1);

  size_t ilastpoint = path.rfind(L'.');
  if (ilastpoint != npos) path.resize(ilastpoint);
  return path;
}

bool include_wildcards(wstring file)
// возвращает true если в строке file есть литеры поиска файлов '*' и/или '?'
{
  size_t w1 = file.find(L'*');
  size_t w2 = file.find(L'?');
  return w1 != npos || w2 != npos;
}

wstring GetModulePath()
// возвращает путь\ к исполняемому файлу.exe/scr/dll (со слешем в конце)
{
  wchar_t str[MAX_PATH+4];
  int len = GetModuleFileName(0, str, MAX_PATH);
  str[len] = 0;

  // убираем имя программы в конце пути - после последнего слеша
  if (len)
  {
    wchar_t *lastslash = wcsrchr(str, L'\\');
    if (lastslash) *(lastslash+1) = 0;
  }

  return wstring(str);
}

wstring AddModulePath(wstring2 file)
// добавляет к file полный путь выполняемого exe модуля
{
  wstring2 path = GetModulePath();
  return path + file;
}

bool ShellExecuteOpen(const wchar_t *url_or_file, const wchar_t *params)
// при запуске .exe параметры коммандной строки передаются в parameters!
{
//if ( 32L >= (long) ShellExecute(NULL, L"open", url_or_file, params, NULL, SW_SHOWNORMAL) ) return false;
  if ( HINSTANCE (32L) >=
    ShellExecute(NULL, L"open", url_or_file, params, UNI_NULL_STR, SW_SHOWNORMAL) ) return false;
  return true;
}

DWORD RecursiveFindFiles( wstring pathfilemask, bool recursive, int param2, int (FileFunc)(wstring, int) )
// ищем все файлы по маске pathfilemask (в том числе с точным именем единственного файла)
// в начале pathfilemask может быть (полный или частичный) путь к искомым файлам
// при recursive = true делаем то же самое во всех субдиректориях этого пути
// для каждого файла file вызывается функция int FileFunc(wstring file, int param2)
// возвращает код ошибки GetLastError()
{
  wstring path = PathMinusFile( pathfilemask ); // исходный путь из маски
  wstring filemask = FNameExtFromPath( pathfilemask ); // исходная маска файла

  WIN32_FIND_DATA ffData;
  HANDLE hFind;
  
  // если нужна рекурсия, то сначала ищем субдиректории - по маске *

  if (recursive)
  {
    wstring pathmask = path + L'*';
    hFind = FindFirstFile(pathmask.c_str(), &ffData);

    if (hFind != INVALID_HANDLE_VALUE) // что-то найдено, проверяем
    do
    {
      // отбираем только имена субдиректорий
      if ( ffData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
      {
        wstring findname = ffData.cFileName; // имя найденной субдиректории
        // всегда пропускаем субдиректории "." и ".."
        if (findname == L"." || findname == L"..") continue;
        // делаем новую маску поиска файлов в субдиректории:
        // исходный путь + найденная субдиректория + исходная маска файла
        wstring pathfilemask2 = path + findname + L'\\' + filemask;
        // рекурсивно вызываем саму себя
        RecursiveFindFiles(pathfilemask2, recursive, param2, FileFunc);
      }
    }
    while (FindNextFile(hFind, &ffData) != 0);

    FindClose(hFind); // закрываем поиск!
  }
  
  // теперь ищем только файлы по исходной маске

  hFind = FindFirstFile(pathfilemask.c_str(), &ffData);

  if (hFind != INVALID_HANDLE_VALUE)
  do
  {
    if ( !(ffData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) // это файл, перекодируем
    {
      wstring findname = ffData.cFileName;
      // делаем: исходный путь из маски + точное имя найденного файла
      wstring pathfindname = path + findname;
      // вызываем функцию обработки файла
      FileFunc(pathfindname, param2);
      // если функция вернула не 0 - всё равно продолжаем дальше...
    }
  }
  while (FindNextFile(hFind, &ffData) != 0);

  DWORD dwError = GetLastError();
  FindClose(hFind); // закрываем поиск!
  return dwError;
}

// Windows-диалог записи файла, работает не так как написано в документации:
// полное имя выбранного файла возвращается в File, а FileTitle вообще не меняется!!
bool FileSaveAs(HWND hwnd,    // хендл окна, вызывающего диалог
       const void *buffer, int numbytes, // буфер содержимого файла и его байт-длина
       const wchar_t *DlgTitle,  // заголовок окна диалога
       // >=256 байт для полного имени файла, сначала там - дефолтное имя файла без пути!
       wchar_t *File,
       const wchar_t *DefExt,    // расширение файла, если юзер не ввел его
       const wchar_t *Filter,    // список допустимых расширений файлов, см. OPENFILENAME
//     wchar_t *FileTitle=UNI_NULL_STR,  // >= 256 байт для возврата короткого filename.ext без пути
       uint32 *bytes_write, // если есть ук-ль, то в нем будет число записанных в файл байт
       const wchar_t *DirName, // первоначальный каталог для файла, если 0 то "текущий"
       bool true_save, // false если не надо записывать файл, а только выбрать имя и путь!
       bool overwriteprompt) // false если не надо переспрашивать перезаписывать ли существующий файл
{
  // если имени каталога нет - берем текущий каталог (т.е. во всех вындах одинаково)
  wchar_t Name[512];
  if ( wcscmp(DirName, UNI_NULL_STR) == 0 )
  {
    GetCurrentDirectory(512, Name);
    DirName = Name;
  }

  OPENFILENAME ofn;
  memset(&ofn, 0, sizeof(OPENFILENAME));
    
  ofn.lStructSize     = sizeof(OPENFILENAME);
  ofn.hwndOwner       = hwnd;
  ofn.lpstrFilter     = Filter;
  ofn.lpstrInitialDir = DirName;
  ofn.nFilterIndex    = 1;
  ofn.lpstrFile       = File;
  ofn.nMaxFile        = 256; // sizeof(File);
  ofn.lpstrFileTitle  = 0; // FileTitle;
  ofn.nMaxFileTitle   = 0; // sizeof(FileTitle);
  ofn.lpstrDefExt     = DefExt;
  ofn.lpstrTitle      = DlgTitle;
  ofn.Flags = OFN_HIDEREADONLY;
  if (overwriteprompt) ofn.Flags |= OFN_OVERWRITEPROMPT;

  // выводим на экран диалоговую панель выбора выходного файла
  if( GetSaveFileName(&ofn) )
  {
    // если файл успешно выбран, открываем его для записи или создаем
    if (*ofn.lpstrFile)
    {
      // в исходном параметре File содержится полный путь к выбранному в диалоге файлу
      if (!true_save) return true;

      // создаем файл для записи
      HANDLE hfile = CreateFile(ofn.lpstrFile, GENERIC_WRITE, 0, NULL, // без шаре!
                                 CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
      if (hfile == INVALID_HANDLE_VALUE) return false; // файл создать не удалось

      // записываем данные в файл
      DWORD dwBytesWritten;
      BOOL bResult = WriteFile(hfile, buffer, numbytes, &dwBytesWritten, NULL);
      CloseHandle(hfile); // закрываем файл - как хендл!
      if (!bResult) return false; // записать данные не удалось

      // если указатель на bytes_write есть - возвращаем в нем число записанных байт
      if (bytes_write) *bytes_write = dwBytesWritten;
      return true;
    }
  }
  return false;
}

// Windows-диалог чтения файла, подробности о параметрах см. выше
bool FileOpen(HWND hwnd,
       void *buffer, int numbytes, // буфер для чтения файла и его макс. длина
       const wchar_t *DlgTitle,
       wchar_t *File,      // >=256 байтный буфер для полного имени файла (имя.расширение)
       const wchar_t *DefExt,
       const wchar_t *Filter,
       uint32 *bytes_read, // если есть ук-ль, то в нем будет число прочитанных из файла байт
       // относительный (к "текущей папке") или абсолютный путь к папке для открытия файла
       const wchar_t *DirName, // если 0 то берется "текущая папка"
       bool true_load, // false если не надо загружать файл, а только выбрать его
       bool need_dialog) // если 0 то без Win диалог-бокса читается файл File
{
  wchar_t Name[512];
  if ( wcscmp(DirName, UNI_NULL_STR) == 0 )
  {
    GetCurrentDirectory(512, Name);
    DirName = Name;
  }

  OPENFILENAME ofn;
  memset(&ofn, 0, sizeof(OPENFILENAME));
    
  ofn.lStructSize     = sizeof(OPENFILENAME);
  ofn.hwndOwner       = hwnd;
  ofn.lpstrFilter     = Filter;
  ofn.lpstrInitialDir = DirName;
  ofn.nFilterIndex    = 1;
  ofn.lpstrFile       = File;
  ofn.nMaxFile        = 256; // sizeof(File);
  ofn.lpstrDefExt     = DefExt;
  ofn.lpstrTitle      = DlgTitle;
  // только чтение, скрыть флажек
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_READONLY | OFN_HIDEREADONLY;

  if (need_dialog)
  {
    // определяем имя файла в диалогбоксе
    if ( !GetOpenFileName(&ofn) ) return false;
  }

  // далее "молча" грузим файл
  if (*ofn.lpstrFile)
  {
    // в исходном параметре File содержится полный путь к выбранному в диалоге файлу
    if (!true_load) return true;

    // открываем файл для чтения
    HANDLE hfile = CreateFile(ofn.lpstrFile, GENERIC_READ, FILE_SHARE_READ, NULL,
                               OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hfile == INVALID_HANDLE_VALUE) return false; // файл открыть не удалось

    // читаем данные из файла
    DWORD dwBytesRead;
    BOOL bResult = ReadFile(hfile, buffer, numbytes, &dwBytesRead, NULL);
    CloseHandle(hfile);
    if (!bResult) return false; // файл прочесть не удалось

    // если указатель на bytes_read есть - возвращаем в нем число прочитанных байт
    if (bytes_read) *bytes_read = dwBytesRead;
    return true;
  }
  return false;
}

// ===============================================================================

void MapFile::Close()
{
  if (fhandle)
  {
    if (fmem) UnmapViewOfFile(fmem);
    CloseHandle(fhandle);
  }
  fhandle = 0;
  fmem = 0;
  fname.clear();
  fsize = 0;
}

bool MapFile::CreateMapFile(wstring name, int size)
{
  if (fhandle) return false; // файл уже создан этим же объектом
  fname = name;
  fsize = size;

  fhandle = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, fsize, fname.c_str());
  if (!fhandle) return false; // can't create file mapping

  if (GetLastError() == ERROR_ALREADY_EXISTS)
  {
    // файл уже существует (напр. создан другим объектом)
    CloseHandle(fhandle);
    fhandle = 0;
    return false;
  }
  // else file mapping created successfully

  // map a view of the file into the address space.
  fmem = MapViewOfFile(fhandle, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
  return fmem != 0; // true если отображение мапфайла в RAM произошло успешно
}

bool MapFile::Open(wstring name)
{
  if (fhandle) return false; // файл уже открыт, повторно не открываем!
  fname = name;

  // see if a memory-mapped file already exists
  fhandle = OpenFileMapping( FILE_MAP_READ | FILE_MAP_WRITE, FALSE, fname.c_str() );
  if (!fhandle) return false; // can't open file mapping

  // map a view of the file into the address space.
  fmem = MapViewOfFile(fhandle, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
  return fmem != 0; // true если отображение мапфайла в RAM произошло успешно
}

