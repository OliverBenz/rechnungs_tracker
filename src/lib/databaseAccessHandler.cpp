#include "databaseAccessHandler.hpp"

#include <fmt/format.h>
#include <iostream>
#include <sqlite3.h>

namespace lib {

class billDbHandler::SqlitePimpl {
public:
    // TODO: Avoid raw pointer or make copy/move safe
    sqlite3* m_db{nullptr};
};


billDbHandler::billDbHandler()
    : m_sqlite(std::make_unique<billDbHandler::SqlitePimpl>()) {
	open();
}

billDbHandler::~billDbHandler() {
	sqlite3_close(m_sqlite->m_db);
    m_sqlite->m_db = nullptr;
}

bool billDbHandler::addCategory(const std::string& name) {
	static constexpr char QUERY_FORMAT[] = "INSERT INTO category (name) VALUES ('{}');";

    if (name.empty()) {
		return false;
	}

    // Write into db
    const std::string query = fmt::format(QUERY_FORMAT, sanitize(name));
    char* messageError;
    if (sqlite3_exec(m_sqlite->m_db, query.c_str(), nullptr, nullptr, &messageError) != SQLITE_OK) {
        std::cerr << "[DB] Error Insertion: " << messageError << std::endl;
        sqlite3_free(messageError);
        return false;
    }

    std::cout << "[DB] Inserted new Category\n";
    return true;
}

bool billDbHandler::addUsage(const std::string& name, int categoryId) {
    static constexpr char QUERY_FORMAT[] = "INSERT INTO usage (name, categoryId) VALUES ('{}', {});";
    if (name.empty() || categoryId == 0) {
        std::cout << "[DB] Failed to add usage class. Missing or invalid information.\n";
        return false;
    }

    // Write into db
    const std::string query = fmt::format(QUERY_FORMAT, sanitize(name), categoryId);
    char* messageError;
    if (sqlite3_exec(m_sqlite->m_db, query.c_str(), nullptr, nullptr, &messageError) != SQLITE_OK) {
        std::cerr << "[DB] Error Insertion: " << messageError << std::endl;
        sqlite3_free(messageError);
        return false;
    }

    std::cout << "[DB] Inserted new usage class.\n";
    return true;
}

bool billDbHandler::addShop(const std::string& name) {
    static constexpr char QUERY_FORMAT[] = "INSERT INTO shops (name) VALUES ('{}');";
    if (name.empty()) {
        std::cout << "[DB] Failed to add shop. Name cannot be empty.\n";
        return false;
    }

    // Write into db
    const std::string query = fmt::format(QUERY_FORMAT, sanitize(name));
    char* messageError;
    if (sqlite3_exec(m_sqlite->m_db, query.c_str(), nullptr, nullptr, &messageError) != SQLITE_OK) {
        std::cerr << "[DB] Error Insertion: " << messageError << std::endl;
        sqlite3_free(messageError);
        return false;
    }

    std::cout << "[DB] Inserted new shop.\n";
    return true;
}

bool billDbHandler::addBill(const std::string& date, float price, unsigned shopId, unsigned usageId, const std::string& filename) {
    static constexpr char QUERY_FORMAT[] = "INSERT INTO bills (date, price, shopId, usageId, filename) VALUES ('{}', {}, {}, {}, '{}');";
    if (date.empty() || usageId == 0 || shopId == 0 || price == 0.f) {
        std::cout << "[DB] Failed to add bill. Missing or invalid information.\n";
        return false;
    }

    // Write into db
    const std::string query = fmt::format(QUERY_FORMAT, sanitize(date), price, shopId, usageId, sanitize(filename));
    char* messageError;
    if (sqlite3_exec(m_sqlite->m_db, query.c_str(), nullptr, nullptr, &messageError) != SQLITE_OK) {
        std::cerr << "[DB] Error Insertion: " << messageError << '\n';
        sqlite3_free(messageError);
        return false;
    }

    std::cout << "[DB] Inserted new bill.\n";
    return true;
}

std::vector<usage> billDbHandler::getAllUsages() {
    static constexpr char QUERY[] = "SELECT id, categoryId, name FROM usage";

    sqlite3_stmt* statement;
    if(sqlite3_prepare_v2(m_sqlite->m_db, QUERY, -1, &statement, nullptr) != SQLITE_OK) {
        std::cerr << "[DB] Error reading usages.\n";
    }

    std::vector<usage> usages;
    while(sqlite3_step(statement) == SQLITE_ROW) {
        usages.emplace_back(
            static_cast<unsigned>(sqlite3_column_int(statement, 0)),
            static_cast<unsigned>(sqlite3_column_int(statement, 1)),
            std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 2)))
        );
    }

    sqlite3_finalize(statement);
    return usages;
}



