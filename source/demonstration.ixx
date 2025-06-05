module;
#include <pqxx/pqxx>
#include <memory>
export module demo;
export import math:thing;

export namespace Demo {
    class Eve {
    public:
        void create_task(pqxx::connection &conn);
        Eve(const std::string &conn_uri) {
            reader(std::make_unique<pqxx::connection>(conn_uri));
            writer(std::make_unique<pqxx::connection>(conn_uri));
            deleter(std::make_unique<pqxx::connection>(conn_uri));
        };
    private:
        std::unique_ptr<pqxx::connection> reader;
        std::unique_ptr<pqxx::connection> writer;
        std::unique_ptr<pqxx::connection> deleter;
    }
}

// Needs to have some key value pair thing so that the db connections
// are an array or something instead of three free floating ones.
// easier to return for a getter and shit like that.
