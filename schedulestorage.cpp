#include "schedulestorage.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>

ScheduleStorage::ScheduleStorage(QString fileName)
    : m_fileName(std::move(fileName))
{
}

bool ScheduleStorage::save(const Schedule &schedule, QString &errorMessage) const
{
    errorMessage.clear();

    QJsonObject root;
    root["scheduleName"] = "Розклад співбесід";

    QJsonObject daysObject;
    const QMap<QDate, QVector<TimeSlot>> &days = schedule.allDays();
    for (auto it = days.constBegin(); it != days.constEnd(); ++it)
    {
        QJsonArray slotsArray;
        for (const TimeSlot &slot : it.value())
        {
            QJsonObject slotObject;
            slotObject["time"] = slot.time;
            slotObject["isBooked"] = slot.isBooked;
            slotObject["bookedBy"] = slot.bookedBy;
            slotsArray.append(slotObject);
        }
        daysObject[it.key().toString(Qt::ISODate)] = slotsArray;
    }
    root["days"] = daysObject;

    QFile file(m_fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        errorMessage = QString("Не вдалося зберегти розклад у файл %1").arg(m_fileName);
        return false;
    }

    file.write(QJsonDocument(root).toJson());
    file.close();
    return true;
}

bool ScheduleStorage::load(Schedule &schedule, QString &errorMessage) const
{
    errorMessage.clear();

    QFile file(m_fileName);
    if (!file.exists())
    {
        return true;
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        errorMessage = QString("Не вдалося відкрити файл розкладу %1").arg(m_fileName);
        return false;
    }

    const QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError || !doc.isObject())
    {
        errorMessage = "Файл розкладу пошкоджено або має неправильний формат.\n"
                       "Буде створено новий розклад.";
        return false;
    }

    const QJsonObject root = doc.object();
    const QJsonObject daysObject = root["days"].toObject();

    for (auto it = daysObject.constBegin(); it != daysObject.constEnd(); ++it)
    {
        const QDate date = QDate::fromString(it.key(), Qt::ISODate);
        if (!date.isValid()) continue;

        const QJsonArray slotsArray = it.value().toArray();
        QVector<TimeSlot> daySlots = Schedule::defaultSlots();

        for (int i = 0; i < slotsArray.size() && i < daySlots.size(); ++i)
        {
            const QJsonObject slotObject = slotsArray[i].toObject();
            daySlots[i].time = slotObject["time"].toString(daySlots[i].time);
            daySlots[i].isBooked = slotObject["isBooked"].toBool();
            daySlots[i].bookedBy = slotObject["bookedBy"].toString();
        }

        schedule.setDay(date, daySlots);
    }

    return true;
}