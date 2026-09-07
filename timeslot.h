#ifndef TIMESLOT_H
#define TIMESLOT_H

#include <QString>

struct TimeSlot
{
    QString time;
    bool isBooked = false;
    QString bookedBy;
};

#endif