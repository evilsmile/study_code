#include <iostream>
#include <mysql/mysql.h>
#include <string>

static MYSQL mysql;
static void init()
{
		mysql_init(&mysql);
		mysql_real_connect(&mysql, "localhost", "root", "naruto", "test",
						3306, NULL, 0);
}

static void insert()
{
		std::string sql("INSERT INTO t1 (id, name) VALUES(1, 'java1')");
		mysql_query(&mysql, sql.c_str());
		mysql_close(&mysql);
}

static void update()
{
		std::string sql("INSERT INTO t1(id, name) VALUES(2, 'java2')");
		mysql_query(&mysql, sql.c_str());
		sql = "update t1 set name='java22' where id=2;";
		mysql_query(&mysql, sql.c_str());
}

static void procedure()
{
		std::string sql("call p01()");
		mysql_query(&mysql, sql.c_str());
}

static void checkTotal()
{
		MYSQL_RES *result = NULL;
		std::string sql("SELECT id,name FROM t1");
		mysql_query(&mysql, sql.c_str());
		result = mysql_store_result(&mysql);
		int rowcount = mysql_num_rows(result);
		std::cout << rowcount << "\n";
}

static void checkData()
{
		MYSQL_RES *result = NULL;
		MYSQL_FIELD *field = NULL;
		std::string sql("SELECT id,name FROM t1");
		mysql_query(&mysql, sql.c_str());
		result = mysql_store_result(&mysql);
		int rowcount = mysql_num_rows(result);
		std::cout << "row:" << rowcount << "\n";	
		int fieldscount = mysql_num_fields(result);
		std::cout << "fields: " << fieldscount << "\n";	
		for(int i = 0; i < fieldscount; i++){
				field = mysql_fetch_field_direct(result, i);
				std::cout << field->name << "\t";
		}
		std::cout << "\n";

		MYSQL_ROW row = NULL;
		row = mysql_fetch_row(result);
		while(NULL != row){
				for(int i = 0; i < fieldscount; i++){
						std::cout << row[i] << "\t";
				}
				std::cout << "\n";
				row = mysql_fetch_row(result);
		}
}

static void close()
{
		mysql_close(&mysql);
}

int main(int argc, char *argv[])
{
		init();
//		insert();
//		update();
//		procedure();
//		checkTotal();
		checkData();
		
		close();

		return 0;
}
