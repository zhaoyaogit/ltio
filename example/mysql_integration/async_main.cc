#include "mysql_async_con.h"


MysqlOptions option = {
  .host = "10.9.158.21",
  .port = 3306,
  .user = "fancy_test",
  .passwd = "fancy_test",
  .dbname = "mysql",
  .query_timeout = 5000,
};

int main(int argc, char** argv) {
  mysql_library_init(0, NULL, NULL);
  
  if (argc > 1 && std::string(argv[1]) == "localhost") {
    option.host = "localhost";
    option.user = "root";
    option.passwd = "";
  }

  base::MessageLoop loop;

  loop.SetLoopName("main");
  loop.Start();

  MysqlConnection* g_conn = new MysqlConnection(NULL, &loop);

  loop.PostTask(NewClosure([&]()->void {
    g_conn->InitConnection(option);
  }));

  loop.PostDelayTask(NewClosure([&]() {
    LOG(INFO) << "start query mysql db";
    g_conn->StartAQuery("select Host, User, Password from user where User!=''");
  }), 3000);

  loop.WaitLoopEnd();

  delete g_conn;
  mysql_library_end();
  return 0;
}

