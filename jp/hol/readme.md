# IoT PnP Partner Workshop - ハンズオンラボ (HOL)

このページでは、HOL の内容、流れをご説明します

## (事前準備)

> [!IMPORTANT]  
> **(必須/重要) 開始までに以下作業をお願いします (反映に5-10分かかります)**  

以下のコマンドを Azure ポータル **Cloud Shell** で実行し、サービスを登録してください  

```bash
az provider register --namespace Microsoft.ContainerInstance  
az provider register --namespace Microsoft.DigitalTwins  
```

> [!IMPORTANT]  
> **(必須/重要) 2日目開始までに開発環境に以下のツールのインストールをお願いします**

- Azure CLI  
<https://docs.microsoft.com/ja-jp/cli/azure/install-azure-cli-windows?tabs=azure-cli>
- .NET Core 3.1 Runtime  
<https://dotnet.microsoft.com/download/dotnet-core/3.1>
- Windows 環境に対して、何らかの方法で Git を利用出来る環境の準備を推奨します  

  Githubツールの例
  - Git (CLI)
  <https://git-scm.com/>
  - TortoiseGit (GUI)  
  <https://tortoisegit.org/>  

***  

## (DAY.1 - 115 min)

### サンプルソリューションのデプロイ  

"PaaS - Azure Services で体験" で用いるサンプル IoT ソリューションを Azure Resource Management (ARM) テンプレートを使用してデプロイします  

[(手順) ARM テンプレートを用いてデプロイ](./deployment/) (30 min)

- ARM テンプレートとは?
- デプロイの流れ - サービスのデプロイと権限の設定
- ソリューション アーキテクチャ

### SaaS - Azure IoT Central で体験  

IoT Plug and Play をサポートした SaaS : Azure IoT Central を使用して IoT Plug and Play 対応・非対応デバイスを接続した際のユーザーエクスペリエンスを体験します  
IoT Plug and Play を使ってデバイスがソリューションに供給するメタデータ = Device Model を活用した例の一つ  

[(手順) Azure IoT Central + Wio Terminal の接続](./wioterminal/) (45 min)  

- PaaS vs SaaS - Azure IoT Central は PaaS の集合体  
- IoT PnP 対応/非対応の違い (デバイスモデル有り無し)  
- IoT Plug and Play 非対応 デバイスの接続  
- IoT Plug and Play 対応 デバイスの接続  
- *EXTRA. PnP 非対応デバイス向け、デバイステンプレートの作成*  

### PaaS - Azure Platform Services で体験

PaaS ベースのソリューションを使用してテクニカルディープダイブ。ソリューション側の仕組みを見てみます  

[(手順) IoT Plug and Play 対応デバイス (Wio Terminal) の接続](./wioterminal/sample.md) (30 min)

- Model Resolution (Service SDK) とは  
- Model Parser とは
- *EXTRA. IoT Plug and Play 非対応 デバイスの接続*  
- *EXTRA. 光センサー対応版の接続*  

### Azure Digital Twins デモ (DAY.2 のご紹介)  

### (宿題) : シミュレーターを使ってソリューションの動作を確認  

DAY.2 で使用するシミュレーターデバイスの動作確認をしてください  

[(手順) IoT Plug and Play デバイス シミュレーター (温度計) の実行と接続](./simulator/)  

***  

## (DAY2 : 55 min) - **12/11 解放**  

### PaaS で体験 (応用編)

1. Azure Digital Twins (ADT) と接続する
    - ADT と繋ぐ、とは?
    - [(手順) ADT Explorer 環境構築 (10min)](./adt/explorer.md)
    - [(手順) ADT シナリオの確認 (10min) - シミュレーターを実行、部屋の色が変わるのを確認](./adt/)  
    *EXTRA. Wio Terminal + 光センサー で確認*  

***
**お疲れ様でした!!**  