std::string billDbHandler::getUsageName(unsigned id) {
    static constexpr char QUERY[] = "SELECT name FROM usage WHERE id IS {}";

    sqlite3_stmt* statement;
    if(sqlite3_prepare_v2(m_sqlite->m_db, fmt::format(QUERY, id).c_str(), -1, &statement, nullptr) != SQLITE_OK) {
        std::cerr << "[DB] Error reading usage name.\n";
    }

    if (sqlite3_step(statement) != SQLITE_ROW) {
        throw std::invalid_argument("Usage id does not exist.");
    }

    const auto usageName = std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 0)));
    assert(sqlite3_step(statement) == SQLITE_DONE); // Make sure only one result.

    return usageName;
}

std::string billDbHandler::getShopName(unsigned id) {
    static constexpr char QUERY[] = "SELECT name FROM shops WHERE id IS {}";

    sqlite3_stmt* statement;
    if(sqlite3_prepare_v2(m_sqlite->m_db, fmt::format(QUERY, id).c_str(), -1, &statement, nullptr) != SQLITE_OK) {
        std::cerr << "[DB] Error reading shop name.\n";
    }

    if (sqlite3_step(statement) != SQLITE_ROW) {
        throw std::invalid_argument("Shop id does not exist.");
    }

    const auto shopName = std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 0)));
    assert(sqlite3_step(statement) == SQLITE_DONE); // Make sure only one result.

    return shopName;
}

std::string billDbHandler::getCategoryName(unsigned id) {
    static constexpr char QUERY[] = "SELECT name FROM category WHERE id IS {}";

    sqlite3_stmt* statement;
    if(sqlite3_prepare_v2(m_sqlite->m_db, fmt::format(QUERY, id).c_str(), -1, &statement, nullptr) != SQLITE_OK) {
        std::cerr << "[DB] Error reading category name.\n";
    }

    if (sqlite3_step(statement) != SQLITE_ROW) {
        throw std::invalid_argument("Category id does not exist.");
    }

    const auto categoryName = std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 0)));
    assert(sqlite3_step(statement) == SQLITE_DONE); // Make sure only one result.

    return categoryName;
}

std::string billDbHandler::sanitize(const std::string& value) {
	// Don't care for now. TODO: Implement
	return value;
}

void billDbHandler::open() {
    // TODO: Allow user to change db path
    static constexpr char dbPath[] ="/home/oliver/.billtracker/data/expense_planner.db";
    static constexpr char FOREIGN_KEYS_ON[] = "PRAGMA foreign_keys = ON;";
	if (m_sqlite->m_db != nullptr) {
		return;
	}

	if (sqlite3_open(dbPath, &m_sqlite->m_db) != SQLITE_OK) {
		std::cerr << "[DB] Error connecting to Bill Database:  " << sqlite3_errmsg(m_sqlite->m_db) << std::endl;
		throw std::invalid_argument(fmt::format("Could not open database: '{}'", dbPath));
	} else {
		std::cout << "[DB] Connected to Bill Database." << std::endl;
	}

    // Enable foreign key constraint
    char* errorMsg;
    if (sqlite3_exec(m_sqlite->m_db, FOREIGN_KEYS_ON, 0, 0, &errorMsg) != SQLITE_OK) {
        std::cout << "[DB] Failed to enable foreign key constraint: " << errorMsg << '\n';
        sqlite3_free(errorMsg);
    }
}

}
