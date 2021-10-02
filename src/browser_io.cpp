// Copyright (c) 2021 Jeffrey D. Hirschberg
#ifdef __EMSCRIPTEN__
#include <fstream>
#include <filesystem>
#include "config.h"

extern "C" {
  void setSavePath(char *scoresPath, char *savePath) {
    config::files::scores = scoresPath;
    config::files::save_game = savePath;

    // scores.dat is opened as rb+, which means the first time Umoria
    // runs in a browser environment, it needs to touch the file to
    // make sure it exists
    if (!(std::filesystem::exists(scoresPath))) {
      std::ofstream(config::files::scores, std::ios_base::out);
    }
  }
}
#endif
