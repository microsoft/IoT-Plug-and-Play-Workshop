# ADT Explorer

ADT Explorer is a GUI tool written with Node.js to visualize Digital Twin graph, nodes, and models.

Please ensure Node.js is installed on your host PC environment.

```bash
C:\>node --version
v14.15.1

C:\>npm --version
6.14.8
```

## Preparation

1. Install Node.js ver 12+ LTS from <https://nodejs.org/>  

## Set up ADT Explorer

Download and initialize ADT Explorer with :

```cmd
git clone https://github.com/Azure-Samples/digital-twins-explorer.git
cd digital-twins-explorer\client\src
npm install 
```

## Setup environment for ADT Explorer to connect to ADT instance

ADT Explorer uses the current logged in user credential.  Please log in to Azure prior to running ADT Explorer.

Log in to Azure from the host PC with :

```bash
az login
```  

> [!NOTE]  
> If you have multiple subscriptions, please select subscription in which your ADT instance is deployed. 
>
> ```bash  
>  az account list --output table
>  az account set --subscription "subscription ID or name"
> ```  

## Run ADT Explorer

Run ADT Explorer with :

```cmd
npm run start
```

Once you are logged in, provide ADT Host Name from the deployment output to ADT Explorer

![ADTHostName](media/ADTHostName.png)

[Return to Azure Digital Twin instruction](AzureDigitalTwins.md)

