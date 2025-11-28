#ifndef CUSTOM_ESP_NVS_H
#define CUSTOM_ESP_NVS_H

// #include "project_top.h"
// #include "../custom_esp_gpio/custom_esp_gpio.h"
#include "hw_level_top.h"

#include "nvs.h"            // nvs_handle_t, nvs_open 등
#include "nvs_flash.h"      // 비휘발성 저장소(NVS) 라이브러리를 사용하기 위해 포함합니다.

extern nvs_handle_t* A_nvs_handle;

/**
 * @enum        esp_nvs_handle_enum(enhe)
 * @brief       ESP NVS Handle enum
 * @attention   *주의사항
 * @warning     *경고
 * @note        *참고사항
 *
 * @param NVS_HANDLE_STATUS     0
 * @param NVS_HANDLE_CONFIG     1
 * @param NVS_HANDLE_DATA       2
 * @param NVS_NAMESPACE_MAX     3
 * 
 * @see         A_c_nvs_namespaces[]
 * @details     디테일 설명
 * @todo        todo
 * @bug         bug
 */
typedef enum esp_nvs_handle_enum{
    // esp_nvs_handle_t
    NVS_HANDLE_STATUS,
    NVS_HANDLE_CONFIG,
    NVS_HANDLE_DATA,
    NVS_NAMESPACE_MAX
}enhe;

/**
 * @struct      esp_nvs_value_type_enum(envte)
 * @brief       ESP NVS Vlaue Type enum
 * @attention   *주의사항
 * @warning     *경고
 * @note        *참고사항
 *
 * @param INT8      0
 * @param UINT8     1
 * @param INT16     2
 * @param UINT16    3
 * @param INT32     4
 * @param UINT32    5
 * @param INT64     6
 * @param BOOL      7
 * @param STRING    8
 * @param ARRAY     9
 * @param UNKNOWN   10
 * 
 * @details     디테일 설명
 * @todo        todo
 * @bug         bug
 */
typedef enum esp_nvs_value_type_enum{
    // esp_nvs_value_type_t
    INT8,
    UINT8,
    INT16,
    UINT16,
    INT32,
    UINT32,
    INT64,
    UINT64,
    BOOL,
    STRING,
    ARRAY,
    UNKNOWN
}envte;

/**
 * @struct      esp_nvs_value_structer(envs)
 * @brief       ESP NVS Value Structer
 * @attention   *주의사항
 * @warning     *경고
 * @note        *참고사항
 *
 * @param esp_err_t  esp_err;                // NVS API 호출 결과
 * @param char       *c_nvs_key;             // 저장된 변수 이름
 * @param envte      envte_type;             // 저장된 변수 타입
 * @union union      union_value;            // 입력/반환 변수 공간
    * @param int8_t     i8;
    * @param uint8_t    u8;
    * @param int16_t    i16;
    * @param uint16_t   u16;
    * @param int32_t    i32;
    * @param uint32_t   u32;
    * @param int64_t    i64;
    * @param uint64_t   u64;
    * @param bool       b;
    * @struct struct     str;                // 문자열
        * @param char     *str_data;
        * @param size_t   str_len;
    * @struct struct     blob;               // Binary 배열
        * @param char     *blob_data;
        * @param size_t   blob_len;
 * 
 * @details     디테일 설명
 * @todo        todo
 * @bug         bug
 */
typedef struct esp_nvs_value_structer{
    esp_err_t   esp_err;            
    char        *c_nvs_key;       // 변수 이름
    envte       envte_type;          // 실제 데이터 타입
    union {
        int8_t   i8;
        uint8_t  u8;
        int16_t  i16;
        uint16_t u16;
        int32_t  i32;
        uint32_t u32;
        int64_t  i64;
        uint64_t u64;
        bool     b;
        // char    *str_data;        // 문자열은 malloc/free 필요
        struct {
            char  *str_data;     // 배열/구조체
            size_t str_len;   // 바이트 길이
        } str;
        struct {
            void  *blob_data;     // 배열/구조체
            size_t blob_len;   // 바이트 길이
        } blob;
    } union_value;
} envs;

extern const char *nvs_namespaces[NVS_NAMESPACE_MAX];

/**
 * @brief       ######
 * @attention   *주의사항 [const : 읽기 전용]
 * @param[in]   enhe [const]
 * @return      char*
 * @warning     *경고
 * @note        *참고사항
 * 
 * @details     디테일 설명
 * @todo        todo
 * @bug         bug
 */
char* nvs_handle_to_string(const enhe input_enhe_value);

/**
 * @brief       ######
 * @attention   *주의사항 [const : 읽기 전용]
 * @param[in]   const char*
 * @return      enhe
 * @warning     *경고
 * @note        *참고사항
 * 
 * @details     디테일 설명
 * @todo        todo
 * @bug         bug
 */
enhe nvs_string_to_handle(const char* input_str_value);

/**
 * @brief       Custom NVS Initial Function
 * @attention   *주의사항 [const : 읽기 전용]
 * @param[in]   const char*
 * @return      enhe
 * @warning     *경고
 * @note        *참고사항
 * 
 * @details     디테일 설명
 * @todo        todo
 * @bug         bug
 */
bool custom_nvs_init(void);

/**
 * @brief       #####
 * @attention   *주의사항 [const : 읽기 전용]
 * @param[in]   const char*
 * @return      enhe
 * @warning     *경고
 * @note        *참고사항
 * 
 * @details     디테일 설명
 * @todo        todo
 * @bug         bug
 */
void custom_print_nvs_err(const esp_err_t input_err);

/**
 * @brief       Custom Initial envs Value Function
 * @attention   *주의사항 [const : 읽기 전용]
 * @param[in]   const esp_err_t
 * @return      void
 * @warning     *경고
 * @note        *참고사항
 * 
 * @details     디테일 설명
 * @todo        todo
 * @bug         bug
 */
void custom_init_envs_value(envs* input_envs_value);

/**
 * @brief       Custom Read NVS Function
 * @attention   *주의사항
 * @param[in]   enhe input_enhe_value [const : 읽기 전용]
 * @param[in]   envs *input_envs_value
 * @return      bool
 * @warning     *경고
 * @note        *참고사항
 * 
 * @details     디테일 설명
 * @todo        todo
 * @bug         bug
 */
bool custom_read_nvs(const enhe input_enhe_value, envs* input_envs_value);

/**
 * @brief       Custom Write NVS Function
 * @attention   *주의사항
 * @param[in]   enhe input_enhe_value [const : 읽기 전용]
 * @param[in]   envs *input_envs_value
 * @return      bool
 * @warning     *경고
 * @note        *참고사항
 * 
 * @details     디테일 설명
 * @todo        todo
 * @bug         bug
 */
bool custom_write_nvs(const enhe input_enhe_value, envs* input_envs_value);

/**
 * @brief       Custom Commit NVS Function
 * @attention   *주의사항
 * @param[in]   enhe input_enhe_value [const : 읽기 전용]
 * @param[in]   envs *input_envs_value
 * @return      bool
 * @warning     *경고
 * @note        *참고사항
 * 
 * @details     디테일 설명
 * @todo        todo
 * @bug         bug
 */
void custom_commit_nvs(const enhe input_enhe_value);

/**
 * @brief       custom_nvs_deinit Function
 * @attention   *주의사항
 * @param[in]   void
 * @return      bool
 * @warning     *경고
 * @note        *참고사항
 * 
 * @details     디테일 설명
 * @todo        todo
 * @bug         bug
 */
bool custom_nvs_deinit(void);
#endif