// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Sample IoT Plug and Play device app for X.509 certificate attestation
// Custom HSM module based on <azure-iot-sdk-c/provisioning_client/sample/custom_hsm_example
//
// Daisuke Nakahara 
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hsm_client_data.h"
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509v3.h>
#include <unistd.h>

static char CERTIFICATE_FILE[1024];
static char CERTIFICATE_KEY[1024];

typedef struct CUSTOM_HSM_DATA_TAG
{
    char* certificate;
    char* common_name;
    char* deviceKey;
    char* deviceId;
    char* symmetricKey;
} CUSTOM_HSM_DATA;

/*
** For X.509
*/
int hsm_client_x509_init()
{
    // printf("%s()\r\n", __func__);
    return 0;
}

void hsm_client_x509_deinit()
{
    // printf("%s()\r\n", __func__);
    return;
}

/*
*   X.509 Certificate
*/
HSM_CLIENT_HANDLE custom_hsm_create_x509()
{
    HSM_CLIENT_HANDLE result;
    CUSTOM_HSM_DATA* hsm_info = NULL;
    FILE * fp_certificate;
    FILE * fp_key;
    size_t fileSize;
    size_t readSize;

    X509 *x509 = NULL;
    X509_NAME *issuer = NULL;
    X509_NAME *subject = NULL;

    int index;
    X509_NAME_ENTRY *name_entry = NULL;
    ASN1_STRING *entryData = NULL;
    int length;
    char buffer[1024];
    char hostNameBuffer[256];

    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    printf("=======================================================\r\nReading X.509 Certificate\r\n");

    if (gethostname(hostNameBuffer, sizeof(hostNameBuffer)) == -1)
    {
        printf("Error: failed to get hostname\r\n");
    }
    else if (snprintf(CERTIFICATE_FILE, sizeof(CERTIFICATE_FILE), "./certs/%s.cer.pem", hostNameBuffer) < 0)
    {
        printf("Error: failed to snprintf CERTIFICATE_FILE\r\n");
    }
    else if (snprintf(CERTIFICATE_KEY, sizeof(CERTIFICATE_KEY), "./certs/%s.key.pem", hostNameBuffer) < 0)
    {
        printf("Error: failed to snprintf CERTIFICATE_KEY\r\n");
    }
    else if ((hsm_info = malloc(sizeof(CUSTOM_HSM_DATA))) == NULL)
    {
        printf("Error: failed allocating hsm info\r\n");
    }
    else if ((memset(hsm_info, 0, sizeof(CUSTOM_HSM_DATA))) == NULL)
    {
        printf("Error: failed to zero set hsm_info\r\n");
    }
    else if ((fp_certificate = fopen(CERTIFICATE_FILE, "r")) == NULL)
    {
        printf("Error: %s not found\r\n", CERTIFICATE_FILE);
    }
    else if (fseek(fp_certificate, 0L, SEEK_END) != 0)
    {
        printf("Error: fseek SEEK_END failed\r\n");
    }
    else if ((fileSize = ftell(fp_certificate)) == -1)
    {
        printf("Error: ftell failed\r\n");
    }
    else if (fseek(fp_certificate, 0L, SEEK_SET) != 0)
    {
        printf("Error: fseek SEEK_SET failed\r\n");
    }
    else if ((hsm_info->certificate = malloc(fileSize + 1)) == NULL)
    {
        printf("Error: failed to allocate buffer for certificate (%ld bytes)", fileSize + 1);
    }
    else if ((readSize = fread(hsm_info->certificate, 1, fileSize, fp_certificate)) != fileSize)
    {
        printf("Error: certificate read size not match.  read : %ld byes, expected : %ld bytes", readSize, fileSize);
    }
    else if (fseek(fp_certificate, 0L, SEEK_SET) != 0)
    {
        printf("Error: fseek SEEK_SET failed\r\n");
    }
    else if ((x509 = PEM_read_X509(fp_certificate, NULL, NULL, NULL)) == NULL)
    {
        printf("Error: PEM_read_X509() failed\r\n");
    }
    else if ((issuer = X509_get_issuer_name(x509)) == NULL)
    {
        printf("Error: X509_get_subject_name() failed\r\n");
    }
    else if ((subject = X509_get_subject_name(x509)) == NULL)
    {
        printf("Error: X509_get_subject_name() failed\r\n");
    }
    else if ((index = X509_NAME_get_index_by_NID( subject, NID_commonName, -1 )) < 0)
    {
        printf("Error: X509_NAME_get_index_by_NID() NID_commonName failed\r\n");
    }
    else if ((name_entry = X509_NAME_get_entry( subject, index )) == NULL)
    {
        printf("Error: X509_NAME_get_entry() failed\r\n");
    }
    else if ((entryData = X509_NAME_ENTRY_get_data( name_entry )) == NULL)
    {
        printf("Error: X509_NAME_ENTRY_get_data() failed\r\n");
    }
    else if ((length = ASN1_STRING_to_UTF8( (unsigned char **)&hsm_info->common_name, entryData )) <= 0)
    {
        printf("Error: failed to convert subject name\r\n");
    }
    else if ((fp_key = fopen(CERTIFICATE_KEY, "r")) == NULL)
    {
        printf("Error: %s not found\r\n", CERTIFICATE_KEY);
    }
    else if (fseek(fp_key, 0L, SEEK_END) != 0)
    {
        printf("Error: fseek SEEK_END failed\r\n");
    }
    else if ((fileSize = ftell(fp_key)) == -1)
    {
        printf("Error: ftell failed\r\n");
    }
    else if (fseek(fp_key, 0L, SEEK_SET) != 0)
    {
        printf("Error: fseek SEEK_SET failed\r\n");
    }
    else if ((hsm_info->deviceKey = malloc(fileSize + 1)) == NULL)
    {
        printf("Error: failed to allocate buffer for device key (%ld bytes)", fileSize + 1);
    }
    else if ((readSize = fread(hsm_info->deviceKey, 1, fileSize, fp_key)) != fileSize)
    {
        printf("Error: deviceKey read size not match.  read : %ld byes, expected : %ld bytes", readSize, fileSize);
    }
    else
    {
        //__asm__("int $3");
        result = hsm_info;
    }

    if (x509 != NULL)
    {
        if (issuer != NULL)
        {
            X509_NAME_oneline(issuer, buffer, sizeof(buffer));
            printf("Issuer     : %s\r\n", buffer);
        }

        if (subject != NULL)
        {
            X509_NAME_oneline(subject, buffer, sizeof(buffer));
            printf("Subject    : %s\r\n", buffer);
        }

        printf("=======================================================\r\n");
        X509_free(x509);
    }

    if (fp_certificate != NULL)
    {
        fclose(fp_certificate);
    }

    if (fp_key != NULL)
    {
        fclose(fp_key);
    }

    return result;
}

