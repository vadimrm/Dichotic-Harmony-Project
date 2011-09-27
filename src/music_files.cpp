
#include "stdafx.h"
#include "stdafx2.h"

// #pragma warning(push)
// #pragma warning(disable:4996) // 'func' was declared deprecated

const wchar_t *UNI_SPACE2 = L"  ";
const wchar_t *VOICE_PAUSE = L"X"; // маркер паузы голоса в файле аккорда
const wchar_t *UNI_SPACE_VOICE_PAUSE = L" X";
const wchar_t *ACCORD_NUMBER = L";"; // префикс порядкового номера аккорда в файле аккорда
const wchar_t *UNI_SPACE_ACCORD_NUMBER = L" ;";

void MakeAccordsStrings(SequenceHeader ch, const DichoticAccord *acc_sequence, int num_accords, wstring2 &s0, wstring2 &s)
// упаковка аккордов в две строки: s0 - заголовок, s - тело файла аккорда
{
  // в самом начале файла - маркер Unicode текста, затем 1-я строка - номер версии формата файла
  s0 << UNI_HDR << MusicFile::format_version_header << UNI_SPACE
                << MusicFile::dflt_version
                << L"  Commentary:" << UNI_CRLF;

  // затем 2-я строка - комментарий к секвенции аккордов
  wchar_t comment[512]; // комментарий из эдитбокса в файл
  actbox().get_edit(Accords_Sequence_Comment).get_text(comment, 512);
  s0 << comment << UNI_CRLF;

  // затем 3-я строка - названия и значения общих параметров секвенции аккордов
  s0 << L"Transposition " << ch.transposition
     << L"  Sequence_Speed " << ch.sequence_speed
     << L"  Dont_Change_GM_Instr " << ch.dont_change_gm_instrument
     << L"  Instrument_Number " << ch.instrument_number << UNI_CRLF;

  // затем 4-я строка - аббревиатуры числовых параметров аккорда
  s0 << L"Instrum" << UNI_TAB << L"Durat-n" << UNI_TAB << L"TempBPS" << UNI_TAB << L"Voices"
     << UNI_TAB << L"Volume" << UNI_TAB << L"note pan" << UNI_CRLF;

  // затем начиная с 5-й строки идут числовые параметры каждого аккорда секвенции
  for (int n = 0; n < num_accords; ++n) // этот цикл медленный при num_accords > 1000 @
  {
    DichoticAccord acc = acc_sequence[n];

    // преобразуем аккорд согласно реал-тайм манипуляторам
    if ( actbox().get_check(Save_With_Manipuls).checked_state() ) acc.accord_manipulator();

    s << acc.instrument;
    s += UNI_TAB; // += вместо << для ускорения...
    s << acc.duration;
    s += UNI_TAB;
    s << acc.temp;
    s += UNI_TAB;
    s << acc.voices_number;

    // если это не пауза аккорда - выводим все ноты/панорамы аккорда
    if ( !acc.is_pause() )
    {
      s += UNI_TAB;
      s << acc.volume;
      s += UNI_TAB;

      for (int i=0; i < acc.voices_number; ++i)
      {
        if ( acc.dn[i].pause ) // если это пауза голоса
        {
          s += UNI_SPACE_VOICE_PAUSE;
          continue;
        }
        // else это нота
        s += UNI_SPACE2;

        int n = acc.dn[i].note;
        int p = acc.dn[i].pan;
        if (n < 10) s += UNI_SPACE;
        s << n;
        s += UNI_SPACE;
        if (p >= 0) s += L"+";
        s << p;
      }
    }

    // после конца аккорда добавляем пробел, маркер и номер аккорда, номера начинаются с 1!
    s += UNI_SPACE_ACCORD_NUMBER;
    s << (n+1);
    // если есть комментарий - добавляем его через пробел!
    if ( acc.ok_comment() )
    {
      s += UNI_SPACE;
      s << acc.comment;
    }

    s += UNI_CRLF;
  }
}

// ========================== class MusicFile ===========================

wchar_t accords_filename[FILE_STRLEN] = L"accords_file"; // этот массив нужен, не уничтожать!


