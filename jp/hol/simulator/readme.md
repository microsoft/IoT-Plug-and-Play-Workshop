# IoT Plug and Play デバイス シミュレーター (温度計)

このドキュメントでは、[Thermostat Device Model](https://github.com/Azure/iot-plugandplay-models/blob/main/dtmi/com/example/thermostat-1.json) と連携して動作するシミュレーター (コード) の展開方法を説明します

特徴は以下の通りです  
- サンプル IoT ソリューション ハンズオン対応  
- IoT Plug and Play 認証デモ対応  
- 対称鍵、及び X.509 証明書認証をサポート  

## 開発環境をセットアップ  
Windows 10 環境を想定しています

### 要件

- Linux 環境  
    - Windows Subsystem for Linux (WSL) ( [Ubuntu 18.04 LTS](https://www.microsoft.com/ja-jp/p/ubuntu-1804-lts/9n9tngvndl3q) でテスト済 )  
- ツールチェーン   
以下の [手順](#install-pre-requisite) に沿って必要なツールとライブラリをインストールしてください

### 環境の事前構築
1. WSL(2) のインストール
    1. *スタートボタン* で右クリック、**Windows Power Shell (管理者)** をクリックします  
    以下のコマンドを順番に実行します  
        ``` Windows Power Shell
        dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart  

        dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart  
        ```
        実行後、PCを再起動します
    1. WSL2 Linux カーネル更新プログラムを適用します  
    http://aka.ms/wsl2kernelmsix64  
    からダウンロード、実行します  
    1. 再度 **Windows Power Shell (管理者)** をクリックします  
    以下のコマンドを実行します  
        ``` Windows Power Shell
        wsl --set-default-version 2   
        ```
    1. Microsoft Store から Ubuntu イメージをインストールします  
    [Ubuntu 18.04 LTS](https://www.microsoft.com/ja-jp/p/ubuntu-1804-lts/9n9tngvndl3q) にアクセス、**入手** をクリックします  
    Microsoft Store アプリが立ち上がりますので、再度 **入手** をクリックします  
    Microsoft アカウントでログインしていない場合は、サインインが促されます  
    (サインインは必須ではありませんので、*必要ありません* でスキップでも構いません)  

    1. インストールが完了したら **起動** をクリックします  
    UNIX ユーザーアカウントの設定が求められますので、任意に入力します  
    (Windows のユーザーとは関連性ありません)  
    Ubuntu 18.04 LTS が起動します  

1. 必要なツールとライブラリをインストールします
    ```bash
    sudo apt-get update && \
    sudo apt-get install -y git cmake build-essential curl libcurl4-openssl-dev libssl-dev uuid-dev
    ```  
1. このレポジトリを Clone します
    ```bash
    git clone https://github.com/microsoft/IoT-Plug-and-Play-Workshop.git && \
    cd IoT-Plug-and-Play-Workshop/demo/SimpleThermostat/script/
    ```  
1. セットアップスクリプト (内容は以下の通り) を実行します  
    - Azure IoT C Device SDK をクローン  
    - セルフサインの X.509 証明書を生成  
    - 温度計 シミュレーター アプリをビルド  

    ```bash
    ./setup.sh
    ```

    > [!TIP]  
    > setup.sh は以下のオプションに対応しています  
    >
    > ```bash
    > Usage: ./setup.sh [-c] [-r] [-v] [-h]
    >  -c : (C)lean up environment.  Deletes Azure IoT SDK C and other folders and files
    >  -r : (R)e-create new X509 certificates
    >  -v : (V)erbose
    >  -h : (H)elp menu
    >```

## 温度計 シミュレーター アプリの利用  

1. 実行用シェルスクリプトを任意のテキストエディタで開きます  
    ```bash
    nano run.sh  
    ```
1. DPS Scope ID、Device ID、対称鍵を入力して保存します
    ```bash
    # export DPS_X509=1
    unset DPS_X509
    export DPS_IDSCOPE='0ne001D591B'
    export DPS_DEVICE_ID='simtemp'
    export DPS_SYMMETRIC_KEY='3j+Vi2CtCidXQijTc/zYX8bHQbcE1vVN/LIgCWtJT8Q='
    ```  
1. スクリプトを実行します
    ```bash
    ./run.sh  
    ```  
    