void custom_hsm_destroy(HSM_CLIENT_HANDLE handle)
{
    // printf("Info: %s()\r\n", __func__);

    if (handle != NULL)
    {
        CUSTOM_HSM_DATA* hsm_info = (CUSTOM_HSM_DATA*)handle;

        if (hsm_info->certificate != NULL)
        {
            free(hsm_info->certificate);
        }

        if (hsm_info->common_name != NULL)
        {
            free(hsm_info->common_name);
        }

        if (hsm_info->deviceKey != NULL)
        {
            free(hsm_info->deviceKey);
        }

        if (hsm_info->symmetricKey != NULL)
        {
            free(hsm_info->symmetricKey);
        }

        if (hsm_info->deviceId != NULL)
        {
            free(hsm_info->deviceId);
        }

        free(hsm_info);
    }
}

char* custom_hsm_get_x509_certificate(HSM_CLIENT_HANDLE handle)
{
    CUSTOM_HSM_DATA* hsm_info = (CUSTOM_HSM_DATA*)handle;
    char* buffer = NULL;

    // printf("Info: %s()\r\n", __func__);

    if (hsm_info == NULL)
    {
        printf("Error: %s() : Invalid handle value specified\r\n", __func__);
    }
    else if (hsm_info->certificate == NULL)
    {
        printf("Error: %s() : certificate empty\r\n", __func__);
    }
    else if ((buffer = (char*)malloc(strlen(hsm_info->certificate) + 1)) == NULL)
    {
        printf("Error: %s() : Failure allocating buffer for certificate\r\n", __func__);
    }
    else
    {
        strcpy(buffer, hsm_info->certificate);
    }

    return buffer;
}

