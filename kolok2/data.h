#ifndef data
#define data

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

#endif