const wchar_t *MusicFile::dflt_dir = files_default_dir;
wchar_t *MusicFile::dflt_filename = accords_filename;
const int MusicFile::dflt_version = 0;
const wchar_t *MusicFile::format_version_header = L"Dichotic_Accords_File_Format_Version";

// при mode = 1 (Append) добавляем аккорд к сушествующему файлу, а если файла нет - пишем его с нуля!
// при     0, 2, 3(Save) записываем 1 или секвенцию аккордов в файл, если файл есть - стираем и перезаписываем его!
bool MusicFile::SaveAs(SequenceHeader ch, const DichoticAccord *acc_sequence, int num_accords, int mode, const wchar_t *DirName)
{
  // файл на диске будет текстовым в кодировке Unicode
  wstring2 s0, s;
  // упаковка аккордов в две строки: s0 - заголовок, s - тело файла аккорда
  MakeAccordsStrings(ch, acc_sequence, num_accords, s0, s);
  s0 += s; // теперь строка s0 содержит всё для нового файла, а s только для добавления в существующий файл

  // буфер текста и размер буфера в байтах (сначала предполагаем что файл будет новый)
  const void *file_buf = s0.c_str();
  int file_bytes = sizeof(wchar_t)*s0.size();

  // записываем данные в файл, по умолчанию в текущий каталог
  const wchar_t *title[4] = { L"Save Accord", L"Append Accord", L"Save Accords Sequence", L"Save Accords Sequence" };
  const wchar_t *DlgTitle = title[mode]; // титул диалога
  const wchar_t *DefExt = L"daccords"; // расширение файла если юзер не ввел его, без "точки"!
  wchar_t File[FILE_STRLEN]; // буфер для полного имени файла, >= 256 байт!
  // сначала будет дефолтное имя файла
  copy_wo_ext(File, dflt_filename, 256);

  // фильтр пропускает файлы с нужным расширением, или любым другим (применение wstring2 невозможно)
  const wchar_t *Filter = L"Accords Files\0*.daccords\0" // тут надо еще раз явно указать расширение!!
                          L"Any Files\0*.*\0\0"; // в конце 2 нуля!!!

  // выбираем имя файла и путь для записи, но не записываем файл
  bool overwriteprompt = (mode==1)? 0:1; // при Append (1) не спрашиваем разрешение на "перезапись" файла
  if ( FileSaveAs(hwnd, file_buf, file_bytes, DlgTitle, File, DefExt, Filter, 0, DirName, false, overwriteprompt) )
  {
    bool res = 0;
    // запоминаем путь и имя файла (без расширения), выбранное юзером
    copy_wo_ext(dflt_filename, File, 256);

    // печатаем имя сохранённого файла
    actbox().PrintFileTitle(dflt_filename, DefExt, 0);

    if (mode != 1) // Save
    {
      // пишем файл с нуля, выводя строку s0
      res = write_bin(File, file_buf, file_bytes, true);
    }
    else // mode == 1, Append
    // проверяем если такого файла нет - пишем его с нуля, выводя строку s0
    if (!file_exist(File)) res = write_bin(File, file_buf, file_bytes, true);
    else // такой файл уже есть
    {
      // дописываем строку s (без заголовка) в конец файла
      file_buf = s.c_str();
      file_bytes = sizeof(wchar_t)*s.size();
      // это write_bin_append() с разрешением коллективного доступа к файлу на чтение/запись
      res = write_bin_append2(File, file_buf, file_bytes);
    }

    if (!res) Mbox(L"Error save file ", File);
    return res;
  }

  return false;
}

