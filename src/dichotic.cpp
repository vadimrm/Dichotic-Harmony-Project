
#include "stdafx.h"
#include "stdafx2.h"

pair<int,int> DichoticAccord::accord_notes_min() const
// ищет минимальный номер ноты аккорда: возвращает её индекс и сам номер ноты, для паузы индекс < 0
{
  int index = -1, min_note = 0;

  for (int i = 0; i < voices_number; ++i)
  {
    if ( !dn[i].pause )
    {
      int note = dn[i].note;
      if (index < 0)
      {
        min_note = note;
        index = i;
      }
      else if (min_note > note)
      {
        min_note = note;
        index = i;
      }
    }
  }

  return make_pair(index, min_note);
}

int DichoticAccord::unique_notes_num() const
// возвращает число уникальных нот аккорда, игнорируя панораму
{
  // копируем все ноты аккорда в массив с исключением совпадающих нот
  int notes[MAX_ACC_VOICES];
  int num = 0;
  for (int i = 0; i < voices_number; ++i)
  {
    if (dn[i].pause) continue;

    int note = dn[i].note;
    for (int n = 0; n < num; ++n)
    {
      if (note == notes[n]) goto next;
    }
    notes[num++] = note;

    next: continue;
  }

  return num;
}

bool DichoticAccord::mirror_accords(const DichoticAccord &ac1, const DichoticAccord &ac2)
// возвращает true если все ноты 2-х аккордов одинаковы, но имеют зеркальные панорамы (т.е. с инверсией знака)
// причём номера голосов одинаковых нот тоже должны быть одинаковы (или они должны быть паузами голоса)
{
  if (ac1.voices_number != ac2.voices_number) return false;
  for (int i = 0; i < ac1.voices_number; ++i)
  {
    if ( ac1.dn[i].pause )
    {
      if ( ac2.dn[i].pause ) continue;
      else return false; // несовпадение пауз голоса
    }
    // else нота
    if ( ac1.dn[i].note != ac2.dn[i].note ) return false; // несовпадение нот
    // else ноты одинаковы
    if ( ac1.dn[i].pan != -ac2.dn[i].pan ) return false; // незеркальность панорам
  }
  return true;
}

bool DichoticAccord::identic_accords(const DichoticAccord &ac1, const DichoticAccord &ac2)
// возвращает true если все ноты 2-х аккордов и их панорамы одинаковы
{
  if (ac1.voices_number != ac2.voices_number) return false;
  for (int i = 0; i < ac1.voices_number; ++i)
  {
    if ( ac1.dn[i].pause )
    {
      if ( ac2.dn[i].pause ) continue;
      else return false; // несовпадение пауз голоса
    }
    // else нота
    if ( ac1.dn[i].note != ac2.dn[i].note ) return false; // несовпадение нот
    // else ноты одинаковы
    if ( ac1.dn[i].pan != ac2.dn[i].pan ) return false; // несовпадение панорам
  }
  return true;
}

bool DichoticAccord::test_unisons() const
// возвращает true если в аккорде есть унисон(ы) (хоть и с разной панорамой)
{
  for (int i = 0; i < voices_number; ++i)
  {
    if (dn[i].pause) continue;
    int note = dn[i].note;
    for (int j = i+1; j < voices_number; ++j)
    {
      if (dn[j].pause) continue;
      if (note == dn[j].note) return true;
    }
  }
  return false;
}

void DichoticAccord::remove_unisons()
// удаляет все лишние голоса-унисоны (с любой панорамой) и голоса-паузы, сжимая аккорд
{
  for (int i = 0; i < voices_number; ++i)
  {
    if (dn[i].pause) continue;
    int note = dn[i].note;
    for (int j = i+1; j < voices_number; ++j)
    {
      if (dn[j].pause) continue;
      if (note == dn[j].note) dn[j].pause = 1;
    }
  }

  remove_pauses();
}

void DichoticAccord::remove_pauses()
// удаляет голоса-паузы, сжимая аккорд
{
  for (int i = voices_number-1; i >= 0; --i)
  {
    if (!dn[i].pause) continue;
    // else найдена пауза голоса
    // сдвигаем всё что сверху на неё
    for (int j = i; j < (voices_number-1); ++j)
      dn[j] = dn[j+1];
    // уменьшаем число голосов
    --voices_number;
  }
}

