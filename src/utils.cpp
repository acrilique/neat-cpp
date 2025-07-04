#include "utils.h"
#include "json.hpp"
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits.h>
#include <unistd.h>

namespace fs = std::filesystem;
namespace json = nlohmann::json_abi_v3_11_3;

namespace utils {

std::string ensure_neat_directory() {
  fs::path neat_dir;

  // First try XDG_DATA_HOME (Flatpak-compatible)
  if (const char *xdg_data_home = std::getenv("XDG_DATA_HOME")) {
    neat_dir = fs::path(xdg_data_home) / "neat";
  }
  // Fall back to ~/.local/share (XDG Base Directory spec)
  else if (const char *home = std::getenv("HOME")) {
    neat_dir = fs::path(home) / ".local" / "share" / "neat";
  }
  // Last resort: fall back to ~/.neat (legacy behavior)
  else if (const char *userprofile = std::getenv("USERPROFILE")) {
    neat_dir = fs::path(userprofile) / ".neat";
  } else {
    throw std::runtime_error("Could not determine user data directory");
  }

  if (!fs::exists(neat_dir)) {
    fs::create_directories(neat_dir);
  }

  return neat_dir.string();
}

void save_state(const std::string &file_path, const std::string &last_folder,
                const std::vector<std::string> &recent_files) {
  std::string neat_dir = ensure_neat_directory();
  fs::path state_file = fs::path(neat_dir) / "state.json";

  json::json state = {{"last_opened_file", file_path},
                      {"last_accessed_folder", last_folder},
                      {"recent_files", recent_files}};

  std::ofstream file(state_file);
  file << state.dump(4);
}

std::tuple<std::string, std::string, std::vector<std::string>> load_state() {
  std::string neat_dir = ensure_neat_directory();
  fs::path state_file = fs::path(neat_dir) / "state.json";

  if (fs::exists(state_file)) {
    std::ifstream file(state_file);
    json::json state;
    file >> state;

    return std::make_tuple(
        state["last_opened_file"].get<std::string>(),
        state["last_accessed_folder"].get<std::string>(),
        state["recent_files"].get<std::vector<std::string>>());
  }

  return std::make_tuple("", "", std::vector<std::string>());
}

void log_session(const std::string &message) {
  std::string neat_dir = ensure_neat_directory();
  fs::path log_file = fs::path(neat_dir) / "log.txt";

  std::ofstream file(log_file, std::ios_base::app);

  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);

  std::stringstream ss;
  ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");

  file << ss.str() << " - " << message << std::endl;
}

QStringList stdVectorToQStringList(const std::vector<std::string> &vec) {
  QStringList list;
  for (const auto &str : vec) {
    list.append(QString::fromStdString(str));
  }
  return list;
}

std::vector<std::string> QStringListToStdVector(const QStringList &list) {
  std::vector<std::string> vec;
  for (const auto &str : list) {
    vec.push_back(str.toStdString());
  }
  return vec;
}

} // namespace utils
