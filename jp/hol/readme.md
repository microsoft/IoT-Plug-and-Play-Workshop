# IoT Plug and Play Partner Workshop ハンズオン (DAY-2/3)

このドキュメントでは、ハンズオンの具体的な手順を説明します  
ハンズオンは以下のステップで進行します  

### (DAY 2 - PREP)
**(重要) 以下作業を開始前にお願いします (反映に5-10分かかります)**  
以下のコマンドを **Cloud Shell** で実行し、サービスを登録してください  
```
az provider register --namespace Microsoft.ContainerInstance  
az provider register --namespace Microsoft.DigitalTwins  
```

### (DAY 2 - 1h55min)
## PaaS で体験 で用いるソリューションのデプロイ  
1. サンプル IoT ソリューション のデプロイ (30 min)
    - ARM テンプレート とは  
    - デプロイの流れ - サービスのデプロイと権限の設定
    - ソリューション アーキテクチャ
    - [(手順) ARM テンプレートを用いて デプロイ](./deployment/)

## SaaS - Azure IoT Central で体験  
1. [(手順) Azure IoT Central + Wio Terminal の展開](./wioterminal/) (45 min)  
    - PaaS vs SaaS - Azure IoT Central は PaaS の集合体  
    - IoT PnP 対応/非対応の違い (デバイスモデル有り無し)  
    - IoT Plug and Play 非対応 デバイスの接続  
        EXTRA. デバイステンプレートの作成
    - IoT Plug and Play 対応 デバイスの接続  
        EXTRA. 光センサー対応版の接続

## PaaS で体験
1. サンプル IoT ソリューション トライアル (30 min)  
    - Model Resolution (Service SDK) とは  
    - Model Parser とは
    - [(手順) IoT Plug and Play 対応デバイス (Wio Terminal) の接続](./wioterminal/sample.md)  
    EXTRA. IoT Plug and Play 非対応 デバイスの接続  

1. ADT デモ (ワークショップ DAY.2) [!demo]
       
### (DAY3 - PREP)  
1. 以下のシミュレーターを使ってサンプルソリューションの動作を確認
    - [(手順) IoT Plug and Play デバイス シミュレーター (温度計) の実行と接続](./simulator/)

### (DAY3 : 55 min) - **12/11 解放**  
## PaaS で体験 (応用編)
1. Azure Digital Twins と接続する
    - ADT と繋ぐってどういうこと? *[!deck]*
    - ADT Explorer 環境構築 (10min) *[!page]*
    - ADT シナリオの確認 (10min) - シミュレーターを実行、部屋の色が変わるのを確認 *[!page]*  
    EXTRA. Wio Terminal + 光センサー で確認  

***
**お疲れ様でした!!**  
***