bool DichoticAccord::identic_voices(const DichoticAccord &acc) const
// возвращает true если все голоса *this и acc аккордов одинаковы с учётом панорамы
{
  if (acc.voices_number != voices_number) return false;
  for (int i = 0; i < voices_number; ++i)
    if ( !dn[i].identic_note(acc.dn[i]) ) return false;
  return true;
}

bool DichoticAccord::test_conflicts(bool make_no_conflicts)
// анализ аккорда и возврат наличия конфликтов диссонанса (совпадение нот с учётом панорамы)
// если make_no_conflicts=true устраняет конфликты путём отключения совпадающих нот
{
  bool conflict_diss = false; // флаг конфликта
  for (int i = 0; i < voices_number; ++i)
  {
    if ( dn[i].pause ) continue;

    int note = dn[i].note;
    int pan = dn[i].pan;

    // находим совпадения с другими нотами аккорда (начиная с [i+1]-й ноты) и анализируем их панорамы
    for (int j = i+1; j < voices_number; ++j)
    {
      if (dn[j].pause) continue;

      int note2 = dn[j].note;
      if (note2 != note) continue;
      //  note2 == note, анализируем соотношение панорам обоих голосов

      int pan2 = dn[j].pan;
      switch (pan)
      {
        case -1: // левый край конфликтует с левым и с центром
          if (pan2 == -1 || pan2 == 0) conflict_diss = true;
          break;

        case  0: // центр конфликтует со всеми тремя точками
          conflict_diss = true;
          break;

        case +1: // правый край конфликтует с правым и с центром
          if (pan2 == +1 || pan2 == 0) conflict_diss = true;
          break;
      }

      if (conflict_diss) // найден конфликт
      {
        if (make_no_conflicts) // надо исправлять конфликты
        {
          dn[j].pause = 1; // из ноты [j] делаем паузу голоса
          conflict_diss = false; // сбрасываем конфликт и продолжаем работу с остальными нотами
        }
        else // поиск конфликтов без исправления: при первом же конфликте выходим из функции
        {
          return true;
        }
      }
    }
  }

  return conflict_diss;
}

bool DichoticAccord::transpose_accord(int base_note)
// аккорд транспонируется т.о. чтобы нижняя его нота стала равна base_note, возвращает false если аккорд - пауза
{
  // определяем миним-й номер ноты аккорда
  pair<int,int> index_note = accord_notes_min();
  int index = index_note.first; // индекс нижней ноты
  if ( index < 0 ) return false; // аккорд - пауза!

  int min_note = index_note.second; // номер нижней ноты
  int dnote = base_note - min_note; // нужная транспозиция аккорда
  // транспонируем все ноты аккорда
  for (int i = 0; i < voices_number; ++i) dn[i].note += dnote;
  return true;
}

void DichoticAccord::accord_manipulator()
// входной аккорд пропускается через все контролы диалог бокса, влияющие на его звучание и подаётся на выход
{
  // изменение номеров нот аккорда
  if ( actbox().get_check(One_Octave).checked_state() ) // приводим диапазон нот к 1-й октаве
  {
    // нижняя нота аккорда сохраняет свою высоту, а все остальные опускаются вниз так чтобы быть
    // в пределах одной октавы вверх от неё

    // определяем миним-й номер из всех нот аккорда
    pair<int,int> index_note = accord_notes_min();
    int index = index_note.first; // индекс нижней ноты
    if ( index >= 0 ) // аккорд - не пауза!
    {
      int min_note = index_note.second; // номер нижней ноты

      for (int i = 0; i < voices_number; ++i)
      {
        // у каждой ноты кроме нижней меняем номер
        if ( !dn[i].pause && i != index )
        {
          int dnote = dn[i].note - min_note; // интервал от нижней ноты, >= 0
          int new_note = min_note + dnote%12; // приводим интервал к одной октаве
          dn[i].note = new_note;
        }
      }
    }
  }

  // изменение панорам нот и отключение нот по 3-м манипуляторам панорамы лев/центр/прав
  for (int i = 0; i < voices_number; ++i)
  {
    if ( !dn[i].pause )
    {
      int pan = dn[i].pan;

      // сначала меняем панораму ноты
      if ( !actbox().get_check(Dichotic_Output).checked_state() ) pan = 0;
      if (  actbox().get_check(Swap_Output).checked_state()     ) pan = -pan;

      dn[i].pan = pan;

      // затем проверяем "разрешена" ли такая панорама, если нет - то делаем паузу голоса
      if ( ( pan == -1 && actbox().get_check(Switchon_Left_Pan).unchecked_state() ) ||
           ( pan ==  0 && actbox().get_check(Switchon_Mid_Pan).unchecked_state() ) ||
           ( pan == +1 && actbox().get_check(Switchon_Right_Pan).unchecked_state() ) )
      {
        dn[i].pause = 1;
      }
    }
  }

  // производим отключение совпадающих нот с учётом их панорамы
  if ( actbox().get_check(With_Unisons).unchecked_state() )
  {
    test_conflicts(true);
  }

  // сортируем голоса аккорда в порядке возрастания номеров нот
  if ( actbox().get_check(Show_Sorted_Notes).checked_state() ) sort_notes();
}

