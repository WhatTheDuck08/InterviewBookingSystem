#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVector>
#include <QDate>

#include "schedule.h"
#include "schedulestorage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onSlotButtonClicked();
    void onNextDayClicked();
    void onPrevDayClicked();

private:
    void setupUI();
    void updateButtonVisuals(QPushButton *button, const TimeSlot &slot);
    void showDay(const QDate &date);
    void offerNextDayIfFull();
    void persistSchedule();

    Ui::MainWindow *ui;

    Schedule m_schedule;
    ScheduleStorage m_storage;

    QVector<QPushButton*> m_slotButtons;
    QDate m_currentDate;

    QLabel *m_dateLabel = nullptr;
    QLabel *m_statusLabel = nullptr;
    QPushButton *m_prevDayBtn = nullptr;
    QPushButton *m_nextDayBtn = nullptr;
};

#endif