
#pragma once

// все операции с midi аппаратурой происходят через единственный объект этого класса
class MidiInterface
{
  HMIDIOUT hout; // хендл устройство вывода midi сообщений

  void MidiOut(DWORD msg) { if (hout) midiOutShortMsg(hout, msg); } // произвольное сообщения на открытое устройство

  // перед использованием midi канала в любых функциях надо проверить что он существует в железе:
  // эта ф-я возвращает true если канал chan предназначен для генерации звука ноты, иначе - false
  bool chan_ok(int chan)
  {
    if ( !in_range(MIDI_CHANNEL_MIN, chan, MIDI_CHANNEL_MAX) ) return false;
    return !in_range(NOT_USED_MIDI_CHANNEL_MIN, chan, NOT_USED_MIDI_CHANNEL_MAX);
  }

public:
  static const int MIDI_NOTE_MIN = 0, MIDI_NOTE_MAX = 127; // абсолютно мин. и макс. номера миди ноты
  static const int MIDI_NOTE_MIN_GEN =  12, // min номер миди ноты, которая реально озвучивается: частота 16.3 Гц!
                   MIDI_NOTE_MAX_GEN = 120; // max номер миди ноты, которая реально озвучивается: частота 8370 Гц!

  // мин. и макс. номера миди каналов, которые используются для генерации звука (кроме неиспользуемых)
  static const int MIDI_CHANNEL_MIN = 0, MIDI_CHANNEL_MAX = 15; // это для General Midi, но бывают и другие...
  // мин. и макс. номера неиспользыемых каналов: 9-й канал ударных, но может быть и 10-й канал ударных...
  static const int NOT_USED_MIDI_CHANNEL_MIN = 9, NOT_USED_MIDI_CHANNEL_MAX = 9;

  static const int GM_INSTRUMENTS_NUMBER = 128;
  static const wchar_t *PatchNames[GM_INSTRUMENTS_NUMBER]; // имена стандартных миди инструментов

  MidiInterface() : hout(0) {}

  ~MidiInterface() { CloseDevicesOut(); }

  void MidiOutChan(int chan, int b1, int b2, int b3) // произвольное канальное сообщение, проверяем канал!
  {
    if ( chan_ok(chan) ) MidiOut(((((255&b3) << 8) | (255&b2)) << 8) | ((255&b1) | (255&chan)));
  }

  // отпускание ноты note в канале chan со скоростью volume (иногда используется как скорость затухания)
  void MidiNoteOff(int chan, int note, int volume)
  {
    if ( in_range(MIDI_NOTE_MIN_GEN, note, MIDI_NOTE_MAX_GEN) ) MidiOutChan(chan, 0x80, note, volume);
  }

  // нажатие ноты note в канале chan со скоростью volume (при volume=0 это синоним отпускания)
  void MidiNoteOn(int chan, int note, int volume)
  {
    if ( in_range(MIDI_NOTE_MIN_GEN, note, MIDI_NOTE_MAX_GEN) ) MidiOutChan(chan, 0x90, note, volume);
  }

  void AllNotesOff(int chan) { MidiOutChan(chan, 0xB0, 123, 0); } // сброс всех нажатых нот в канале
  void AllNotesOff() { midiOutReset(hout); Sleep(10); } // сброс всех нажатых нот во всех каналах

  // сообщение панорамирования: Bn 10 pan (0=left, 64=mid, 127=right)
  void MidiPanChan(int chan, int pan) { MidiOutChan(chan, 0xB0, 10, pan); }

  void ChanPan(int chan, double pan)
  // панорамирование в миди канале chan
  // с условным параметром pan         -1,      0,        +1
  // соответствующим миди панораме 0=left, 64=mid, 127=right
  {
    int midipan = 64 + float2int(pan*64);
    mintestmax(0, midipan, 127);
    MidiOutChan(chan, 0xB0, 10, midipan);
  }

  void CloseDevicesOut() // Закрывание Out устройств
  {
    if (hout)
    {
      AllNotesOff();
      midiOutClose(hout); // Закроем устройство
      hout = NULL;
    }
  }

  // открытие устройства midi девайса с номером number
  bool OpenDeviceOut(int device_number)
  {
    hout = NULL;
    // пытаемся открыть нужное устройство без запроса уведомления
    MMRESULT res = midiOutOpen(&hout, device_number, 0, 0, 0);
    return (res == MMSYSERR_NOERROR);
  }

  void SendPatchChange(int chan, int instrument_number) // сообщение о смене инструмента в канале
  {
    MidiOutChan(chan, 0xC0, instrument_number, 0); // сообщение структуры Cn pp
  }

  void AllChanPatchChange(int instrument_number) // меняем midi инструмент во всех каналах
  {
    if (!hout) return;
    PATCHARRAY patches; // массив midi инструментов
    for (int i = 0; i < MIDIPATCHSIZE; i++) patches[i] = 0; // обнуляем массив инструментов

    patches[instrument_number] = 0xFFFF; // включим инструменты для всех каналов
    midiOutCachePatches(hout, 0, patches, MIDI_CACHE_ALL); // грузим инструменты

    for (int chan = MIDI_CHANNEL_MIN; chan < MIDI_CHANNEL_MAX; ++chan)
    {
      // не трогаем неиспользуемые midi каналы
      if ( in_range(NOT_USED_MIDI_CHANNEL_MIN, chan, NOT_USED_MIDI_CHANNEL_MAX) ) continue;
      SendPatchChange(chan, instrument_number);
    }
  }
};


// макс. число озвучиваемых голосов, в принятой модели "1 голос = 1 midi канал": сейчас = 15
const int MAX_MIDI_CHANNELS = (MidiInterface::MIDI_CHANNEL_MAX - MidiInterface::MIDI_CHANNEL_MIN + 1) - 
                              (MidiInterface::NOT_USED_MIDI_CHANNEL_MAX - MidiInterface::NOT_USED_MIDI_CHANNEL_MIN + 1);


