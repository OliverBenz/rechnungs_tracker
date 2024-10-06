#pragma once

#include <string>

namespace lib {

inline static constexpr char APP_NAME[] = "Bill Tracker";
inline static constexpr char APP_AUTHOR[] = "Oliver Benz";

//! Creates the config file if not yet exists.
//! \returns False if it needs to be created but could not be.
bool initializeConfig();

//! Get the path to the database file.
std::string getDatabasePath();

//! Get the path to the backup folder.
std::string getBackupPath();

}