# todo

* take frame, current_frame mutexes race conditions dead loop не працює photographer після виділення rectangle

* серво живуть своїм життям


* зробити визначення переміщення об'єкта порівнянням зображень







## (необов'язково)

* зробити компресію стрічок з frontend/index_src.html

* влаштувати живлення від батарейки


## fanfacts
* дисторсія (від лат. distorsio, distortio — викривлення) — аберація оптичних систем, при якій лінійне збільшення змінюється по полю зору, порушуючи подібність між об'єктом та його зображенням.


## done to commit -m









# problems

## 1
I (49064) my_photographer: take_photo, got fb
[0;31mE (55064) my_photographer: take_photo() failed to take frame_mutex
E (55064) my_photographer: task couldn't take_photo
[0;31mE (57994) task_wdt: Task watchdog got triggered. The following tasks/users did not reset the watchdog in time:
[0;31mE (57994) task_wdt:  - IDLE1 (CPU 1)
E (57994) task_wdt: Tasks currently running:
E (57994) task_wdt: CPU 0: IDLE0
E (57994) task_wdt: CPU 1: tiT
E (57994) task_wdt: Print CPU 1 backtrace
ets Jul 29 2019 12:21:46



## 2
I (70926) my_photographer: take_photo, got fb
Guru Meditation Error: Core  1 panic'ed (LoadProhibited). Exception was unhandled.

Core  1 register dump:
PC      : 0x40090b82  PS      : 0x00060033  A0      : 0x80091615  A1      : 0x3ffcd600
A2      : 0xa5a5a5a5  A3      : 0xa5a5a5a5  A4      : 0x3ffb8b84  A5      : 0x00060023
A6      : 0x00000001  A7      : 0x00000000  A8      : 0x00000002  A9      : 0x00000005
A10     : 0x3ffd025c  A11     : 0x8000b83f  A12     : 0x00000001  A13     : 0x3ffb89e0
A14     : 0x3f007d5d  A15     : 0x00000001  SAR     : 0x00000010  EXCCAUSE: 0x0000001c
EXCVADDR: 0x8000b883  LBEG    : 0x4008bed0  LEND    : 0x4008beec  LCOUNT  : 0xffffffff


Backtrace: 0x40090b7f:0x3ffcd600 0x40091612:0x3ffcd620 0x40090584:0x3ffcd640 0x40090536:0x3ffcd660 0x400f5bdb:0x00000000 |<-CORRUPTED



## 3
(61096) my_photographer: take_photo, got fb
[0;32mI (62096) my_esp_things:   --  memory used 539552 / 4476680 (12.05 %) core 0
[0;31mE (67096) my_photographer: take_photo() failed to take frame_mutex
E (67096) my_photographer: task couldn't take_photo
[0;32mI (67296) my_webserver:  -- sent pkts

assert failed: heap_caps_free heap_caps_base.c:69 (heap != NULL && "free() target pointer is outside heap areas")


Backtrace: 0x4008354d:0x3fff1090 0x4008f211:0x3fff10b0 0x40097541:0x3fff10d0 0x40083d81:0x3fff11f0 0x40097571:0x3fff1210 0x400d6cf2:0x3fff1230 0x40090075:0x3fff1480






## after inapropriate refactoring does this

I (4566) my_photographer: take_photo, got fb
[0;32mI (4776) my_webserver: WebSocket handshake
[0;32mI (5576) my_webserver: in the end of form meta
0;32mI (5576) my_webserver: formed metadata_json
I (5576) my_webserver: sent metadata
[0;32mI (6996) my_webserver:  -- sent pkts
[0;32mI (7126) my_vision: image (240 x 240) corners: 60, threshold 23

I (7126) my_photographer: take_photo, got fb
Guru Meditation Error: Core  0 panic'ed (LoadProhibited). Exception was unhandled.

Core  0 register dump:
PC      : 0x40090b82  PS      : 0x00060033  A0      : 0x80091615  A1      : 0x3ffbe970
A2      : 0xa5a5a5a5  A3      : 0xa5a5a5a5  A4      : 0x80090835  A5      : 0x00060023
A6      : 0x00000001  A7      : 0x00000000  A8      : 0x00000002  A9      : 0x00000005
A10     : 0x3fff1228  A11     : 0x8000b83f  A12     : 0x00000000  A13     : 0x3ffb89e0
A14     : 0x3a227822  A15     : 0x00000000  SAR     : 0x0000001e  EXCCAUSE: 0x0000001c
EXCVADDR: 0x8000b883  LBEG    : 0x00000000  LEND    : 0x00000000  LCOUNT  : 0x00000000


Backtrace: 0x40090b7f:0x3ffbe970 0x40091612:0x3ffbe990 0x40090584:0x3ffbe9b0 0x40090536:0x400882da |<-CORRUPTED


##
I (12046) my_photographer: take_photo, got fb
[0;32mI (12456) my_webserver: WebSocket handshake
Guru Meditation Error: Core  1 panic'ed (InstrFetchProhibited). Exception was unhandled.

Core  1 register dump:
PC      : 0x8000b83f  PS      : 0x00060730  A0      : 0x8013a4ca  A1      : 0x3fff0c80
A2      : 0x2c38323a  A3      : 0x3f400894  A4      : 0x3fff0f04  A5      : 0x0000000a
A6      : 0x7fffff80  A7      : 0x3fff0ecc  A8      : 0x8008ba0d  A9      : 0x8000b83f
A10     : 0x3fff1580  A11     : 0x8000b83f  A12     : 0x3f400894  A13     : 0x0000000a
A14     : 0x00000001  A15     : 0x3fff1440  SAR     : 0x00000004  EXCCAUSE: 0x00000014
EXCVADDR: 0x8000b83c  LBEG    : 0x4008c6d5  LEND    : 0x4008c6e5  LCOUNT  : 0xfffffeed


Backtrace: 0x4000b83c:0x3fff0c80 0x4013a4c7:0x3fff0cb0 0x40139e5d:0x3fff0cd0 0x4012ef3d:0x3fff0ff0 0x4015a775:0x3fff1020 0x40095de9:0x3fff1050 0x400d6b5d:0x3fff10a0 0x400d6c0b:0x3fff1240 0x40090075:0x3fff1490