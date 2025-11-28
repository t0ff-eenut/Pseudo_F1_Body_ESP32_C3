/*
******************************************************************************
* File Name          : custom_esp_nvs.c
* Description        : ESP32 NVS MODULE
******************************************************************************
* ESP32의 NVS(비휘발성 저장소)를 쉽게 사용하도록 돕는 래퍼(wrapper) 모듈.
*
* 주요 기능:
* - 여러 네임스페이스('status', 'config', 'data')를 생성하고 관리합니다.
* - 'envs' 구조체를 활용하여, 단일 함수('custom_read_nvs', 'custom_write_nvs')로 다양한 데이터 타입(정수, 문자열 등)을 처리합니다.
******************************************************************************
******************************************************************************
* first update : 2025/09/03
******************************************************************************
* final update : 2025/11/04
******************************************************************************
*/

#include "custom_esp_nvs.h"

#define NVS_DEBUG         DEBUG
#if NVS_DEBUG
    // 로그 출력 시 사용할 태그를 정의합니다.
    static const char *custom_esp_nvs_TAG = "[@]custom_esp_nvs.c";
#endif

nvs_handle_t* A_nvs_handle;

// | 항목   | 제한                        |
// | ----   | ------------------------- |
// | 길이   | 최대 15자 (NULL 종료 포함하면 16자) |
// | 문자   | 알파벳, 숫자, `_` 등 ASCII 문자   |
// | NULL   | key는 NULL로 시작할 수 없음       |
static const char* A_c_nvs_namespaces[NVS_NAMESPACE_MAX] = {
    [NVS_HANDLE_STATUS] = "status",
    [NVS_HANDLE_CONFIG] = "config",
    [NVS_HANDLE_DATA]   = "data"
};

// enhe → str 변환
char* nvs_handle_to_string(const enhe input_enhe_value){
    if (input_enhe_value >= 0 && input_enhe_value < NVS_NAMESPACE_MAX)
        return A_c_nvs_namespaces[input_enhe_value];
    else
        return "unknown";
}

// str → enhe 변환
enhe nvs_string_to_handle(const char* input_str_value){
    for (int i = 0; i < NVS_NAMESPACE_MAX; i++) {
        // | 반환값   | 의미                       |
        // | ----- | ------------------------ |
        // | `< 0` | `s1`이 `s2`보다 사전순으로 앞에 있음 |
        // | `0`   | 두 문자열이 완전히 같음            |
        // | `> 0` | `s1`이 `s2`보다 사전순으로 뒤에 있음 |
        if (strcmp(input_str_value, A_c_nvs_namespaces[i]) == 0)
            return (enhe)i;
    }
    return NVS_NAMESPACE_MAX; // invalid or not found
}

