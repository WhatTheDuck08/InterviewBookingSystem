#ifndef SCHEDULE_H
#define SCHEDULE_H
#include <QMap>
#include <QVector>
#include <QDate>
#include "timeslot.h"

class Schedule
{
public:
    explicit Schedule(QDate startDate = QDate::currentDate());

    void ensureDayExists(const QDate &date);
    const QVector<TimeSlot> &slotsForDay(const QDate &date);

    bool bookSlot(const QDate &date, int index, const QString &name);
    bool cancelSlot(const QDate &date, int index);

    bool isDayFullyBooked(const QDate &date) const;
    int freeSlotCount(const QDate &date) const;
    int totalSlotCount() const;

    QDate startDate() const { return m_startDate; }

    const QMap<QDate, QVector<TimeSlot>> &allDays() const { return m_days; }
    void setDay(const QDate &date, const QVector<TimeSlot> &daySlots);
    void clear();

    static QVector<TimeSlot> defaultSlots();

private:
    QDate m_startDate;
    QMap<QDate, QVector<TimeSlot>> m_days;
};

#endif