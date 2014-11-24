#ifndef FILTERPROFILE_H
#define FILTERPROFILE_H

# include <QString>
# include <QJsonDocument>
# include <QJsonArray>
# include <QJsonObject>
# include <functional>

class FilterProfile
{
public:
    FilterProfile(const QString& name);

    static QStringList filterList(void);

    void load(void);
    void save(void);

    void forEachFilter(std::function<void (QJsonObject object, unsigned short index)>);
    void addFilter(QJsonObject);
    void removeFilter(int index);

private:
    const QString filePath(void)    const;
    QJsonArray    filterArray(void) const;

    static const QString profilesPath;
    QString              name;
    QJsonDocument        document;
};

#endif // FILTERPROFILE_H