bool MusicFile::OpenPAS(int dtrans, SequenceHeader &ch, DichoticAccord *acc_sequence, int &max_accords, const wchar_t *file, const wchar_t *DirName)
// загрузка секвенции из .PAS файлов от редактора Muse, переделанных в Unicode .txt
// с добавлением в конец 1-й строки файла midi номера инструмента "Instr N", причём
// dtrans - дихотическая транспозиция: если 0, то все аккорды звучат по центру (моно), а любое другое значение
// даёт загрузку в левый канал исходных аккордов, а в правый то же самое со сдвигом высоты на dtans полутонов!
{
  const wchar_t *DlgTitle = L"Muse PAScal (text) Accords Open";
  const wchar_t *DefExt = L"txt";
  wchar_t File[FILE_STRLEN]; // буфер для полного имени файла, >= 256 байт!
  const wchar_t *Filter = L"Muse PAScal (.txt) Accords Files\0*.txt\0"
                          L"Any Files\0*.*\0\0";

  bool need_dialog = ( wcscmp(file, UNI_NULL_STR) == 0 ); // диалог нужен если нет заранее известного файла
  bool res = 0;

  // если диалог нужен - сначала определяем в диалоге имя фала
  if (need_dialog)
  {
    // сначала будет дефолтное имя
    copy_wo_ext(File, dflt_filename, 256);
    res = FileOpen(hwnd, NULL, 0, DlgTitle, File, DefExt, Filter, NULL, DirName, false, true);
    if (!res) return false;
  }
  else // диалог не нужен - копируем имя заранее известного файла
    wcsncpy(File, file, FILE_STRLEN);

  // определяем длину файла в байтах и выделяем буфер для чтения файла
  int file_bytes = get_file_length(File);
  Ar <int8> buf(file_bytes+2); // в конце на всяк. случ. дополнительное место для пары байт!
  int8 *file_buf = buf.memory();

  // теперь читаем файл без диалога ("молча")
  uint32 bytes_read;
  res = FileOpen(hwnd, file_buf, file_bytes, DlgTitle, File, DefExt, Filter, &bytes_read, DirName, true, false);
  if (!res) return false;
  copy_wo_ext(dflt_filename, File, 256); // запоминаем имя файла, выбранное юзером

  // печатаем имя загруженного файла
  actbox().PrintFileTitle(dflt_filename, DefExt, 1);

  // после конца прочитанных данных добавляем пару нулевых байт - метка конца широкой строки!
  file_buf[bytes_read] = file_buf[bytes_read+1] = 0;

  // проверяем что прочлось и если ок - используем информацию по назначению

  // содержимое файла в виде широкой строки текста, Unicode заголовок опускаем
  wchar_t *fstr = (wchar_t *)(file_buf+2);
  wchar_t *fstr_end = (wchar_t *)(file_buf+bytes_read); // первый символ за концом строки fstr

  // заменяем все "ненужные" знаки на пробелы
  wchar_t lets[4] = { L'.', L',', L'(', L')' }; // массив знаков для замены
  for (wchar_t *s = fstr; s < fstr_end; ++s)
    for (int i = 0; i < 4; ++ i) if (*s == lets[i]) *s = UNI_SPACE;

  // заменяем все концы строк UNI_CRLF на нули, попутно расставляя указатели на начало строк

  const int ADDS = 1; // служебные строки в начале файла: общие параметры

  // указатели на начало строк (до очередного символа CR/LF)
  Ar <wchar_t *> pstr(max_accords+ADDS);

  int i;
  for (i = 0; i < max_accords+ADDS; ++i)
  {
    pstr[i] = fstr;
    wstring ws(fstr);
    size_t n = ws.find(UNI_CRLF);
    if (n == npos) break; // конец строки не найден: это последняя строка
    // else найден, записываем туда нули и сдвигаем указатель
    fstr[n] = fstr[n+1] = UNI_NULL;
    fstr = fstr + n + 2;
    if (fstr >= fstr_end) break;
  }

  // pstr[ADDS] это 1-й аккорд
  max_accords = i-ADDS+1;
  if (max_accords <= 0) return false; // нет ни одного аккорда

  // Accords_Sequence_Comment.set_text( pstr[0] ); // для отладки показываем строку общих параметров

  // 1-я строка: вводим общие параметры секвенции аккордов
  wstring2 s = pstr[0];
  wstring str;
  int instr, trans, temp60; // темп - видимо ударов в минуту
  s >> str >> temp60 >> str >> trans >> str >> instr;

  ch.transposition = trans;
  ch.sequence_speed = 1;
  ch.dont_change_gm_instrument = 1;
  ch.instrument_number = instr;

  // следующая строка: вводим параметры 1-го и каждого следующего аккорда секвенции
  for (i = 0; i < max_accords; ++i)
  {
    s = pstr[i+ADDS];
    DichoticAccord acc;

    // вводим исходные ноты аккорда по центру или в левый канал при дихотическом сдвиге
    bool all_pause = true; // флаг аккорда-паузы
    for (int n=0; n < 5; ++n)
    {
      int note;
      s >> note;
      acc.dn[n].note = note;
      bool pause = (note==0); // флаг ноты-паузы
      acc.dn[n].pause = pause;
      acc.dn[n].pan = (dtrans==0)? 0:-1;
      if (!pause) all_pause = false;
    }
    acc.voices_number = 5;
    if (all_pause) acc.make_pause();

    // при дихотическом сдвиге добавляем в правый канал исходные ноты со сдвигом
    if (dtrans != 0 && !acc.is_pause() )
    {
      acc.voices_number = 10;
      for (int n=5; n < 10; ++n)
      {
        acc.dn[n].note = acc.dn[n-5].note + dtrans;
        acc.dn[n].pause = acc.dn[n-5].pause;
        acc.dn[n].pan = 1;
      }
    }

    acc.instrument = instr; // иначе там мусор!
    acc.volume = 100;
    int dur16;
    s >> dur16; // длительность аккорда, видимо в 1/16 целой ноты

    // рассчитываем длительность в секундах
    double dur1 = (dur16*60.)/(16.*temp60);
    // имитируем эту длительность в моих единицах
    acc.temp = 200;
    acc.duration = float2int(acc.temp * dur1);
    // очищаем комментарий аккорда
    acc.comment[0] = UNI_NULL;

    acc_sequence[i] = acc;
  }

  return true;
}

