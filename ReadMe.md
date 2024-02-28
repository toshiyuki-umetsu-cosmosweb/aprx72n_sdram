# 概要

RX72N で SDRAMを使うためのサンプル。

# 使用ボード

(株)アルファプロジェクト社 AP-RX72N-0Aボード
https://www.apnet.co.jp/product/rx/ap-rx72n-0a.html

# 何を見るの？

* 内蔵RAMとSDRAM間で、DMA転送をするだけの単純なプログラム。
  DMACドライバは FIT dmaca を使用し、CGドライバは使わない。
* 内蔵RAM -> SDRAM DMA転送開始時にLD1(P95)を点灯。DMA転送完了で消灯。
             オシロスコープで信号の H 期間を見れば、かかった時間がわかる
* SDRAM -> 内蔵RAM DMA転送開始時にLD2(P96)を点灯。DMA転送完了で消灯。
             オシロスコープで信号の H 期間を見れば、かかった時間がわかる
* デバッグ用入出力で USB-Serial を使った

転送サイズは内蔵RAMのサイズを考え、512KBとした。