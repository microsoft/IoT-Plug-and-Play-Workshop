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
## PaaS - Azure Services の組み合わせで体験 で用いるソリューションのデプロイ  
1. サンプル IoT ソリューション のデプロイ (30 min)
    1. ARM テンプレート とは  
    1. デプロイの流れ - サービスのデプロイと権限の設定は別で
    1. [(手順) ARM テンプレートを用いて デプロイ](./deployment/)
    1. アーキテクチャ 箇条書き(ただし複雑過ぎないこと) *[!deck]*  

## SaaS - Azure IoT Central で体験  
1. [(手順) Azure IoT Central + Wio Terminal の展開](./wioterminal/) (45 min)  
    1. PaaS vs SaaS - Azure IoT Central は PaaS の集合体  
    1. IoT PnP 対応/非対応の違い (デバイスモデル有り無し)  
    1. IoT Plug and Play 非対応 デバイスの接続  
    1. IoT Plug and Play 対応 デバイスの接続

## PaaS で体験
1. サンプル IoT ソリューション トライアル (30 min)  
    1. Model Resolution (Service SDK)  
    1. Model Parser  
    1. [(手順) IoT Plug and Play 対応デバイス (Wio Terminal) の接続](./wioterminal)

    EXTRA. IoT Plug and Play 非対応 デバイスの接続 

1. ADT デモ (ワークショップ DAY.2) [!demo]
       
### (DAY3 - PREP)  
1. 以下のシミュレーターを使ってサンプルソリューションの動作を確認
    - [(手順) IoT Plug and Play デバイス シミュレーター (温度計) の実行と接続](./simulator/)

### (DAY3 : 55 min) - **12/11 解放**  
## PaaS で体験 (応用編)
1. Azure Digital Twins と接続する
    1. ADT と繋ぐってどういうこと? *[!deck]*
    1. ADT Explorer 環境構築 (10min) *[!page]*
    1. ADT シナリオの確認 (10min) *[!page]*
        - シミュレーター 実行 して部屋の色が変わるのを確認

    EXTRA. Wio Terminal + 光センサー で確認  

お疲れ様でした!!  