bool custom_nvs_init(void){
    #define CUSTOM_NVS_INIT_DEBUG         NVS_DEBUG

    static bool b_success = true;

    #if CUSTOM_NVS_INIT_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_nvs_init() - NVS Flash 초기 설정\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG);
    #endif
    b_success &= !nvs_flash_init();
    if(!b_success){
        #if INITIAL_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_nvs_init() - NVS Flash 초기 설정 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        return b_success;
    }

    #if CUSTOM_NVS_INIT_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_nvs_init() - NVS Flash 초기화 여부 확인\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG);
    #endif
    //// 초기화 동작 수행 ////
    if(
        // BOOT 버튼 눌림
        gpio_get_level(NVS_RESET_BUTTON_GPIO_NUM) == 0
        || // 또는
        // NVS 가득 참 or 새로운 Version
        (esp_sleep_wakeup_cause == ESP_SLEEP_WAKEUP_UNDEFINED && (global_esp_err == ESP_ERR_NVS_NO_FREE_PAGES || global_esp_err == ESP_ERR_NVS_NEW_VERSION_FOUND))
    ){
        #if CUSTOM_NVS_INIT_DEBUG
        printf("[%s] ##### [특수-Case]\t %s custom_nvs_init() - NVS 초기화\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        b_success &= !nvs_flash_erase();
        if(!b_success){
            #if INITIAL_DEBUG
            printf("[%s] "COLOR_RED"[오류-ERROR]\t %s nvs_flash_erase() - NVS 초기화 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG);
            #endif
            return b_success;
        }

        #if CUSTOM_NVS_INIT_DEBUG
        printf("[%s] ##### [특수-Case]\t %s custom_nvs_init() - NVS Flash 재설정\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        b_success &= !nvs_flash_init();
        if(!b_success){
            #if INITIAL_DEBUG
            printf("[%s] "COLOR_RED"[오류-ERROR]\t %s nvs_flash_init() - NVS Flash 재설정 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG);
            #endif
            return b_success;
        }
    }
    //// 초기화 동작 수행 ////

    #if CUSTOM_NVS_INIT_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_nvs_init() - NVS 핸들 동적 공간 생성\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG);
    #endif
    A_nvs_handle = (nvs_handle_t*) malloc(sizeof(nvs_handle_t) * NVS_NAMESPACE_MAX);
    if(A_nvs_handle == NULL){
        #if CUSTOM_NVS_INIT_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_nvs_init() - NVS 핸들 동적 공간 생성 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG);
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_nvs_init() - return false\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        b_success = false;
        return b_success;
    }
    else{
        #if CUSTOM_NVS_INIT_DEBUG
        printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_nvs_init() - NVS 핸들 생성\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        for(enhe nvs_handle_name = NVS_HANDLE_STATUS; nvs_handle_name < NVS_NAMESPACE_MAX; nvs_handle_name++){
            #if CUSTOM_NVS_INIT_DEBUG
            printf("[%s] "COLOR_BLACK"[정보-INFO]\t %s custom_nvs_init() - NVS Handle Init : %s\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, nvs_handle_to_string(nvs_handle_name));
            #endif
            // int i_return_value = nvs_open(nvs_handle_to_string(nvs_handle_name), NVS_READWRITE, &A_nvs_handle[nvs_handle_name]);
            // printf("custom_nvs_init() - nvs_open() - i_return_value : %d\n" COLOR_RESET, i_return_value);

            b_success &= !nvs_open(nvs_handle_to_string(nvs_handle_name), NVS_READWRITE, &A_nvs_handle[nvs_handle_name]);
            if(!b_success){
                #if INITIAL_DEBUG
                printf("[%s] "COLOR_RED"[오류-ERROR]\t %s nvs_open() - %s , NVS_READWRITE 실패\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, nvs_handle_to_string(nvs_handle_name));
                #endif
                return b_success;
            }
        }
        b_success = true;
    }
    return b_success;
}

void custom_print_nvs_err(const esp_err_t input_err){
    #define CUSTOM_PRINT_NVS_ERROR_DEBUG         NVS_DEBUG

    switch (input_err) {
    case ESP_OK:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_print_nvs_err() - [ESP_OK] 작업 성공\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_NOT_INITIALIZED:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_NOT_INITIALIZED] NVS가 초기화되지 않았습니다 (nvs_flash_init() 필요)\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_PART_NOT_FOUND:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_PART_NOT_FOUND] NVS 파티션을 찾을 수 없습니다 (partition_table.csv 확인 필요)\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_NOT_ENOUGH_SPACE:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_NOT_ENOUGH_SPACE] NVS 파티션에 공간이 부족합니다\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_INVALID_STATE:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_INVALID_STATE] NVS 상태가 유효하지 않습니다 (중복 init 또는 내부 오류)\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_NOT_FOUND:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] [경고-WARRING]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_NOT_FOUND] 요청한 key가 NVS에 존재하지 않습니다\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_TYPE_MISMATCH:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] [경고-WARRING]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_TYPE_MISMATCH] key는 찾았지만 데이터 타입이 다릅니다\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_READ_ONLY:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_READ_ONLY] NVS 파티션이 읽기 전용으로 열려 있습니다\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_INVALID_NAME:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_INVALID_NAME] NVS 키 이름이 잘못되었습니다 (길이/문자 제한 위반)\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_INVALID_HANDLE:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_INVALID_HANDLE] NVS 핸들이 유효하지 않습니다 (open 실패 등)\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_REMOVE_FAILED:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_REMOVE_FAILED] key 삭제 실패\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_KEY_TOO_LONG:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_KEY_TOO_LONG] NVS 키 이름이 너무 깁니다\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_PAGE_FULL:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_PAGE_FULL] NVS 페이지가 가득 찼습니다\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_INVALID_LENGTH:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_INVALID_LENGTH] 데이터 길이가 유효하지 않습니다\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_NO_FREE_PAGES:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_NO_FREE_PAGES] 사용 가능한 페이지가 없습니다 (포맷 필요)\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_VALUE_TOO_LONG:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_VALUE_TOO_LONG] 값이 너무 커서 기록 불가합니다\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_NEW_VERSION_FOUND:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_NEW_VERSION_FOUND] NVS 버전이 호환되지 않습니다 (포맷 필요)\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_XTS_ENCR_FAILED:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_XTS_ENCR_FAILED] NVS 암호화 실패\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_XTS_DECR_FAILED:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_XTS_DECR_FAILED] NVS 복호화 실패\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_XTS_CFG_FAILED:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_XTS_CFG_FAILED] NVS 암호화 설정 실패\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_XTS_CFG_NOT_FOUND:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_XTS_CFG_NOT_FOUND] NVS 암호화 설정을 찾을 수 없음\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_ENCR_NOT_SUPPORTED:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_ENCR_NOT_SUPPORTED] NVS 암호화 기능 미지원\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_KEYS_NOT_INITIALIZED:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_KEYS_NOT_INITIALIZED] 암호화 키가 초기화되지 않음\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_CORRUPT_KEY_PART:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_CORRUPT_KEY_PART] 키 파티션 손상됨\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_WRONG_ENCRYPTION:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_WRONG_ENCRYPTION] 잘못된 암호화 방식 사용\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    case ESP_ERR_NVS_CONTENT_DIFFERS:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] [경고-WARRING]\t %s custom_print_nvs_err() - [ESP_ERR_NVS_CONTENT_DIFFERS] 기록된 내용이 기대값과 다릅니다\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        break;
    default:
        #if CUSTOM_PRINT_NVS_ERROR_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_print_nvs_err() - 알 수 없는 오류: 0x%X\n" COLOR_RESET,
               getRuntimeString(), custom_esp_nvs_TAG, input_err);
        #endif
    }
}

