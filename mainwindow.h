#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVector>
#include <QString>

struct TimeSlot {
    QString time;
    bool isBooked;
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onSlotButtonClicked();

private:
    void setupUI();
    void updateButtonVisuals(QPushButton* button, bool isBooked);

    Ui::MainWindow *ui;
    QVector<TimeSlot> m_slots;
    QVector<QPushButton*> m_slotButtons;
};

#endif // MAINWINDOW_H