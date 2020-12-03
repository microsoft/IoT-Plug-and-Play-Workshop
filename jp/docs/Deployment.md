# サンプル IoT ソリューションを展開する

この文章は 以下のことを説明します :

- サンプル IoT ソリューションの展開方法
- Raspberry Pi エミュレーターを用いた基本的な操作案内

## 必須

- Azure サブスクリプション  
    もし Azure Subscription をお持ちでない場合には、無償 (1年有効)   
    <https://azure.microsoft.com/free/>  
    サブスクリプションのオーナーか管理者である必要があります  
- ウェブブラウザ (Microsoft Edge など) がインストールされた PC

## 1. 展開開始する

以下の **Deploy to Azure** ボタンをクリックしてください  

<a href="https://portal.azure.com/#create/Microsoft.Template/uri/https%3A%2F%2Fraw.githubusercontent.com%2Fdaisukeiot%2FIoT-Plug-and-Play-Workshop-Deploy%2Fmain%2Fiotplugandplayworkshop.json" target="_blank"><img src="./media/deploy-to-azure.svg"/></a>

> [!TIP]  
> この文章を参照し続けるため、ボタンを右クリックして **リンクを新しいタブで開く** または **リンクを新しいウィンドウで開く** とするのを推奨します  

## 2. クラウドシェルを開始する

> [!NOTE]  
> Azure Time Series Insights (TSI) は Azure Active Directory を用いた暗黙的なアクセス許可を必要とします  
> 現状 Azure Resource Manager (ARM) テンプレートで Azure Active Directory (AAD) を操作する簡単な方法はありませんので、一時的な回避策として、いくつか手動で処理していただく必要があります  
> ステップ 2 ~ 4 は、その (TSI の要件に関連した) 一時的な回避策です  

Azure サービスプリンシパルは、ユーザーが作成したアプリ、サービス、及び自動化ツールが特定の Azure リソースにアクセスする際に用いる セキュリティID であり、ユーザーそれぞれのアクセス許可を管理する代わりに、Web サイトはサービスプリンシパルを利用して、TSI からのデータにアクセスします

1. **カスタムデプロイ** ページを開いている状態で、**Cloud Shell** を立ち上げます

    ![Deployment 01](media/Deployment-01.png)

> [!TIP]  
> お使いのサブスクリプションにおいて過去 **Cloud Shell** を使ったことが無い場合、ストレージアカウントの作成、ストレージのマウントを促すウィンドウが表示されます  

1. **Cloud Shell** 左上の 環境選択 が **Bash** になっているのを確認します

    ![Deployment 02](media/Deployment-02.png)

    **Bash** でない場合、**Bash** を選択して **確認** をクリックすると、再確認を促すウィンドウが表示されます  

    ![Deployment 03](media/Deployment-03.png)

## 3. サービスプリンシパル を作成する

**Cloud Shell** に、以下のコマンドを丸ごとコピー＆ペーストしてください

```bash
wget -q https://raw.githubusercontent.com/microsoft/project15/master/Deploy/tsi-setup.sh -O ./tsi-setup.sh --no-cache && chmod +x tsi-setup.sh && ./tsi-setup.sh
```

カスタム デプロイ ページの **Run this command** フィールドにも同じ内容が表示されていますので、そちらをコピーいただいても構いません

> [!TIP]  
> **Cloud Shell** に貼り付けるには、**Cloud Shell** 上で右クリックして **貼り付け** をクリックしてください

![Deployment 04](media/Deployment-04.png)

コマンド実行が完了すると、**Cloud Shell** 上で4行の出力が確認できます (出力されるまで約1分かかります)  

![Deployment 05](media/Deployment-05.png)

> [!TIP]  
> 権限エラーに遭遇した場合には、管理者またはアカウントのオーナー権を持つユーザーになっているかどうか確認してください  

> [!TIP]  
> もし複数のサブスクリプションの際には、上記コマンドを実行する前に、正しい (目的の) アカウントを設定してください :
>  
> ```bash  
> az login  
> az account set --subscription <Your Subscription Name or ID>  
>```

## 4. テンプレートに サービスプリンシパル 情報を入力する

それではここで、テンプレートにサービスプリンシパル ID とパスワードを入力します  

先ほど作成された **Service Principal App Id**, **Service Principal Password**, **Service Principal Tenant Id**, そして **Service Principal Object Id** を、テンプレートの該当項目にそれぞれコピー＆ペーストします

![Deployment 06](media/Deployment-06.png)

## 5. デプロイを開始する