char* custom_hsm_get_x509_key(HSM_CLIENT_HANDLE handle)
{
    CUSTOM_HSM_DATA* hsm_info = (CUSTOM_HSM_DATA*)handle;
    char* buffer = NULL;

    // printf("Info: %s()\r\n", __func__);

    if (hsm_info == NULL)
    {
        printf("Error: %s() : Invalid handle value specified\r\n", __func__);
    }
    else if (hsm_info->deviceKey == NULL)
    {
        printf("Error: %s() : deviceKey empty\r\n", __func__);
    }
    else if ((buffer = (char*)malloc(strlen(hsm_info->deviceKey) + 1)) == NULL)
    {
        printf("Error: %s() : Failure allocating buffer for deviceKey\r\n", __func__);
    }
    else
    {
        strcpy(buffer, hsm_info->deviceKey);
    }

    return buffer;
}

char* custom_hsm_get_x609_common_name(HSM_CLIENT_HANDLE handle)
{
    CUSTOM_HSM_DATA* hsm_info = (CUSTOM_HSM_DATA*)handle;
    char* buffer = NULL;

    // printf("Info: %s()\r\n", __func__);

    if (hsm_info == NULL)
    {
        printf("Error: %s() : Invalid handle value specified\r\n", __func__);
    }
    else if (hsm_info->common_name == NULL)
    {
        printf("Error: %s() : common_name empty\r\n", __func__);
    }
    else if ((buffer = (char*)malloc(strlen(hsm_info->common_name) + 1)) == NULL)
    {
        printf("Error: %s() : Failure allocating buffer for common_name\r\n", __func__);
    }
    else
    {
        strcpy(buffer, hsm_info->common_name);
    }

    printf("Info: Common Name : %s\r\n", buffer);
    return buffer;
}

/*
*  Symmetric Key
*/
HSM_CLIENT_HANDLE custom_hsm_create_symkey()
{
    HSM_CLIENT_HANDLE result = NULL;
    CUSTOM_HSM_DATA* hsm_info;
    char* envPointer;

    // printf("Info: %s()\r\n", __func__);

    if ((hsm_info = malloc(sizeof(CUSTOM_HSM_DATA))) == NULL)
    {
        printf("Error: failed to allocate hsm info\r\n");
    }
    else if ((memset(hsm_info, 0, sizeof(CUSTOM_HSM_DATA))) == NULL)
    {
        printf("Error: failed to zero set hsm_info\r\n");
    }
    else if ((envPointer = getenv("DPS_DEVICE_ID")) == NULL)
    {
        printf("Error: Cannot read environment variable=DPS_DEVICE_ID\r\n");
    }
    else if ((hsm_info->deviceId = malloc(strlen(envPointer) + 1)) == NULL)
    {
        printf("Error: failed to allocate buffer for DPS_DEVICE_ID\r\n");
    }
    else if (strcpy(hsm_info->deviceId, envPointer) == NULL)
    {
        printf("Error: failed to copy variable=DPS_DEVICE_ID to buffer\r\n");
    }
    else if ((envPointer = getenv("DPS_SYMMETRIC_KEY")) == NULL)
    {
        printf("Error: Cannot read environment variable=DPS_SYMMETRIC_KEY\r\n");
    }
    else if ((hsm_info->symmetricKey = malloc(strlen(envPointer) + 1)) == NULL)
    {
        printf("Error: failed to allocate buffer for DPS_SYMMETRIC_KEY\r\n");
    }
    else if (strcpy(hsm_info->symmetricKey, envPointer) == NULL)
    {
        printf("Error: failed to copy variable=DPS_SYMMETRIC_KEY to buffer\r\n");
    }
    else
    {
        result = hsm_info;
    }

    return result;
}

