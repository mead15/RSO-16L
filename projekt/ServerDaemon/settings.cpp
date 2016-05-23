#include "settings.h"

Settings::Settings(QString& path):QSettings (path, QSettings::NativeFormat)
{
    settingsFilePath = path;
}