void custom_init_envs_value(envs *input_envs_value){
    input_envs_value->esp_err                      = ESP_FAIL;
    input_envs_value->envte_type                   = UNKNOWN;
    input_envs_value->union_value.i8               = 0;
    input_envs_value->union_value.u8               = 0;
    input_envs_value->union_value.i16              = 0;
    input_envs_value->union_value.u16              = 0;
    input_envs_value->union_value.i32              = 0;
    input_envs_value->union_value.u32              = 0;
    input_envs_value->union_value.i64              = 0;
    input_envs_value->union_value.u64              = 0;
    input_envs_value->union_value.b                = false;
    input_envs_value->union_value.str.str_data     = NULL;
    input_envs_value->union_value.str.str_len      = 0;
    input_envs_value->union_value.blob.blob_data   = NULL;
    input_envs_value->union_value.blob.blob_len    = 0;
}

bool custom_read_nvs(const enhe input_enhe_value, envs* input_envs_value){
    #define CUSTOM_READ_NVS_DEBUG         NVS_DEBUG
    bool b_success = true;

    if (input_envs_value->c_nvs_key == NULL) {
        #if CUSTOM_READ_NVS_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_nvs_init() - input_envs_value->c_nvs_key 존재하지 않음\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        input_envs_value->esp_err = ESP_ERR_INVALID_ARG;
        b_success = false;
    }else{
        // custom_init_envs_value(&envs_return_value);
        #if CUSTOM_READ_NVS_DEBUG
        printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_read_nvs() - %s NVS에 %s 변수 존재 확인\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, input_enhe_value == NVS_HANDLE_STATUS  ? "NVS_HANDLE_STATUS" 
                                                                                                                            : input_enhe_value == NVS_HANDLE_CONFIG   ? "NVS_HANDLE_CONFIG " 
                                                                                                                            : input_enhe_value == NVS_HANDLE_DATA   ? "NVS_HANDLE_DATA "
                                                                                                                            : input_enhe_value == NVS_NAMESPACE_MAX    ? "NVS_NAMESPACE_MAX(ERROR)" : "ERROR",
                                                                                                                            input_envs_value->c_nvs_key);
        #endif
        #if CUSTOM_READ_NVS_DEBUG
        printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_read_nvs() - TYPE : %s\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, input_envs_value->envte_type == INT8 ? "INT8" 
                                                                                                            : input_envs_value->envte_type == UINT8 ? "UINT8"
                                                                                                            : input_envs_value->envte_type == INT16 ? "INT16"
                                                                                                            : input_envs_value->envte_type == UINT16 ? "UINT16"
                                                                                                            : input_envs_value->envte_type == INT32 ? "INT32"
                                                                                                            : input_envs_value->envte_type == UINT32 ? "UINT32"
                                                                                                            : input_envs_value->envte_type == INT64 ? "INT64"
                                                                                                            : input_envs_value->envte_type == UINT64 ? "UINT64"
                                                                                                            : input_envs_value->envte_type == BOOL ? "BOOL"
                                                                                                            : input_envs_value->envte_type == STRING ? "STRING"
                                                                                                            : input_envs_value->envte_type == ARRAY ? "ARRAY"
                                                                                                            : input_envs_value->envte_type == UNKNOWN ? "UNKNOWN" : "ERROR");
        #endif
        switch (input_envs_value->envte_type)
        {
            case INT8:
                input_envs_value->esp_err = nvs_get_i8(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key, &input_envs_value->union_value.i8);
                #if CUSTOM_READ_NVS_DEBUG
                printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_read_nvs() - VALUE : 0x%X\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, input_envs_value->union_value.i8);
                #endif
                break;
            case UINT8: case BOOL:
                input_envs_value->esp_err = nvs_get_u8(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key, &input_envs_value->union_value.u8);
                #if CUSTOM_READ_NVS_DEBUG
                printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_read_nvs() - VALUE : 0x%X\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, input_envs_value->union_value.u8);
                #endif
                break;
            case INT16:
                input_envs_value->esp_err = nvs_get_i16(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key, &input_envs_value->union_value.i16);
                #if CUSTOM_READ_NVS_DEBUG
                printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_read_nvs() - VALUE : 0x%X\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, input_envs_value->union_value.i16);
                #endif
                break;
            case UINT16:
                input_envs_value->esp_err = nvs_get_u16(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key, &input_envs_value->union_value.u16);
                #if CUSTOM_READ_NVS_DEBUG
                printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_read_nvs() - VALUE : 0x%X\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, input_envs_value->union_value.u16);
                #endif
                break;
            case INT32:
                input_envs_value->esp_err = nvs_get_i32(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key, &input_envs_value->union_value.i32);
                #if CUSTOM_READ_NVS_DEBUG
                printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_read_nvs() - VALUE : 0x%lX\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, input_envs_value->union_value.i32);
                #endif
                break;
            case UINT32:
                input_envs_value->esp_err = nvs_get_u32(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key, &input_envs_value->union_value.u32);
                #if CUSTOM_READ_NVS_DEBUG
                printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_read_nvs() - VALUE : 0x%lX\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, input_envs_value->union_value.u32);
                #endif
                break;
            case INT64:
                input_envs_value->esp_err = nvs_get_i64(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key, &input_envs_value->union_value.i64);
                #if CUSTOM_READ_NVS_DEBUG
                printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_read_nvs() - VALUE : 0x%llX\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, input_envs_value->union_value.i64);
                #endif
                break;
            case UINT64:
                input_envs_value->esp_err = nvs_get_u64(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key, &input_envs_value->union_value.u64);
                #if CUSTOM_READ_NVS_DEBUG
                printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_read_nvs() - VALUE : 0x%llX\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, input_envs_value->union_value.u64);
                #endif
                break;
            case STRING:
                input_envs_value->union_value.str.str_data = NULL;
                input_envs_value->union_value.str.str_len = 0;
                // 길이 먼저 확인
                input_envs_value->esp_err = nvs_get_str(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key, NULL, &input_envs_value->union_value.str.str_len);
                if(input_envs_value->esp_err == ESP_OK){
                    input_envs_value->union_value.str.str_data = malloc(input_envs_value->union_value.str.str_len);
                    input_envs_value->esp_err = nvs_get_str(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key, input_envs_value->union_value.str.str_data, &input_envs_value->union_value.str.str_len);
                    if (input_envs_value->esp_err != ESP_OK) {
                        free(input_envs_value->union_value.str.str_data);
                        input_envs_value->union_value.str.str_data = NULL;
                        b_success = false;
                    }
                }
                break;
            case ARRAY:
                input_envs_value->union_value.blob.blob_data = NULL;
                input_envs_value->union_value.blob.blob_len = 0;
                // 길이 먼저 확인
                input_envs_value->esp_err = nvs_get_blob(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key, NULL, &input_envs_value->union_value.blob.blob_len);
                if (input_envs_value->esp_err == ESP_OK) {
                    input_envs_value->union_value.blob.blob_data = malloc(input_envs_value->union_value.blob.blob_len);
                    input_envs_value->esp_err = nvs_get_blob(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key, input_envs_value->union_value.blob.blob_data, &input_envs_value->union_value.blob.blob_len);
                    if (input_envs_value->esp_err != ESP_OK) {
                        free(input_envs_value->union_value.blob.blob_data);
                        input_envs_value->union_value.blob.blob_data = NULL;
                        b_success = false;
                    }
                }
                break;
            default:
                input_envs_value->esp_err = ESP_ERR_NOT_SUPPORTED;
                #if CUSTOM_READ_NVS_DEBUG
                printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_read_nvs() - Type ERROR\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG);
                #endif
                b_success = false;
                break;
        }
    }
    custom_print_nvs_err(input_envs_value->esp_err);
    if(input_envs_value->esp_err != ESP_OK){
        #if CUSTOM_READ_NVS_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_read_nvs() - input_envs_value->esp_err != ESP_OK\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        b_success = false;
    }

    return b_success;
}

