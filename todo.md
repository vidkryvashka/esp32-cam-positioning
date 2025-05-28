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
I (22666) my_esp_things:   --  memory used 339344 / 4476712 (7.58 %) core 1
[0;32mI (22706) my_vision: image (24 x 18) corners: 5


***ERROR*** A stack overflow in task IDLE0 has been detected.


Backtrace: 0x4008354d:0x3ffbe8b0 0x4008f211:0x3ffbe8d0 0x4009047a:0x3ffbe8f0 0x4009160f:0x3ffbe970 0x40090584:0x3ffbe990 0x40090536:0x400882da |<-CORRUPTED


## or

[0;32mI (24856) my_photographer: Rectangle coordinates: Top left: (96, 143), width: 25, height: 21)
I (24856) my_photographer: Unpaused
I (24866) my_esp_things:   --  memory used 339012 / 4476712 (7.57 %) core 1
[0;32mI (24926) my_vision: image (25 x 21) corners: 3

Guru Meditation Error: Core  0 panic'ed (StoreProhibited). Exception was unhandled.

Core  0 register dump:
PC      : 0x40095033  PS      : 0x00060333  A0      : 0x80094894  A1      : 0x3ffcd4f0
A2      : 0x3fff1850  A3      : 0x000005e8  A4      : 0x00000280  A5      : 0x00000036
A6      : 0x00000000  A7      : 0x3ffe4364  A8      : 0x3fff1e3c  A9      : 0x00000001
A10     : 0x4d4d47b4  A11     : 0x0000000b  A12     : 0x00000000  A13     : 0x8d4c65f4
A14     : 0x3fff1858  A15     : 0x00000000  SAR     : 0x0000001a  EXCCAUSE: 0x0000001d
EXCVADDR: 0x8d4c65f4  LBEG    : 0x4008bed0  LEND    : 0x4008beec  LCOUNT  : 0xffffffff


Backtrace: 0x40095030:0x3ffcd4f0 0x40094891:0x3ffcd510 0x40083d2d:0x3ffcd530 0x40083e39:0x3ffcd550 0x40083e6d:0x3ffcd580 0x4008409d:0x3ffcd5a0 0x40097551:0x3ffcd5c0 0x400f595c:0x3ffcd5e0 0x400f6663:0x3ffcd600 0x400fa32c:0x3ffcd630 0x400fa902:0x3ffcd650 0x4010155a:0x3ffcd6c0 0x4010212c:0x3ffcd6e0 0x400f564d:0x3ffcd700 0x400f56fc:0x3ffcd720 0x40090075:0x3ffcd750

the problem was in vision.c // im1to3(img1_target, img3_target, wt * ht);
