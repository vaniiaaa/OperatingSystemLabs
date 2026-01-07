#include "data.h"
#include "sqlite3.h"
#include <iostream>

sqlite3 *db = nullptr;

void init_db()
{
    int rc = sqlite3_open("tasks.db", &db);
    if (rc)
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    const char *sql = "CREATE TABLE IF NOT EXISTS tasks ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "title TEXT NOT NULL, "
                      "description TEXT, "
                      "status TEXT DEFAULT 'todo');";

    char *errMsg = 0;
    rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

bool get_task_from_db(int id, Task &task)
{
    const char *sql =
        "SELECT id, title, description, status FROM tasks WHERE id = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
        return false;

    sqlite3_bind_int(stmt, 1, id);

    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        task.id = sqlite3_column_int(stmt, 0);
        task.title =
            reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));

        const char *desc =
            reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        task.description = desc ? desc : "";

        const char *st =
            reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
        task.status = st ? st : "todo";
        found = true;
    }

    sqlite3_finalize(stmt);
    return found;
}