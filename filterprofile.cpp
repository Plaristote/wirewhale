#include "filterprofile.h"
#include <QFile>
#include <QDir>
#include <QDirIterator>

using namespace std;

const QString FilterProfile::profilesPath = "profiles/";

FilterProfile::FilterProfile(const QString& name) : name(name)
{
}

QStringList FilterProfile::filterList()
{
  QDirIterator iterator(profilesPath, QDirIterator::Subdirectories);
  QStringList  filters;

  while (iterator.hasNext())
  {
    QFileInfo fileInfo;

    iterator.next();
    fileInfo = QFileInfo(iterator.filePath());
    if (fileInfo.isFile() && fileInfo.suffix() == "json")
      filters << fileInfo.baseName();
  }
  return (filters);
}

void FilterProfile::load()
{
  QFile file(filePath());

  if (file.open(QIODevice::ReadOnly))
    document = QJsonDocument::fromJson(file.readAll());
}

void FilterProfile::save()
{
  QFile file(filePath());

  if (file.open(QIODevice::WriteOnly))
    file.write(document.toJson());
}

const QString FilterProfile::filePath() const
{
    return profilesPath + name + ".json";
}

void FilterProfile::forEachFilter(function<void (QJsonObject object, unsigned short index)> block)
{
  QJsonArray array = filterArray();

  for (unsigned short index = 0 ; index < array.size() ; ++index)
    block(array[index].toObject(), index);
}

void FilterProfile::removeFilter(int index)
{
  QJsonArray array = filterArray();

  array.removeAt(index);
}

QJsonArray FilterProfile::filterArray() const
{
  return document.object()["filters"].toArray();
}
