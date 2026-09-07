#ifndef SCHEDULESTORAGE_H
#define SCHEDULESTORAGE_H
#include <QString>
#include "schedule.h"

class ScheduleStorage
{
public:
    explicit ScheduleStorage(QString fileName = "schedule.json");

    bool load(Schedule &schedule, QString &errorMessage) const;
    bool save(const Schedule &schedule, QString &errorMessage) const;

private:
    QString m_fileName;
};

#endif