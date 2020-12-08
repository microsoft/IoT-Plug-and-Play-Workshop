# IoT Plug and Play Partner Workshop ハンズオン (DAY-2/3)

このドキュメントでは、ハンズオンの具体的な手順を説明します  
ハンズオンは以下のステップで進行します  

### (DAY 2 - PREP)
**(重要) 事前にやっておいていただきたいこと]**  
以下のコマンドを **Cloud Shell** で実行し、サービスを登録してください  
```
az provider register --namespace Microsoft.ContainerInstance  
az provider register --namespace Microsoft.DigitalTwins  
```

### (DAY 2 - 1h55min)
## PaaS - Azure Services の組み合わせで体験、の下準備  
1. [サンプル IoT ソリューション のデプロイ](./deployment/readme.md) (30 min)
    1. ARM テンプレート とは [!deck] 
    1. デプロイの流れ - サービスのデプロイと権限の設定は別で [!deck]
    1. デプロイ 開始
    1. アーキテクチャ 箇条書き(ただし複雑過ぎないこと) [!deck] > refer project 15 ?
    > (require 3pages)

## SaaS - Azure IoT Central で体験  
1. [Azure IoT Central + Wio Terminal の展開](./wioterminal/readme.md)  (30min)  
    1．PaaS vs SaaS - Azure IoT Central は PaaS の集合体 [!deck]
    1. IoT PnP 対応/非対応の違い(デバイスモデル有り無し) [!deck] > refer device SDK?
    1. IoT Plug and Play 非対応 デバイスの接続  
    1. IoT Plug and Play 対応 デバイスの接続
    > (require 2 deck)

## PaaS で体験
1. サンプル IoT ソリューション トライアル (30min)  
    1. Model Resolution ソリューションで PnP 対応するって ? (Service SDK) [!deck]  
    1. Model Parser [!deck]
    1. IoT Plug and Play 対応デバイスの接続  (DPS) -> Wio + DPS な手順書 [!page]
    > (require 2 decks 1page)

    EXTRA. IoT Plug and Play 非対応  

1. ADT デモ (ワークショップ DAY.2) [!demo]
       
### (DAY3 - prep) 
1. 以下のシミュレーターを使ってサンプルソリューションの動作を確認
    - [IoT Plug and Play デバイス シミュレーター (温度計)](./simulator/readme.md)

### (DAY3 : 55min)
## PaaS で体験 (応用編)
1. ADT と接続する
1. ADT と繋ぐってどういうこと? [!deck] > アーキテクチャ図?
1. ADT Explorer 環境構築 (10min) [!page]> node.js ありき
1. ADT シナリオの確認 (10min) [!page]
    - シミュレーター実行
    - Azure Indoor Map 色変更を確認

    EXTRA. Wio + 光  

    > (require 1 deck 2 pages)

お疲れ様でした!!  
