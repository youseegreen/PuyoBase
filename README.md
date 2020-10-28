# ぷよべ～す

## ぷよReader
ぷよ動画からぷよ譜を生成して保存する

## ぷよPlayer
ぷよ譜の再生を行う


## HowToUse
Release版からPuyoBase.zipをダウンロードして解凍

### ぷよReader
1. videoフォルダ内にぷよ譜生成したい動画ファイルを置く
2. PuyoReader.exeを実行
3. puyofuフォルダ内にぷよ譜が保存される

### ぷよPlayer
1. puyofuフォルダ内にぷよ譜を設置(#ぷよ譜参照)
2. PuyoPlayer.exeを実行
3. ぷよ譜が再生される

## ぷよ譜について
### フォルダ構造
各試合ごとにフォルダを作成  
そのフォルダ内に  
player0, player1フォルダがあり、それぞれ  
0.csv, 1.csv...
のように設置する

### CSVファイルの中身
1行目：現在ぷよをどこに設置したか, 試合開始から何秒後に設置されたか,   
2行目：現在ぷよ1, 現在ぷよ2, ネクストぷよ1, ネクストぷよ2, ネクネクぷよ1, ネクネクぷよ2  
3~15行目：フィールド情報 (3行目：13段目、15行目：1段目)


## 詳細設定
config.csvで管理[TBA]




# for開発者

## 開発環境
- Windows
- Visual Studio
- c++
- OpenCV (c++, v3.4.1)

## 各ソースファイルの概要
###  PuyoReader Project 
- main.cpp  
対象フォルダ内の動画ファイルを取得。  
各動画に対してぷよ譜ジェネレータを実行  

