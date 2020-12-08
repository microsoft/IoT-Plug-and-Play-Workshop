# IoT Plug and Play Partner Workshop ハンズオン

このドキュメントでは、ハンズオンの具体的な手順を説明します  
ハンズオンは以下のステップで進行します  

(DAY1 : 1h55min)

1. [サンプル IoT ソリューション のデプロイ](./deployment/readme.md) (15min)
    1. ARM テンプレート とは [!TAHIRAI] 
    1. 大きな流れ(1PAGE で) [!TAHIRAI] > ユーザー権限は別よ(AADに絡む)
    1. デプロイ開始 (所要時間20分くらいを想定)
    1. 実際やってることの箇条書き(ただし複雑過ぎないこと) [!TAHIRAI] > project 15 からぱくる?

    3page 作る

準備

1. [Azure IoT Central + Wio Terminal の展開](./wioterminal/readme.md)  (30min)  
    1．PaaS vs SaaS [!PAGE] CENTRAL は PaaS の集合体 [!TAHIRAI]
    1. IoT PnP 対応非対応の違い(デバイスモデル有り無し) [!PAGE] (Device SDK)
    1. IoT Plug and Play 非対応デバイスの接続  
    1. IoT Plug and Play 対応デバイスの接続

    1page 作る

SaaS で体験

1. サンプル IoT ソリューション トライアル (30min)  
    1．Model Resolution ソリューションで PnP 対応するって ? (Service SDK) [!PAGE]   [!TALK]
    1. Model Parser
    1. IoT Plug and Play 対応デバイスの接続  (DPS) -> Wio + DPS な手順書 [!TAHIRAI]

    EXTRA. 非 PnP

PaaS で体験＆Deep Dive

1. ADT デモ (ワークショップ DAY.2 への)

応用編（前振り）
 
    宿題：
    以下のシミュレーターを使ってサンプルソリューションの動作を確認
    - [IoT Plug and Play デバイス シミュレーター (温度計)](./simulator/readme.md)

(DAY2 : 55min)

1. ADT と繋ぐ
応用編（本番）

1. ADT と繋ぐってどういうこと? [!PAGE] [!TAHIRAI] アーキテクチャ図?
1. ADT Explorer 環境構築 (10min)
1. Node ()
1. ADT シナリオの確認 (10min)
    シミュレーター実行
    Azure Indoor Map 色変更を確認

    EXTRA. 
    Wio + 光