char* custom_hsm_get_symm_key(HSM_CLIENT_HANDLE handle)
{
    CUSTOM_HSM_DATA* hsm_info = (CUSTOM_HSM_DATA*)handle;
    char* buffer = NULL;

    // printf("Info: %s()\r\n", __func__);

    if (hsm_info == NULL)
    {
        printf("Error: %s() : Invalid handle value specified\r\n", __func__);
    }
    else if (hsm_info->symmetricKey == NULL)
    {
        printf("Error: %s() : symmetricKey empty\r\n", __func__);
    }
    else if ((buffer = (char*)malloc(strlen(hsm_info->symmetricKey) + 1)) == NULL)
    {
        printf("Error: %s() : Failure allocating buffer for symmetricKey\r\n", __func__);
    }
    else
    {
        strcpy(buffer, hsm_info->symmetricKey);
    }

    return buffer;
}

char* custom_hsm_get_registration_name(HSM_CLIENT_HANDLE handle)
{
    CUSTOM_HSM_DATA* hsm_info = (CUSTOM_HSM_DATA*)handle;
    char* buffer = NULL;

    // printf("Info: %s()\r\n", __func__);

    if (hsm_info == NULL)
    {
        printf("Error: %s() : Invalid handle value specified\r\n", __func__);
    }
    else if (hsm_info->deviceId == NULL)
    {
        printf("Error: %s() : deviceId empty\r\n", __func__);
    }
    else if ((buffer = (char*)malloc(strlen(hsm_info->deviceId) + 1)) == NULL)
    {
        printf("Error: %s() : Failure allocating buffer for deviceId\r\n", __func__);
    }
    else
    {
        strcpy(buffer, hsm_info->deviceId);
    }

    return buffer;
}

int custom_hsm_client_set_symm_key_info(HSM_CLIENT_HANDLE handle, const char* reg_name, const char* symmetricKey)
{
    return 0;
}

// Defining the v-table for the x509 hsm calls
static const HSM_CLIENT_X509_INTERFACE x509_interface =
{
    custom_hsm_create_x509,
    custom_hsm_destroy,
    custom_hsm_get_x509_certificate,
    custom_hsm_get_x509_key,
    custom_hsm_get_x609_common_name
};

// Defining the v-table for the symmetric key hsm calls
static const HSM_CLIENT_KEY_INTERFACE symm_key_interface =
{
    custom_hsm_create_symkey,
    custom_hsm_destroy,
    custom_hsm_get_symm_key,
    custom_hsm_get_registration_name,
    custom_hsm_client_set_symm_key_info
};

const HSM_CLIENT_X509_INTERFACE* hsm_client_x509_interface()
{
    // printf("%s()\r\n", __func__);
    return &x509_interface;
}

const HSM_CLIENT_TPM_INTERFACE* hsm_client_tpm_interface()
{
    // printf("%s()\r\n", __func__);
    return NULL;
}

int hsm_client_tpm_init()
{
    // printf("%s()\r\n", __func__);
    return 0;
}

void hsm_client_tpm_deinit()
{
    return;
}

const HSM_CLIENT_KEY_INTERFACE* hsm_client_key_interface()
{
    // printf("%s()\r\n", __func__);
    return &symm_key_interface;
}