1. **サブスクリプション** を選択する (もし複数お持ちの場合)
1. **新規作成** をクリックして、新しい **リソース グループ** を作成する  

    入力例: **MySolution**

    ![Deployment 07](media/Deployment-07.png)

1. お好きな **リーション** を選択し, **確認および作成** をクリックします  

    ![Deployment 08](media/Deployment-08.png)

    > [!TIP]  
    > ARM テンプレートで展開される一部のサービスは、サービス側の提供状況に基づき、リージョンが固定されています

1. 全ての必要なパラメーターが入力されていることを確認後、**作成** をクリックしてデプロイを開始します  

    ![Deployment 09](media/Deployment-09.png)

1. 15分ほど待つとデプロイが完了します

    ![Deployment 10](media/Deployment-10.png)

## 5. デプロイ後の作業を行う

1. デプロイが完了したら、画面上の **Outputs** をクリックします

    ![Deployment 11](media/Deployment-11.png)

1. Click the button to copy **Post Deployment Command** フィールドをボタンをクリックしてコピーして、**Cloud Shell** に貼り付けます

    ![Deployment 12](media/Deployment-12.png)

    > [!NOTE]  
    > **Cloud Shell** がタイムアウトしている場合は、**再接続** をクリックします  

1. **Enter** を入力して、コマンドを実行します

    > [!NOTE]  
    > このコマンドは何も出力/表示しません  

1. 実行完了したら **Web Site Address** フィールドを新しいブラウザのウィンドウまたはタブにコピー＆ペーストし、サンプルポータルサイトを確認します

    ![Deployment 13](media/Deployment-13.png)

## サンプルポータルサイト

サンプルポータルサイトは4つの大項目を有します

![Deployment 14](media/Deployment-14.png)

- Azure Time Series Insights (TSI)  

    デバイスからのテレメトリーやイベントは TSI に送信されます  
    今回のサンプルでは、温度と湿度のデータを TSI 内に保存されたデータから読み取り、ラインチャートとして表示します

    TSI 詳細 :  
     <https://docs.microsoft.com/en-us/azure/time-series-insights/overview-what-is-tsi>

- Azure Maps

    ロケーションベースのサービス (例: GPSなど) を統合するソリューションで使用します  
    今回のサンプルでは、地図だけをシンプルに表示します

- Device Management  

    デバイス情報を登録、削除、取得します  

    > [!NOTE]  
    > 今回のサンプルには Device Provisioning Service と対話する手段が実装されていません

- Device Telemetry and Events  

   バックエンドのシステムからリアルタイムデータを受信する方法の一例として
   全てのデバイスのテレメトリーやイベントを受信して表示します

## Send Temperature from Raspberry Pi Simulator

1. Click on **+Add Device**

    ![Deployment 15](media/Deployment-15.png)

1. Give a name to a new device, then click **+Add**  

    E.g. RP-Simulator

    > [!TIP]  
    > The name is called **Device ID**.  
    > A case-sensitive string (up to 128 characters long) of alphanumeric characters plus certain special characters: - . + % _ # * ? ! ( ) , : = @ $ '

    ![Deployment 16](media/Deployment-16.png)

1. Make sure the device just created is selected in the list, and the device is in **Enabled** state.  

    Click on **Copy Connection String**.  
    We will use this later with Raspberry Pi simulator.

    ![Deployment 17](media/Deployment-17.png)

1. Click on **Raspberry Pi Simulator**

    ![Deployment 18](media/Deployment-18.png)

    This will open a new browser tab.

    ![Deployment 19](media/Deployment-19.png)

1. Browse to line #15 in the top right pane, and replace **[Your IoT hub device connection string]** with the connection string

    ![Deployment 20](media/Deployment-20.png)

    ![Deployment 21](media/Deployment-21.png)

    The line should look like :

    ```c
    const connectionString = 'HostName=Hub-q7fec.azure-devices.net;DeviceId=RP-Simulator;SharedAccessKey=)(*)()(*)()()=';
    ```

1. Click **Run** to start sending Temperature and Humidity data

    ![Deployment 22](media/Deployment-22.png)

1. Open the web app from the solution, then click **Refresh** to see temperature and humidity data in line graph

    ![Deployment 23](media/Deployment-23.png)

## Connecting a real device

If you have Raspberry Pi and SenseHat, please visit <https://github.com/daisukeiot/RP4-SenseHat-PnP/tree/WIP>

## Next Step

- Developers : Learn more technical details of the Open Platform Open Platform Developer Guide : [Architecture Overview](../Developer-Guide/Architecture-Overview.md)

[Project 15 from Microsoft - Open Platform](../README.md)
