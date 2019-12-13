# force_publisher - phidgetの歪データをpublishする

※このプログラムは，phidgetの歪ブリッジを用いて力センサの計測をし、MQTTに送信する。


## データパケットの送信速度（頻度）

Phidgetからデータが得られる速度（頻度）でPublishする．
速度はphidgetの計測速度に依存しており，最速で約8msの周期である。


## 送信されるデータパケットの内容

送信されるデータパケットは，以下の通り force_structure.hpp で規定されます．


```


## 現状はpublishのみ

本来はsubscriberの口を設けていろいろな設定ができるようにしたい．
現状は，ハードコーディング．

