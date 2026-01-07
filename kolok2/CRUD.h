#include "crow_all.h"
#include "data.h"
#include "db.h"
#include <iostream>
#include <string>
#include <vector>

void Runner()
{
    init_db();
    crow::SimpleApp app;

    CROW_ROUTE(app, "/tasks")
        .methods(crow::HTTPMethod::Post)(
            [](const crow::request &req)
            {
                auto x = crow::json::load(req.body);
                if (!x)
                    return crow::response(400, "Bad Request");

                std::string title = x["title"].s();
                std::string description = x.has("description")
                                              ? x["description"].s()
                                              : std::string("");
                ;
                std::string status =
                    x.has("status") ? x["status"].s() : std::string("todo");

                const char *sql = "INSERT INTO tasks (title, description, "
                                  "status) VALUES (?, ?, ?);";
                sqlite3_stmt *stmt;

                if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
                    return crow::response(500, "Database Error");

                sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 2, description.c_str(), -1,
                                  SQLITE_STATIC);
                sqlite3_bind_text(stmt, 3, status.c_str(), -1, SQLITE_STATIC);

                if (sqlite3_step(stmt) != SQLITE_DONE)
                {
                    sqlite3_finalize(stmt);
                    return crow::response(500, "Failed to insert task");
                }

                long long new_id = sqlite3_last_insert_rowid(db);
                sqlite3_finalize(stmt);

                Task new_task{(int)new_id, title, description, status};
                return crow::response(201, new_task.to_json());
            });

    CROW_ROUTE(app, "/tasks")
        .methods(crow::HTTPMethod::Get)(
            []()
            {
                std::vector<crow::json::wvalue> result_list;
                const char *sql =
                    "SELECT id, title, description, status FROM tasks;";
                sqlite3_stmt *stmt;

                if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK)
                {
                    while (sqlite3_step(stmt) == SQLITE_ROW)
                    {
                        Task t;
                        t.id = sqlite3_column_int(stmt, 0);
                        t.title = reinterpret_cast<const char *>(
                            sqlite3_column_text(stmt, 1));
                        const char *d = reinterpret_cast<const char *>(
                            sqlite3_column_text(stmt, 2));
                        t.description = d ? d : "";
                        const char *s = reinterpret_cast<const char *>(
                            sqlite3_column_text(stmt, 3));
                        t.status = s ? s : "todo";
                        result_list.push_back(t.to_json());
                    }
                }
                sqlite3_finalize(stmt);

                crow::json::wvalue final_json =
                    crow::json::wvalue::list(result_list);
                return crow::response(200, final_json);
            });

    CROW_ROUTE(app, "/tasks/<int>")
        .methods(crow::HTTPMethod::Get)(
            [](int id)
            {
                Task t;
                if (!get_task_from_db(id, t))
                {
                    return crow::response(404, "Task not found");
                }
                return crow::response(200, t.to_json());
            });

    CROW_ROUTE(app, "/tasks/<int>")
        .methods(crow::HTTPMethod::Put)(
            [](const crow::request &req, int id)
            {
                auto x = crow::json::load(req.body);
                if (!x)
                    return crow::response(400, "Bad JSON");

                Task t;
                if (!get_task_from_db(id, t))
                {
                    return crow::response(404, "Task not found");
                }

                std::string new_title =
                    x.has("title") ? x["title"].s() : t.title;
                std::string new_desc =
                    x.has("description") ? x["description"].s() : t.description;
                std::string new_status =
                    x.has("status") ? x["status"].s() : t.status;

                const char *sql = "UPDATE tasks SET title = ?, description = "
                                  "?, status = ? WHERE id = ?;";
                sqlite3_stmt *stmt;

                if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
                    return crow::response(500, "DB Error");

                sqlite3_bind_text(stmt, 1, new_title.c_str(), -1,
                                  SQLITE_STATIC);
                sqlite3_bind_text(stmt, 2, new_desc.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 3, new_status.c_str(), -1,
                                  SQLITE_STATIC);
                sqlite3_bind_int(stmt, 4, id);

                sqlite3_step(stmt);
                sqlite3_finalize(stmt);

                t.title = new_title;
                t.description = new_desc;
                t.status = new_status;
                return crow::response(200, t.to_json());
            });

    CROW_ROUTE(app, "/tasks/<int>")
        .methods(crow::HTTPMethod::Delete)(
            [](int id)
            {
                Task t;
                if (!get_task_from_db(id, t))
                {
                    return crow::response(404, "Task not found");
                }

                const char *sql = "DELETE FROM tasks WHERE id = ?;";
                sqlite3_stmt *stmt;
                if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
                    return crow::response(500, "DB Error");

                sqlite3_bind_int(stmt, 1, id);
                sqlite3_step(stmt);
                sqlite3_finalize(stmt);

                return crow::response(200, t.to_json());
            });

    CROW_ROUTE(app, "/stop")(
        [&app]()
        {
            app.stop();
            if (db)
                sqlite3_close(db);
            return "App shutdown";
        });

    app.port(1499).multithreaded().run();
}