void DichoticAccord::NotesGetPos(int*notes1,int&num1,int*notes2,int&num2,int*notes3,int&num3,int add) const
// анализирует позиции звучащих голосов аккорда и записывает их ноты в 3 массива:
// notes1[num1] для левого края панорамы, notes2[num2] для центра, notes3[num3] для правого края
// при записи к номерам всех нот прибавляется константа add
{
  // готовим массивы по 3-м точкам панорамы
  num1 = num2 = num3 = 0;
  for (int i = 0; i < voices_number; ++i)
  {
    if (dn[i].pause) continue;

    int note = dn[i].note + add;
    int pan = dn[i].pan;

    if (pan <  0) notes1[num1++] = note;
    if (pan == 0) notes2[num2++] = note;
    if (pan >  0) notes3[num3++] = note;
  }
}

int DichoticAccord::Dissonances(int *notes, int num)
{
  int dis = 0;
  for (int i = 0; i < num; ++i)
  {
    // чтобы не брать каждый интервал дважды - внутр. цикл по верхним индексам
    for (int j = i+1; j < num; ++j)
    {
      // интервал между голосами нот [i], [j]
      int iv = abs(notes[i] - notes[j]);
      dis += MusicDialogBoxCtrl::diss_rating[iv];
    }
  }
  return dis;
}

int DichoticAccord::CrossDissonances(int *notes1, int num1, int *notes2, int num2)
{
  int dis = 0;
  for (int i1 = 0; i1 < num1; ++i1)
  {
    for (int i2 = 0; i2 < num2; ++i2)
    {
      int iv = abs(notes1[i1] - notes2[i2]);
      dis += MusicDialogBoxCtrl::diss_rating[iv];
    }
  }
  return dis;
}

int DichoticAccord::dissonance()
// возвращает суммарный диссонанс аккорда, не совсем верный если есть совпадающие голоса: см. test_conflicts()
{
  int notes1[MAX_ACC_VOICES], notes2[MAX_ACC_VOICES], notes3[MAX_ACC_VOICES];
  int num1=0, num2=0, num3=0;

  NotesGetPos(notes1, num1, notes2, num2, notes3, num3);

  // определяем суммарный диссонанс аккорда с учётом панорамы

  // суммируем диссонансы всех интервалов внутри каждой группы голосов
  int dis = Dissonances(notes1, num1) + Dissonances(notes2, num2) + Dissonances(notes3, num3);

  // крайние голоса панорамы между собой не дают диссонансов,
  // а голос в середине панорамы дает 1/2 диссонансов с голосами на краях панорамы
  int dis2 = CrossDissonances(notes1, num1, notes2, num2) + CrossDissonances(notes3, num3, notes2, num2);

  // т.к. все диссонансы чётные - дробной части при делении на 2 не будет!
  return (dis + dis2/2);
}

