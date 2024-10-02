# 概要

概念検証(POC1)とは、IMUセンサー(https://www.adafruit.com/product/4517) をM5StampS3に取り付け、これがロケトーンを構成する基礎的要素になります。また、ロケットの発射情報を入手したり、将来的にはロケットデータを収集するという点で、シンプルなNFC（Near Field Communication）コンセプトを含んでいます。

![20240923_212805000_iOS](https://github.com/user-attachments/assets/3c8f7d34-4a6a-43d5-a5d4-5e59dfea9548)


## ゴール

・パワー消費量を検証
・ネットワークの範囲を検証　(ESP-NOW, BLE使用)
・温度の検証　
・センサー用のビジュアルダッシュボードの試作品をくつる（多分Project MagNET使用）
・大まかな設計に必要な部品コストの見積もりの入手
・上記のインプット情報を概念実証２（POC2）に入力

## 概念実証の特徴

・ブラウザを使用する9DOFセンサーのvisual viewer
・ロケット発射データ、多分ロケットキャプテン情報を集めるNFC接触コンセプト


## 配線図

![image](https://github.com/user-attachments/assets/4495456d-c55b-4881-a245-9b784c7f47c2)


M5StampS3にはgroveコネクターがありません。簡単に配線するには以下の図を参照してください。

![image](https://github.com/user-attachments/assets/bfd17168-aba6-4e2e-a48f-4c7ffa76f113)


Qwiicコネクターはブレッドボードを経由してM5StampS3へ接続するために使われます。もしリンク先のようなコネクターがなければ、M5StampS3に直接はんだ付けができます。
https://www.sparkfun.com/products/17912

この場合に使われているIMU（慣性計算ユニット）はAdafruit LSM6DSOX + LIS3MDL - Precision 9 DoF IMU - STEMMA QT / Qwiic。正しく配線されていれば、電源LEDが緑色に点灯しているはずです。

## 設定

・事前にM5StampS3の設定が必要になります。以下の手順にしたがって、アンドロイドM5Stack開発のための設定を行って下さい。

・LEDをインストールすることで、M5StampS3の設定を確認してください。
・Adafruit Sensor Labをインストールする必要があります。以下のリンクを参照してください。　　https://learn.adafruit.com/adafruit-sensorlab-gyroscope-calibration/install-sensorlab

・まずAdafruit Sensor Calibrationを実行し、 sensor_calibration_write とsensor_calibration_read も実行します(検証するため)。以下のリンクを参照してください。
https://github.com/IoTone/RockeTone/blob/main/POC/poc1/sensor_calibration_write/sensor_calibration_write.ini
https://github.com/IoTone/RockeTone/blob/main/POC/poc1/sensor_calibration_read/sensor_calibration_read.ini

・the calibrated_orientation directoryの中で、calibrated_orientation修正版を実行してください。これには必要なPINとヘッダーが定義されています。少しトリッキーなのは、ドキュメントにはとうすればうまくいくか、断片的でまとまっていないからです。

・以下のリンクにあるファイルをあなたのウェブサーバーにコピーしてください。

注意：
ウェブサーバーはHTTPSを使用する必要があります。最新のクロームブラウザは、httpsが配信されないと機能しません。

## 実演

以下のテストサイトを使ってください。
https://iotoneai.site/rocketone/poc1/modelviewer
これにはArduinoのセットアップとcalibrated_orientationサンプルの実行が必要です。



