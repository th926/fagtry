
// C++ HEADERS
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <string.h>
#include <fstream>

// POSTGRES BINDINGS
#include <pqxx/pqxx>

// DEFINE
#define CONNECTION_URI "dbname=WILLDO user=postgres password=darkjoe host=localhost port=5432"

// CRUD

bool create_task_table(pqxx::connection &conn) {
    try {
        pqxx::work slave{ conn };
        slave.exec(R"(
            CREATE TABLE IF NOT EXISTS tasks (
            id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
            name TEXT NOT NULL,
            description TEXT NOT NULL,
            created_timestamp DATE DEFAULT CURRENT_TIMESTAMP,
            completed_timestamp DATE,
            repeating BOOLEAN,
            completed BOOLEAN DEFAULT false)
        )");
        std::cout << "Created table...\n";
        slave.commit();
    } catch (const pqxx::sql_error &e) {
        std::cerr << "SQL Error: " << e.what() << "\n";
        std::cerr << "Query: " << e.query() << "\n";
        return true;
    }
    return false;
}

bool create_task(pqxx::connection &conn, std::string name, std::string description, bool repeating) {
    std::string query{ "INSERT INTO tasks (name, description, repeating) VALUES ($1, $2, $3)" };
    try {
        pqxx::work slave{ conn };
        pqxx::params params{ name, description, repeating };
        slave.exec( query, params );
        std::cout << "Creating task\n";
        slave.commit();
    } catch (const pqxx::sql_error &e) {
        std::cerr << "SQL Error: " << e.what() << "\n";
        std::cerr << "Query: " << e.query() << "\n";
        return true;
    }
    return false;
}

bool delete_task(pqxx::connection &conn, std::string name) {
    std::string query{ "DELETE FROM tasks WHERE name = $1" };
    try {
        pqxx::work slave{ conn };
        pqxx::params params{ name };
        slave.exec( query, params );
        std::cout << "Deleted " << name << '\n';
        slave.commit();
    } catch (const pqxx::sql_error &e) {
        std::cerr << "There was an error deleting the task";
        std::cerr << "SQL Error: " << e.what() << "\n";
        std::cerr << "Query: " << e.query() << "\n";
        return true;
    }
    return false;
}

bool change_task(pqxx::connection conn, std::string task, std::string new_desc) {
    std::string query{ "UPDATE tasks SET description = $2 WHERE name = $1" };
    try {
        pqxx::work slave{ conn };
        pqxx::params params { task, new_desc };
        slave.exec( query, params );
        slave.commit();
    } catch (const pqxx::sql_error &e) {
        std::cerr << "SQL Error: " << e.what() << "\n";
        std::cerr << "Query: " << e.query() << "\n";
        return true;
    }
    return false;
}

// Needs fix
bool list_tasks(pqxx::connection &conn) {
    std::string query{ "SELECT name FROM tasks" };
    try {
        pqxx::work slave{ conn };
        pqxx::result res = slave.exec( query );
        std::cout << "\n\n";
        std::cout << "Tasks\n";
        std::cout << "-----------------\n";

        for (const auto &row : res) {
            std::string name = row["name"].as<std::string>();
            std::cout << name << '\n';
        }
    } catch (const pqxx::sql_error &e) {
        std::cerr << "Error listing tasks\n";
        std::cerr << "SQL Error: " << e.what();
        std::cerr << "Query: " << e.query() << "\n";
    }
    return false;
}

bool list_task(pqxx::connection &conn, std::string task) {
    std::string query{ "SELECT name, description, created_timestamp, completed_timestamp, completed WHERE name = $1" };
    try {
        pqxx::work slave{ conn };
        pqxx::params params{ task };
        pqxx::result res = slave.exec( query, params);
        for (const auto &row : res) {
            std::string name            = row["name"].as<std::string>();
            std::string description     = row["description"].as<std::string>();
            std::string created_stamp   = row["created_timestamp"].as<std::string>();
            bool completed              = row["completed"].as<bool>();
            std::string completed_stamp;
            if (completed)
                completed_stamp = row["completed_timestamp"].as<bool>();

            std::cout << name << '\n';
            std::cout << "created: " << created_stamp << "\n\n";
            std::cout << description << '\n';
            if (completed) {
                std::cout << "completed";
            } else {
                std::cout << "not completed";
            }
        }
    } catch (const pqxx::sql_error &e) {
        std::cerr << "SQL Error: " << e.what() << '\n';
        std::cerr << "Query: " << e.query() << '\n';
    }
    return false;
}

bool complete(pqxx::connection &conn, std::string task) {
    std::string query{ "UPDATE tasks SET completed = true, completed_timestamp = CURRENT_TIMESTAMP WHERE name = $1" };
    try {
        pqxx::work slave{ conn };
        pqxx::params params{ task };
        slave.exec(query, params );
        slave.commit();
    } catch (const pqxx::sql_error &e) {
        std::cerr << "SQL Error: " << e.what() << "\n";
        std::cerr << "Query: " << e.query() << "\n";
        return true;
    }
    return false;
}

