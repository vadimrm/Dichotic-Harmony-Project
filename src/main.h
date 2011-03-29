﻿/*

  =Возможно надо:
  -чекбокс разрешения запуска нескольких экземпляров приложения, по умолчанию сброшенный;
   надо переадресовывать параметры нового запуска в старый, вплоть до прерывания текущих
   действий (по кр. мере - проигрывания) и выполнения работы нового запуска!
  -чекбокс (активный по умолчанию) разрешения запуска "только 1-го экземпляра приложения"
   (если надо - сделать это при помощи мапфайла), чтобы запуск системой .daccords файлов переадресовывался
   в то приложение, которое уже запущено.

  -запись последнего состояния всех регуляторов в конфиг-файл и чтение его при старте,
   кнопка RESET для сброса всех регуляторов в состояние при первом запуске программы
  -сделать pan с плав. запятой, с округлением до +-1 только для индикаторов, см код 2h_converter
  -поддержать ударные инструменты (они кодируются отрицательными нотами)

  -еще один вариант дихотического преобразования готовой музыки Меломана:
   механическая разводка многоголосных аккордов по 2-м или 3-м точкам панорамы (без учёта диссонансов
   или с минимальным их учётом)
  -добавить хоткей D загрузки .daccords файлов с дихотическим сдвигом?

  -в перспективе вывод аккорда на приложение Б. при помощи мапфайла, который создаётся и работает только
   с 1-м экземпляром А.

  =Программа для облегчения сочинения новой музыки:
  -надо не сортировать все аккорды по степени отличия от опорного, а делать ограниченный
   (напр. 10...100 штук) набор аккордов с заданным (суммарным) отличием: 0, 1, 2, 3 ... п/т.
   при этом каждый аккорд должен сдвигаться на +-10 п/т чтобы пытаться получить это заданное
   отличие... и надо это отличие выводить в индикаторе!

  -при операциях перебора аккордов учитывать то, что банки аккордов должны быть удвоены путём
   добавления к каждому аккорду такого же, но с инверсией панорамы!

  -см. подробную схему в тетради Projects-III, стр. 100:
  -убрать с диалогбокса голоса 12-15 (или даже 7-15?), на их место поставить новые элементы
   управления для подбора "музыкальных" секвенций аккордов:
   автозагрузка 6-ти банков аккордов, чекбоксы участия каждого банка для отбора "совпадений",
   2-й массив для отобранных из банков аккордов, "совпадающих" с опорным, слайдер для выбора
   аккорда из этого массива (миним-й размер массива 10...100, регулировать другим слайдером),
   слайдер транспозиции выбранного аккорда на +-12 п/т, кнопка и хоткей для генерации звука
   этого аккорда, кнопка добавления его в конец 1-го массива (основной секвенции аккордов)...

  -идея сочинения музыки ("Дихотическая Рапсодия"):
   кажется когда-то я писал про сочинение обычной многогол-й музыки путём подбора аккордов,
   минимально отличающихся друг от друга (напр. только 1-й нотой)...
   теперь то же самое можно сделать с дихотическими аккордами: делаем базу "хороших" дихотических
   аккордов (5-6 и более голосов), затем ставим какой-то первый аккорд, затем анализируем степень
   отличия от него всех других аккордов базы, причем для каждого аккорда берём несколько его
   относительных положений с различным сдвигом по высоте (а также с инверсией панорамы).
   Из них выбираем 10...20 штук с наименьшим отличием и затем окончательно на слух выбираем один,
   кот. будет 2-м аккордом, и т.д.

  =Программа "автоматический оптимизатор панорамы" (приложение Б):
  -миди вход и миди выход
  -в перспективе вход из приложения А. через мап-файл (но это вместо миди входа)

  -метод панорамирования на случай любого числа голосов:
   для данного числа точек панорамы и данного паттерна разбивки количества голосов по ним делаем
   полный перебор всех вариантов панорамирования, запоминаем тот. диссонансы и затем сортируем
   их по возрастанию, выбираем любой вариант по его порядковому номеру (после сортировки), для
   одинаковых диссонансов делаем доп. выбор по возрастанию/убыванию средней высоты групп нот
   аккорда слева направо (если средняя высота одинакова то сравниваем мин. ноты, затем макс. ноты)

  -при наличии пары (или нескольких разных пар) одинаковых нот обязательно разводим ноты пары по
   разным каналам, а если есть >2-х одинаковых нот, то игнорируем 3-ю и последующие одинаковые!

  -добавить чекбокс/режим оптимизации количества точек панорамы (2 или 3) с минимумом диссонанса!

  -ввести сначала по одному фикс. паттерну панорамы для каждого конкретного размера аккорда!
  -ввести по несколько паттернов панорамы для каждого конкретного размера аккорда, переключатели
   паттернов вручную и режим автоматического выбора наилучшего паттерна!

  -сделать 2 движковых регулятора компрессии скорости нажатия клавиш - "мин" и "макс" скорость,
   чтобы полный диапазон скоростей 1...127 транслировался в этот промежуток мин...макс
   а также 2 чекбокса "сцепления"  регуляторов (когда мин=макс)
   и откл. компрессии (когда мин=1, макс=127)

  -проверить - можно ли заставить играть обработанный миди поток синтезатор-источник первоначальных
   данных, путём его вывода в то же самое устройство (т.е. midi in = midi out)

  =Когда-нибудь:
  -непрерывный континуум позиций панорамирования в соответствии с интенсивностью диссонансов,
   сила диссонанса пропорциональна силе отталкивания голосов...
  -регулятор сдвига высоты всех голосов в одном канале звука, напр. в левом: от 0 до 100 центов!

*/

#pragma once

extern const int VER_NUM;
extern int act_dialog_index;
extern const wchar_t *copyright, *website;

extern class MusicDialogBoxCtrl dbox[];
extern class MidiInterface MIDII;

inline MusicDialogBoxCtrl &act_dbox(); // ссылка на текущий активный диалог бокс

