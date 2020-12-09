# IoT PnP Partner Workshop - ハンズオンラボ (HOL)

このページでは、HOL の内容、ステップをご説明します

## (事前準備)
**(必須/重要) 開始までに以下作業をお願いします (反映に5-10分かかります)**  
以下のコマンドを Azure ポータル **Cloud Shell** で実行し、サービスを登録してください  
```
az provider register --namespace Microsoft.ContainerInstance  
az provider register --namespace Microsoft.DigitalTwins  
```

**(必須/重要) 2日目開始までに以下のインストールをお願いします**  

- Azure CLI  
https://docs.microsoft.com/ja-jp/cli/azure/install-azure-cli-windows?tabs=azure-cli
- .NET Core 3.1 Runtime  
https://dotnet.microsoft.com/download/dotnet-core/3.1

**(推奨) 2日目開始までに以下のインストールを出来ましたらお願いします (Git 環境)**  

- Git (CLI)    
https://git-scm.com/  
- TortoiseGit (GUI)  
https://tortoisegit.org/  
Windows 環境に対して、何らかの方法で Git を利用出来る環境の準備を推奨します  

***  

## (DAY.1 - 115 min)
## "PaaS - Azure Services で体験" で用いるソリューションのデプロイ  
1. サンプル IoT ソリューション のデプロイ
    - ARM テンプレート とは?  
    - デプロイの流れ - サービスのデプロイと権限の設定
    - ソリューション アーキテクチャ
    - [(手順) ARM テンプレートを用いて デプロイ](./deployment/) (30 min)

## SaaS - Azure IoT Central で体験  
1. [(手順) Azure IoT Central + Wio Terminal の展開](./wioterminal/) (45 min)  
    - PaaS vs SaaS - Azure IoT Central は PaaS の集合体  
    - IoT PnP 対応/非対応の違い (デバイスモデル有り無し)  
    - IoT Plug and Play 非対応 デバイスの接続  
        EXTRA. デバイステンプレートの作成
    - IoT Plug and Play 対応 デバイスの接続  

## PaaS - Azure Services で体験
1. サンプル IoT ソリューション トライアル
    - Model Resolution (Service SDK) とは  
    - Model Parser とは
    - [(手順) IoT Plug and Play 対応デバイス (Wio Terminal) の接続](./wioterminal/sample.md) (30 min)  
    EXTRA. IoT Plug and Play 非対応 デバイスの接続  
    EXTRA. 光センサー対応版の接続  

## ADT デモ (DAY.2 のご紹介)  

## (宿題)  
1. シミュレーターを使ってソリューションの動作を確認  
    - [(手順) IoT Plug and Play デバイス シミュレーター (温度計) の実行と接続](./simulator/)  

***  

## (DAY2 : 55 min) - **12/11 解放**  
## PaaS で体験 (応用編)
1. Azure Digital Twins (ADT) と接続する
    - ADT と繋ぐ?
    - [(手順) ADT Explorer 環境構築 (10min)](./adt/explorer.md)
    - [(手順) ADT シナリオの確認 (10min) - シミュレーターを実行、部屋の色が変わるのを確認](./adt/)  
    EXTRA. Wio Terminal + 光センサー で確認  

***
**お疲れ様でした!!**  