std::unique_ptr<pqxx::connection> establish_connection(std::string username, std::string passord) {
    std::string query{ "dbname=WILLDO user=$1 password=$2 host=localhost port=5432" };
    if (!username.empty() || !passord.empty()) {
        // Bad form using the numbers instead of something else...
        query.replace(query.find("$1"), 2, username);
        query.replace(query.find("$2"), 2, passord);
    }
    try {
        std::unique_ptr<pqxx::connection> conn = std::make_unique<pqxx::connection>( query );
        return conn;
    } catch (const pqxx::sql_error &e) {
        std::cerr << "SQL Error: " << e.what() << "\n";
        std::cerr << "Query: " << e.query() << "\n";
        exit(1);
    }
}

void help_out() {
    std::cout << "Basic usage\n";
    std::cout << "[variable] indicates more than one variable\n";
    std::cout << "<variable> indicates a single variable\n";
    std::cout << "list                                              list name of all tasks\n";
    std::cout << "list <task name>                                  list details of specific task\n";
    std::cout << "create -n <name> -d <description>                 create new task\n";
    std::cout << "complete <name>                                   complete a task\n";
    std::cout << "change -n <name> -d <description>                 change a task\n";
    std::cout << "delete <name>                                     delete a task\n";
}

// HERE!
int main(int argc, char **argv) {
    try {
        // Connection
        std::unique_ptr<pqxx::connection> reader = establish_connection("wd_read_all_data", "placeholder");
        std::unique_ptr<pqxx::connection> writer = establish_connection("wd_write_all_data", "placeholder");
        std::unique_ptr<pqxx::connection> deleter = establish_connection("wd_delete_all_data", "placeholder");

        std::cout << "Connecting to databases\n";
        if (!reader->is_open() || !writer->is_open() || !deleter->is_open()) {
            std::cerr << "Connections to databse failed\n";
        }
        std::cout << "CONNECTION STATUS.\n\n";
        std::cout << std::left
                  << std::setw(20) << "NAME"
                  << std::setw(10) << "STATUS" << '\n';
        std::cout << "------------------\n";
        std::cout << std::left
                  << std::setw(20) << reader->username()
                  << std::setw(10) << "SUCCESS" << '\n';
        std::cout << std::left
                  << std::setw(20) << writer->username()
                  << std::setw(10) << "SUCCESS" << '\n';
        std::cout << std::left
                  << std::setw(20) << deleter->username()
                  << std::setw(10) << "SUCCESS" << '\n';

        // Have this program ran before?
        std::string checkfile_path{ "dont.touch" };
        std::ifstream checkfile{ checkfile_path };
        if (!checkfile.is_open()) {
            create_task_table(*writer);
            std::ofstream createdfile{ checkfile_path };
            createdfile.close();
        } else {
            checkfile.close();
        }

        // argc test before using it lol
        if (argc < 2) {
            help_out();
        }

        for (int x = 1; x < argc; ++x) {
            std::string arg = argv[x];
            if (arg == "list" && argc-1 == x) {
                list_tasks(*reader);
                return 0;
            } else if (arg == "list" && x < argc) {
                list_task(*reader, argv[x + 1]);
                return 0;
            }
            if (arg == "create") {
                std::string name, desc;
                for (int i = x + 1; i < argc; ++i) {
                    std::string arg = argv[i];
                    if (arg == "-n") {
                        for (int y = i + 1; y < argc; ++y) {
                            std::string n_arg = argv[y];
                            if (n_arg == "-d") {
                                if (y < argc) {
                                    i = y-1;
                                }
                                name.resize(name.length()-1);
                                break;
                            }
                            name.append(n_arg);
                            name.append(" ");
                        }
                    } else if (arg == "-d") {
                        for (int y = i + 1; y < argc; ++y) {
                            std::string n_arg = argv[y];
                            if (n_arg == "-n") {
                                if (y < argc) {
                                    i = y-1;
                                }
                                desc.resize(desc.length()-1);
                                break;
                            }
                            desc.append(n_arg);
                            desc.append(" ");
                        }
                    }
                    std::cout << arg << '\n';
                }
                create_task(*writer, name, desc, false);
                return 0;
            }
            if (arg == "complete") {
                std::string task;
                if (argc < 4) { // program complete -n name
                    help_out();
                    return 1;
                }
                if (!strcmp(argv[x + 1], "-n")) {
                    task = argv[x + 2];
                    // this needs a loop to handle input
                    complete(*writer, task);
                    return 0;
                } else {
                    help_out();
                    return 1;
                }
            }
            if (arg == "delete") {
                if (argc < 4) {
                    help_out();
                    return 1;
                } // program delete -n task
                std::string n_arg = argv[x++];
                if (n_arg == "-n") {
                    delete_task(*deleter, argv[x + 2]);
                    return 0;
                } else {
                    help_out();
                    return 1;
                }
            }
        }

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
} // Main