bool MusicFile::Open(SequenceHeader &ch, DichoticAccord *acc_sequence, int &max_accords, const wchar_t *file, const wchar_t *DirName)
{
  const wchar_t *DlgTitle = L"Accords Open";
  const wchar_t *DefExt = L"daccords";
  wchar_t File[FILE_STRLEN]; // буфер для полного имени файла, >= 256 байт!
  const wchar_t *Filter = L"Accords Files\0*.daccords\0"
                          L"Any Files\0*.*\0\0";

  bool need_dialog = ( wcscmp(file, UNI_NULL_STR) == 0 ); // диалог нужен если нет заранее известного файла
  bool res = 0;

  // если диалог нужен - сначала определяем в диалоге имя фала
  if (need_dialog)
  {
    // сначала будет дефолтное имя
    copy_wo_ext(File, dflt_filename, 256);
    res = FileOpen(hwnd, NULL, 0, DlgTitle, File, DefExt, Filter, NULL, DirName, false, true);
    if (!res) return false;
  }
  else // диалог не нужен - копируем имя заранее известного файла
    wcsncpy(File, file, FILE_STRLEN);

  // определяем длину файла в байтах и выделяем буфер для чтения файла
  int file_bytes = get_file_length(File);
  Ar <int8> buf(file_bytes+2); // в конце на всяк. случ. дополнительное место для пары байт!
  int8 *file_buf = buf.memory();

  // теперь читаем файл без диалога ("молча")
  uint32 bytes_read;
  res = FileOpen(hwnd, file_buf, file_bytes, DlgTitle, File, DefExt, Filter, &bytes_read, DirName, true, false);
  if (!res)
  {
    Mbox(L"Error open file", File);
    return false;
  }
  copy_wo_ext(dflt_filename, File, 256); // запоминаем имя файла, выбранное юзером

  // печатаем имя загруженного файла
  actbox().PrintFileTitle(dflt_filename, DefExt, 1);

  // после конца прочитанных данных добавляем пару нулевых байт - метка конца широкой строки!
  file_buf[bytes_read] = file_buf[bytes_read+1] = 0;

  // проверяем что прочлось и если ок - используем информацию по назначению

  // содержимое файла в виде широкой строки текста, Unicode заголовок опускаем
  wchar_t *fstr = (wchar_t *)(file_buf+2);
  wchar_t *fstr_end = (wchar_t *)(file_buf+bytes_read); // первый символ за концом строки fstr

  // проверяем наличие Unicode и daccords заголовков
  if ( *(fstr-1) != UNI_HDR ||
       wcsncmp( fstr, format_version_header, wcslen(format_version_header) ) != 0 )
  {
    Mbox(L"Unicode type or daccords header data error in file", File);
    return false;
  }

  // заменяем все концы строк UNI_CRLF на нули, попутно расставляя указатели на начало строк

  const int ADDS = 4; // служебные строки в начале файла (формат, комментарий, общие параметры, аббревиатуры)

  // указатели на начало строк (до очередного символа CR/LF)
  Ar <wchar_t *> pstr(max_accords+ADDS);

  int i;
  for (i = 0; i < max_accords+ADDS; ++i)
  {
    pstr[i] = fstr;
    wstring ws(fstr);
    size_t n = ws.find(UNI_CRLF);
    if (n == npos) break; // конец строки не найден: это последняя строка
    // else найден, записываем туда нули и сдвигаем указатель
    fstr[n] = fstr[n+1] = UNI_NULL;
    fstr = fstr + n + 2;
    if (fstr >= fstr_end) break;
  }

  // pstr[ADDS] это 1-й аккорд
  max_accords = i-ADDS+1;

  // 1-я строка: вводим и проверяем заголовок, версию формата и число аккордов
  wstring header, str;
  wstring2 s = pstr[0];
  s >> header >> version;
  if ( wcscmp(header.c_str(), format_version_header) != 0 ||
       version != dflt_version || max_accords <= 0) // неверный заголовок, версия формата или нет ни одного аккорда
  {
    Mbox(L"Error data format in file", File);
    return false;
  }

  actbox().rw_edit(Accords_Sequence_Comment).set_text( pstr[1] ); // 2-я строка: комментарий секвенции из файла в эдитбокс

  // 3-я строка: вводим общие параметры секвенции аккордов
  s = pstr[2];
  s >> str >> ch.transposition >> str >> ch.sequence_speed >>
       str >> ch.dont_change_gm_instrument >> str >> ch.instrument_number;

  // 4-я строка: аббревиатуры числовых параметров аккорда (не используем)

  // следующая строка: вводим параметры 1-го и каждого следующего аккорда секвенции
  for (i = 0; i < max_accords; ++i)
  {
    s = pstr[i+ADDS];
    DichoticAccord acc;

    s >> acc.instrument >> acc.duration >> acc.temp >> acc.voices_number;
    // приводим число голосов к рабочему диапазону данной версии программы
    mintestmax(0, acc.voices_number, DichoticAccord::MAX_ACC_VOICES);

    // если это не пауза - вводим все ноты/панорамы аккорда
    if ( !acc.is_pause() )
    {
      s >> acc.volume;
      for (int i=0; i < acc.voices_number; ++i)
      {
        // проверяем, нет ли паузы (литера X) вместо ноты голоса i
        wstring2 s2;
        s >> s2;
        if ( s2.compare(VOICE_PAUSE) == 0 )
        {
          acc.dn[i].pause = 1;
          continue;
        }
        acc.dn[i].pause = 0;
        s2 >> acc.dn[i].note;
        s >> acc.dn[i].pan;
      }
    }

    // вводим комментарий аккорда, пропуская возможно отсутствующий номер аккорда с префиксом ACCORD_NUMBER
    wstring2 s3, comm;
    // вводим в строку s3 одно слово, следующее за концом аккорда
    s >> s3;
    // если слово в s3 начинается с префикса номера аккорда, то комментарий будет в следующем слове!
    if ( s3.c_str()[0] == *ACCORD_NUMBER ) s >> comm; // вводим следующее слово
    else comm = s3; // если нет, то номер аккорда отсутствует, а слово в s3 - это и есть комментарий!

    // копируем comm в комментарий аккорда и добавляем ноль
    wcsncpy(acc.comment, comm.c_str(), DichoticAccord::COMMLEN-1);
    acc.comment[DichoticAccord::COMMLEN-1] = UNI_NULL;

    acc_sequence[i] = acc;
  }

  return true;
}

// #pragma warning(pop)

