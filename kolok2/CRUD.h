#include "crow_all.h"
#include <string>

struct Task
{
    int id;
    std::string title;
    std::string description;
    std::string status;

    crow::json::wvalue to_json() const
    {
        crow::json::wvalue j;
        j["id"] = id;
        j["title"] = title;
        j["description"] = description;
        j["status"] = status;
        return j;
    }
};

std::map<int, Task> tasks_db;
std::mutex db_mutex;
int next_id = 1;

void Runner()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/tasks")
        .methods(crow::HTTPMethod::Post)(
            [](const crow::request &req)
            {
                auto x = crow::json::load(req.body);
                if (!x)
                    return crow::response(400, "Bad Request");

                std::lock_guard<std::mutex> lock(db_mutex);

                Task new_task;
                new_task.id = next_id++;
                new_task.title = x["title"].s();
                new_task.description = x["description"].s();
                new_task.status = "todo";

                tasks_db[new_task.id] = new_task;

                return crow::response(201, new_task.to_json());
            });

    CROW_ROUTE(app, "/tasks")
    (
        []()
        {
            std::lock_guard<std::mutex> lock(db_mutex);
            std::vector<crow::json::wvalue> result_list;
            for (const auto &kv : tasks_db)
            {
                result_list.push_back(kv.second.to_json());
            }
            crow::json::wvalue final_json =
                crow::json::wvalue::list(result_list);
            return crow::response(200, final_json);
        });

    CROW_ROUTE(app, "/tasks/<int>")
    (
        [](int id)
        {
            std::lock_guard<std::mutex> lock(db_mutex);

            if (tasks_db.find(id) == tasks_db.end())
                return crow::response(404, "Task not found");

            return crow::response(200, tasks_db[id].to_json());
        });

    CROW_ROUTE(app, "/stop")(
        [&app]()
        {
            app.stop();
            return "App shotdown";
        });
    app.port(1499).run();
}