bool custom_write_nvs(enhe input_enhe_value, envs* input_envs_value){
    #define CUSTOM_WRITE_NVS_DEBUG         NVS_DEBUG

    bool b_success = true;
    // esp_err_t err = ESP_FAIL;

    if (input_envs_value->c_nvs_key == NULL) {
        #if CUSTOM_WRITE_NVS_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_nvs_init() - input_envs_value->c_nvs_key 존재하지 않음\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        return ESP_ERR_INVALID_ARG;
    }else{
        #if CUSTOM_WRITE_NVS_DEBUG
        printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_write_nvs() - input_envs_value->c_nvs_key : %s\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, input_envs_value->c_nvs_key);
        printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_write_nvs() - input_envs_value->envte_type : %s\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, input_envs_value->envte_type == INT8 ? "INT8" 
                                                                                                                                    : input_envs_value->envte_type == UINT8 ? "UINT8"
                                                                                                                                    : input_envs_value->envte_type == INT16 ? "INT16"
                                                                                                                                    : input_envs_value->envte_type == UINT16 ? "UINT16"
                                                                                                                                    : input_envs_value->envte_type == INT32 ? "INT32"
                                                                                                                                    : input_envs_value->envte_type == UINT32 ? "UINT32"
                                                                                                                                    : input_envs_value->envte_type == INT64 ? "INT64"
                                                                                                                                    : input_envs_value->envte_type == UINT64 ? "UINT64"
                                                                                                                                    : input_envs_value->envte_type == BOOL ? "BOOL"
                                                                                                                                    : input_envs_value->envte_type == STRING ? "STRING"
                                                                                                                                    : input_envs_value->envte_type == ARRAY ? "ARRAY"
                                                                                                                                    : input_envs_value->envte_type == UNKNOWN ? "UNKNOWN" : "ERROR");
        #endif

        switch (input_envs_value->envte_type)
        {
            case INT8:
                input_envs_value->esp_err = nvs_set_i8(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key, input_envs_value->union_value.i8);
                #if CUSTOM_WRITE_NVS_DEBUG
                printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_write_nvs() - Writing VALUE : 0x%X\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, input_envs_value->union_value.i8);
                #endif
                break;
            case UINT8: case BOOL:
                input_envs_value->esp_err = nvs_set_u8(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key, input_envs_value->union_value.u8);
                #if CUSTOM_WRITE_NVS_DEBUG
                printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_write_nvs() - Writing VALUE : 0x%X\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, input_envs_value->union_value.u8);
                #endif
                break;
            case INT16:
                input_envs_value->esp_err = nvs_set_i16(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key, input_envs_value->union_value.i16);
                #if CUSTOM_WRITE_NVS_DEBUG
                printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_write_nvs() - Writing VALUE : 0x%X\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, input_envs_value->union_value.i16);
                #endif
                break;
            case UINT16:
                input_envs_value->esp_err = nvs_set_u16(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key, input_envs_value->union_value.u16);
                #if CUSTOM_WRITE_NVS_DEBUG
                printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_write_nvs() - Writing VALUE : 0x%X\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, input_envs_value->union_value.u16);
                #endif
                break;
            case INT32:
                input_envs_value->esp_err = nvs_set_i32(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key, input_envs_value->union_value.i32);
                #if CUSTOM_WRITE_NVS_DEBUG
                printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_write_nvs() - Writing VALUE : 0x%lX\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, input_envs_value->union_value.i32);
                #endif
                break;
            case UINT32:
                input_envs_value->esp_err = nvs_set_u32(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key, input_envs_value->union_value.u32);
                #if CUSTOM_WRITE_NVS_DEBUG
                printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_write_nvs() - Writing VALUE : 0x%lX\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, input_envs_value->union_value.u32);
                #endif
                break;
            case INT64:
                input_envs_value->esp_err = nvs_set_i64(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key, input_envs_value->union_value.i64);
                #if CUSTOM_WRITE_NVS_DEBUG
                printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_write_nvs() - Writing VALUE : 0x%llX\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, input_envs_value->union_value.i64);
                #endif
                break;
            case UINT64:
                input_envs_value->esp_err = nvs_set_u64(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key, input_envs_value->union_value.u64);
                #if CUSTOM_WRITE_NVS_DEBUG
                printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_write_nvs() - Writing VALUE : 0x%llX\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, input_envs_value->union_value.u64);
                #endif
                break;
            case STRING:
                if (input_envs_value->union_value.str.str_data != NULL) {
                    input_envs_value->esp_err = nvs_set_str(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key, input_envs_value->union_value.str.str_data);
                    if (input_envs_value->esp_err == ESP_OK) {
                        // input_envs_value->esp_err = nvs_commit(A_nvs_handle[enhe_handle_num]);    // 실제 저장
                    }
                } else {
                    input_envs_value->esp_err = ESP_ERR_INVALID_ARG;            // NULL 포인터
                }
                break;
            case ARRAY:
                if (input_envs_value->union_value.blob.blob_data != NULL && input_envs_value->union_value.blob.blob_len > 0) {
                    input_envs_value->esp_err = nvs_set_blob(A_nvs_handle[input_enhe_value], input_envs_value->c_nvs_key,
                                                input_envs_value->union_value.blob.blob_data,
                                                input_envs_value->union_value.blob.blob_len);
                    if (input_envs_value->esp_err == ESP_OK) {
                        // input_envs_value->esp_err = nvs_commit(A_nvs_handle[input_enhe_value]);    // 실제 저장
                    }
                } else {
                    input_envs_value->esp_err = ESP_ERR_INVALID_ARG;            // 잘못된 데이터
                }
                break;

            default:
                input_envs_value->esp_err = ESP_ERR_NOT_SUPPORTED;
                break;
        }   
    }
    custom_print_nvs_err(input_envs_value->esp_err);
    if(input_envs_value->esp_err != ESP_OK){
        #if CUSTOM_READ_NVS_DEBUG
        printf("[%s] "COLOR_RED"[오류-ERROR]\t %s custom_write_nvs() - input_envs_value->esp_err != ESP_OK\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG);
        #endif
        b_success = false;
    }
    return b_success;
}

void custom_commit_nvs(const enhe input_enhe_value){
    #define CUSTOM_COMMIT_NVS_DEBUG         NVS_DEBUG
    
    #if CUSTOM_COMMIT_NVS_DEBUG
    printf("[%s] "COLOR_WHITE"[진행-OK]\t %s custom_commit_nvs() - %s Commit\n" COLOR_RESET, getRuntimeString(), custom_esp_nvs_TAG, nvs_handle_to_string(input_enhe_value));
    #endif

    int i_return_value = nvs_commit(A_nvs_handle[input_enhe_value]);
    printf("custom_commit_nvs() - nvs_commit() - i_return_value : %d\n" COLOR_RESET, i_return_value);

}

bool custom_nvs_deinit(void){

    // nvs_close return --> void
    for(enhe enhe_nvs_handle_name = NVS_HANDLE_STATUS; enhe_nvs_handle_name < NVS_NAMESPACE_MAX; enhe_nvs_handle_name++){
        nvs_close(A_nvs_handle[enhe_nvs_handle_name]);
    }

    free(A_nvs_handle);
    A_nvs_handle = NULL;

    return true;
}