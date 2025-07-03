#include <iostream>
#include <string>
#include "mysql/mysql.h"


// 检查MySQL操作的错误
void check_error(MYSQL* conn, const std::string& msg) {
    if (mysql_errno(conn)) {
        std::cerr << msg << ": " << mysql_error(conn) << std::endl;
        mysql_close(conn);
        exit(1);
    }
}

// 检查表是否存在
bool check_table_exists(MYSQL* conn, const std::string& db_name, const std::string& table_name) {
    std::string query = "SELECT COUNT(*) FROM information_schema.tables "
                        "WHERE table_schema = '" + db_name + "' AND table_name = '" + table_name + "'";
    if (mysql_query(conn, query.c_str())) {
        check_error(conn, "查询表是否存在失败");
    }
    MYSQL_RES* result = mysql_store_result(conn);
    if (!result) {
        check_error(conn, "获取查询结果失败");
    }
    MYSQL_ROW row = mysql_fetch_row(result);
    int count = row? std::stoi(row[0]) : 0;
    mysql_free_result(result);
    return count > 0;
}

int main() {
    MYSQL conn;
    mysql_init(&conn);

    // 尝试连接到MySQL服务器
    if (!mysql_real_connect(&conn, "127.0.0.1", "shaohua", "010407", NULL, 0, NULL, 0)) {
        check_error(&conn, "连接MySQL服务器失败");
    }

    // 创建数据库
    std::string create_db_query = "CREATE DATABASE IF NOT EXISTS dbtest";
    if (mysql_query(&conn, create_db_query.c_str())) {
        check_error(&conn, "创建数据库失败");
    }

    // 选择刚刚创建的数据库
    if (mysql_select_db(&conn, "dbtest")) {
        check_error(&conn, "选择数据库失败");
    }

    std::string table_name = "user";
    if (!check_table_exists(&conn, "dbtest", table_name)) {
        // 创建user表
        std::string create_table_query = "CREATE TABLE user ("
                                         "id INT(11) NOT NULL AUTO_INCREMENT PRIMARY KEY,"
                                         "name VARCHAR(50),"
                                         "age INT(11),"
                                         "sex ENUM('male', 'female')"
                                         ")";
        if (mysql_query(&conn, create_table_query.c_str())) {
            check_error(&conn, "创建表失败");
        }
        std::cout << "表 " << table_name << " 创建成功" << std::endl;
    } else {
        std::cout << "表 " << table_name << " 已存在，无需创建" << std::endl;
    }

    mysql_close(&conn);
    return 0;
}