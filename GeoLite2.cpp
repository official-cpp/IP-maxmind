#include <iostream>
#include <maxminddb.h>

bool is_china_ip(const std::string& ip, const std::string& db_path) {
    MMDB_s mmdb;
    int status = MMDB_open(db_path.c_str(), MMDB_MODE_MMAP, &mmdb);
    if (status != MMDB_SUCCESS) {
        printf("警告：数据库不存在，将走全球路线: ");
        //std::cerr << "警告：数据库打开失败: " << MMDB_strerror(status) << std::endl;
        return false;
    }

    int gai_error, mmdb_error;
    MMDB_lookup_result_s result = MMDB_lookup_string(&mmdb, ip.c_str(), &gai_error, &mmdb_error);
    if (gai_error != 0) {
        std::cerr << "DNS解析错误: " << gai_strerror(gai_error) << std::endl;
        MMDB_close(&mmdb);
        return false;
    }
    if (mmdb_error != MMDB_SUCCESS) {
        std::cerr << "数据库查找错误: " << MMDB_strerror(mmdb_error) << std::endl;
        MMDB_close(&mmdb);
        return false;
    }

    if (result.found_entry) {
        MMDB_entry_data_s entry_data;
        int status = MMDB_get_value(&result.entry, &entry_data, "country", "iso_code", NULL);
        if (status == MMDB_SUCCESS && entry_data.has_data) {
            if (entry_data.type == MMDB_DATA_TYPE_UTF8_STRING) {
                std::string iso_code(entry_data.utf8_string, entry_data.data_size);
                MMDB_close(&mmdb);
                return iso_code == "CN";
            }
        }
    }

    MMDB_close(&mmdb);
    return false;
}

int main() {
    std::string db_path = "./GeoLite2-Country.mmdb";
    std::string test_ip = "1.174.45.180";
    std::cout << (is_china_ip(test_ip, db_path) ? "是中国IP" : "不是中国IP") << std::endl;

    return 0;
}

