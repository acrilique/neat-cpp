#ifndef UTILS_H
#define UTILS_H

#include <QStringList>
#include <string>
#include <vector>

namespace utils {

std::string ensure_neat_directory();
void save_state(const std::string &file_path, const std::string &last_folder,
                const std::vector<std::string> &recent_files);
std::tuple<std::string, std::string, std::vector<std::string>> load_state();
void log_session(const std::string &message);
QStringList stdVectorToQStringList(const std::vector<std::string> &vec);
std::vector<std::string> QStringListToStdVector(const QStringList &list);
} // namespace utils

#endif // UTILS_H