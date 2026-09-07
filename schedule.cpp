#include "schedule.h"

Schedule::Schedule(QDate startDate)
    : m_startDate(startDate)
{
}

QVector<TimeSlot> Schedule::defaultSlots()
{
    return
    {
        {"09:00 - 10:00", false, ""},
        {"10:00 - 11:00", false, ""},
        {"11:00 - 12:00", false, ""},
        {"12:00 - 13:00", false, ""},
        {"14:00 - 15:00", false, ""},
        {"15:00 - 16:00", false, ""},
        {"16:00 - 17:00", false, ""},
        {"17:00 - 18:00", false, ""}
    };
}

void Schedule::ensureDayExists(const QDate &date)
{
    if (!m_days.contains(date))
    {
        m_days.insert(date, defaultSlots());
    }
}

const QVector<TimeSlot> &Schedule::slotsForDay(const QDate &date)
{
    ensureDayExists(date);
    return m_days[date];
}

bool Schedule::bookSlot(const QDate &date, int index, const QString &name)
{
    ensureDayExists(date);
    QVector<TimeSlot> &daySlots = m_days[date];

    if (index < 0 || index >= daySlots.size()) return false;
    if (daySlots[index].isBooked) return false;
    if (name.trimmed().isEmpty()) return false;

    daySlots[index].isBooked = true;
    daySlots[index].bookedBy = name.trimmed();
    return true;
}

bool Schedule::cancelSlot(const QDate &date, int index)
{
    ensureDayExists(date);
    QVector<TimeSlot> &daySlots = m_days[date];

    if (index < 0 || index >= daySlots.size()) return false;
    if (!daySlots[index].isBooked) return false;

    daySlots[index].isBooked = false;
    daySlots[index].bookedBy.clear();
    return true;
}

bool Schedule::isDayFullyBooked(const QDate &date) const
{
    const QVector<TimeSlot> daySlots = m_days.value(date);
    if (daySlots.isEmpty()) return false;

    for (const TimeSlot &slot : daySlots)
    {
        if (!slot.isBooked) return false;
    }
    return true;
}

int Schedule::freeSlotCount(const QDate &date) const
{
    const QVector<TimeSlot> daySlots = m_days.value(date);
    int count = 0;
    for (const TimeSlot &slot : daySlots)
    {
        if (!slot.isBooked) ++count;
    }
    return count;
}

int Schedule::totalSlotCount() const
{
    return defaultSlots().size();
}

void Schedule::setDay(const QDate &date, const QVector<TimeSlot> &daySlots)
{
    m_days.insert(date, daySlots);
}

void Schedule::clear()
{
    m_days.clear();
}