int DichoticAccord::WriteCombinationPans(int N, int K, DichoticAccord acc1h, vector<DichoticAccord> &acc2h)
// добавляет все варианты преобразования N-голосного аккорда acc1h в массив acc2h[], которые определяются
// нахождением всех сочетаний из N голосов аккорда acc1h по K, причём эти K голосов записываются в левую
// точку панорамы, а остальные голоса - в правую точку;
// возвращает количество записанных вариантов;
// для правильной работы в acc1h не должно быть "пустых" голосов (голосов-пауз);
{
  // определяем все сочетания из N по K
  vector<int> Cnk;
  int ncomb = arC_n_k( N, K, Cnk);
  // цикл по всем сочетаниям
  for (int nacc = 0; nacc < ncomb; ++nacc)
  {
    // очередной вариант сочетания
    int combination = Cnk[nacc];
    // сканируя N младших бит сочетания используем 0-е биты как маркер левой панорамы, а единичные - как правой
    for (int nbit = 0; nbit < N; ++nbit)
    {
      int pan = get_bit_of_number(combination, nbit)==0? -1:+1;
      // теперь меняем панорамы голосов 1H аккорда для превращения его в 2H
      acc1h.dn[nbit].pan = pan;
    }
    acc2h.push_back( acc1h );
  }
  return ncomb;
}

int DichoticAccord::WriteCombinationVoices(int N, int K, DichoticAccord acc1, vector<DichoticAccord> &acc2arr)
// добавляет все варианты преобразования N-голосного аккорда acc1 в массив acc2[], которые определяются
// нахождением всех сочетаний из N голосов аккорда acc1 по K
// возвращает количество записанных вариантов;
{
  // определяем все сочетания из N по K
  vector<int> Cnk;
  int ncomb = arC_n_k( N, K, Cnk);
  // цикл по всем сочетаниям
  for (int nacc = 0; nacc < ncomb; ++nacc)
  {
    DichoticAccord acc2 = acc1;
    // очередной вариант сочетания
    int combination = Cnk[nacc];
    // сканируя N младших бит сочетания используем 0-е биты как маркер незвучания голоса
    for (int nbit = 0; nbit < N; ++nbit)
    {
      if ( 0 == get_bit_of_number(combination, nbit) )
        acc2.dn[nbit].pause = 1;
    }
    acc2arr.push_back( acc2 );
  }
  return ncomb;
}

int DichoticAccord::DichoticConverter(DichoticAccord acc1h, vector<DichoticAccord> &acc2h)
// функция получения всех 2h вариантов панорамы одного 1h аккорда, возвращает количество вариантов
{
  acc2h.clear();

  // готовим все 2H варианты исходного 1H аккорда
  acc1h.remove_pauses(); // уничтожаем голоса-паузы
  acc1h.zero_pans(); // обнуляем все панорамы (они будут по центру)
  int vnum = acc1h.voices_number;

  if ( vnum <= 1 ) // одноголосный аккорд
  {
    acc2h.push_back( acc1h ); // копируем 1h аккорд в 2h
    return 1;
  }
  // else vnum >= 2

  int vnum2 = vnum/2; // половинки аккорда по краям панорамы, vn2 >= 1
  int vnum1 = vnum%2; // остаточный голос - в центр панорамы, vn1 = 0,1; при =0 голоса нет!

  int accords_num = 0; // полное количество вариантов аккордов

  if (vnum1 == 0) // чётное число голосов
  {
    int N = vnum; // общее число голосов аккорда
    int K = vnum2; // число голосов для левой точки панорамы (все остальные будут в правой)
    // записываем в массив acc2h все варианты (сочетания из N по K) исходного аккорда acc1h
    accords_num = WriteCombinationPans(N, K, acc1h, acc2h);
  }
  else // vnum1 = 1, нечётное число голосов аккорда: 3,5,7...
  {
    int index = 0; // индекс в массиве acc2h[] начала очередной порции 2H вариантов
    for (int n = 0; n < vnum; ++n)
    {
      // убираем один из голосов аккорда и записываем все сочетания остальных голосов
      DichoticAccord a1h = acc1h;
      a1h.dn[n].pause = 1; // делаем n-й голос паузой
      a1h.remove_pauses(); // уничтожаем голос-паузу
      int N = vnum - 1; // общее число голосов усечённого аккорда
      int K = vnum2;
      // добавляем в массив acc2h все варианты усечённого аккорда a1h
      int accnum = WriteCombinationPans(N, K, a1h, acc2h);
      // затем добавляем во все варианты убранный голос (в центр панорамы)
      for (int i = index; i < (index+accnum); ++i)
      {
        acc2h[i].add_note( acc1h.dn[n] ); // все голоса acc1h были по центру!
      }
      // апдейт индекса для определения начала следующей порции вариантов
      index += accnum;
      // аккумулируем полное количество вариантов аккордов
      accords_num += accnum;
    }
  }

  return accords_num;
}

