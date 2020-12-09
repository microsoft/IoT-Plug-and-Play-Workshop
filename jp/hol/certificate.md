## 新しいプロジェクトを Azure Certified Device ポータルで作成する

1. [Azure Certified Device Portal](https://certify.azure.com) にアクセスします
1. **+ 新しいプロジェクトの作成** をクリックします  
    > プロジェクト名:    
    > デバイス名:  
    > デバイスの種類:  
    > デバイスクラス:  

    を入力し、**次へ** をクリックします  

1. 有効にする認定を選択します  
    > Azure Certified Device  
    > IoT プラグアンドプレイ  

    を選択し、**作成** をクリックします  

1. 作成したプロジェクトを選択します  
    **接続とテスト** をクリックします  

    **テストの実行>** をクリックします  

### テスト手順  
認証情報として、*対称キー* または *X.509 証明書* が利用可能です  

1. *対称キー* で行う場合
    1. 認証方法を選択します (*対称キー* を選択)  
    1. *はい、モデルはパブリック モデル レポジトリで使用できます* にチェックを入れます  
    1. **登録の追加** をクリックします
    1. *コードの更新* に表示された項目を ./run.sh に反映します
        > デバイス プロビジョニング サービス (DPS) ID スコープのコピーとコードへの貼り付け  
        > デバイス ID のコピーとコードへの貼り付け  
        > 対称キーのコピーとコードへの貼り付け  

    1. ./run.sh を実行します  
    1. **接続** をクリックします  

1. *X.509 証明書* で行う場合  
    1. 認証方法を選択します (*X.509 証明書* を選択)  
    1. X.509 証明書ファイル の **ファイル選択** をクリックします  
    *IoT-Plug-and-Play-Workshop\demo\SimpleThermostat\cmake\certs* の  
    *(マシン名).cer.pem*  
    を選択します  
        > [!TIP]  
        > WSL2 上のファイルは エクスプローラーからアクセス可能です  
        > エクスプローラーのパスに *\\\wsl$* を入力してください  
    1. *はい、モデルはパブリック モデル レポジトリで使用できます*  にチェックを入れます
    1. *コードの更新* に表示された項目を ./run.sh に反映します
        > デバイス プロビジョニング サービス (DPS) ID スコープのコピーとコードへの貼り付け  

        > [!NOTE]  
        > X.509 証明書を用いる場合は、  
        > export DPS_X509=1  
        > がコメントアウト(行の最初に#)されていないことを確認してください  

    1. **登録の追加** をクリックします
    1. ./run.sh を実行します
    1. **接続** をクリックします

これ以降は対称キー、X.509 証明書で共通です  

1. **次へ** をクリックします  
1. **次へ** をクリックします  
1. **テストの実行** をクリックします  
    テストの状態 において、
    *成功しました。[完了] をクリックして次の手順に進んでください*  
    と表示されたら完了です  

### X.509 Certificate

![X509](./images/Portal-X509.png)

1. Browse to Azure Certified Device portal
1. Create a new project
1. Select `Connect + test`
1. Click `Run tests` for IoT Plug and Play
1. Upload `./cmake/new-device.cert.pem` to the portal, then click `Add Enrollment`
1. Select the checkbox saying `Yes, my models are available in the public model repository.`
1. Click `Add Enrollment`
1. `set` DPS_IDSCOPE to the ID Scope provided by the portal
1. `Set` DPS_X509 (or remove/comment out `unset DPS_X509`)

    Example :

    ```bash
    # Set ID Scope for DPS
    export DPS_IDSCOPE='0ne000FFA42'

    # Detemines type of DPS attestation
    # set DPS_X509 for X.509
    # unset DPS_X509 for Symmetric Key
    export DPS_X509=1
    # unset DPS_X509

    # for Symmetric Key Provisioning
    # Get Device ID and Symmetric Key from the certification portal
    export DPS_DEVICE_ID=''
    export DPS_SYMMETRIC_KEY=''
    ```

### Symmetric Key

![SymmetricKeys](./images/Portal-SymmetricKeys.png)

1. Browse to Azure Certified Device portal
1. Create a new project
1. Select `Connect + test`
1. Click `Run tests` for IoT Plug and Play
1. Select `Symmetric Keys` for Authentication method
1. Select the checkbox saying `Yes, my models are available in the public model repository.`
1. Click `Add Enrollment`
1. Open ./run.sh with your favorite text editor
1. `set` DPS_IDSCOPE to the ID Scope provided by the portal
1. `unset` DPS_X509
1. `set` DPS_DEVICE_ID and DPS_SYMMETRIC_KEY

    Example :

    ```bash
    # Set ID Scope for DPS
    export DPS_IDSCOPE='0ne000FFA42'

    # Detemines type of DPS attestation
    # set DPS_X509 for X.509
    # unset DPS_X509 for Symmetric Key
    export DPS_X509=1
    unset DPS_X509

    # for Symmetric Key Provisioning
    # Get Device ID and Symmetric Key from the certification portal
    export DPS_DEVICE_ID='8cfea9d9-3844-4e62-b6cc-c6be324453df'
    export DPS_SYMMETRIC_KEY='dw88wVI0yd2E86cwN+PkczUNhDG4tGRPT2GwphsQJnE='
    ```

## Running the demo

1. Run the app with  

    ```bash
    ./run.sh
    ```

1. Confirm the device is provisioned to IoT Hub

    Example :

    ```bash
    =======================================================
    Info: Common Name : IoTPnPCertDemoX_15945
    Info: Provisioning Status: PROV_DEVICE_REG_STATUS_CONNECTED
    Info: Provisioning Status: PROV_DEVICE_REG_STATUS_ASSIGNING
    Info: Registration Information received from DPS
    Info: IoT Hub     : certsvc-hub-prod-westus2.azure-devices.net
    Info: Device Id   : IoTPnPCertDemoX_15945
    Info: Connecting to IoT Hub
    =======================================================
    Reading X.509 Certificate
    Issuer     : /CN=Azure IoT Hub CA Cert Test Only
    Subject    : /CN=IoTPnPCertDemoX_15945
    =======================================================
    Info: IoTHubDeviceClient_LL_CreateFromDeviceAuth Success
    Info: Connected to iothub : certsvc-hub-prod-westus2.azure-devices.net
    ```

1. Continue the certification test by clicking `Connect` button

    ![Connect](./images/Portal-Connect.png)

1. Confirm that the portal should find the device, then click `Next`

    ![Connected](./images/Portal-Connected.png)

1. The simulator does not require any parameter changes.  Click `Next` 

    ![Connect Parameter](./images/Portal-Connected-Param.png)

1. Start automated test by clicking `Run tests`

    ![Run Tests](./images/Portal-Run-Tests.png)

1. In a few minutes, the test should complete with `Pass` status

    ![Test Pass](./images/Portal-Test-Pass.png)


[ワークショップ トップページに戻ります](../)  
