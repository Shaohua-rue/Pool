#include "ConnPool.h"
using namespace std;


int main()
{
	// Connection conn;
	// conn.connect("127.0.0.1", 3306, "shaohua", "010407", "dbtest");

	// Connection conn;
	// char sql[1024] = { 0 };
	// sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
	// 	"zhang san", 20, "male");
	// conn.connect("127.0.0.1", 3306, "shaohua", "010407", "dbtest");
	// conn.update(sql);

	clock_t begin = clock();
	
	thread t1([]() {
		ConnPool *cp = ConnPool::getConnPool();
		for (int i = 0; i < 2500; )
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
				if(!sp->update(sql)){
					//std::cout<<"update failed"<<std::endl;
					continue;
				}else{
					++i;
				}
		}
	});
	thread t2([]() {
		ConnPool *cp = ConnPool::getConnPool();
		for (int i = 0; i < 2500; )
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
				if(!sp->update(sql)){
					//std::cout<<"update failed"<<std::endl;
					continue;
				}else{
					++i;
				}
		}
	});
	thread t3([]() {
		ConnPool *cp = ConnPool::getConnPool();
		for (int i = 0; i < 2500; )
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
				if(!sp->update(sql)){
					//std::cout<<"update failed"<<std::endl;
					continue;
				}else{
					++i;
				}
		}
	});
	thread t4([]() {
		ConnPool *cp = ConnPool::getConnPool();
		for (int i = 0; i < 2500; )
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
				if(!sp->update(sql)){
					//std::cout<<"update failed"<<std::endl;
					continue;
				}else{
					++i;
				}
		}
	});

	t1.join();
	t2.join();
	t3.join();
	 t4.join();

	clock_t end = clock();
	cout << (end - begin) << "ms" << endl;


#if 0
	for (int i = 0; i < 10000; ++i)
	{
		Connection conn;
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
			"zhang san", 20, "male");
		conn.connect("127.0.0.1", 3306, "shaohua", "010407", "dbtest");
		conn.update(sql);

		/*shared_ptr<Connection> sp = cp->getConnection();
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
			"zhang san", 20, "male");
		sp->update(sql);*/
	}
#endif

	return 0;
}
