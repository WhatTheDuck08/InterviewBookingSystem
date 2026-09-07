#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QLocale>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_schedule(QDate::currentDate())
    , m_storage("schedule.json")
{
    ui->setupUi(this);

    m_currentDate = m_schedule.startDate();

    setupUI();

    QString errorMessage;
    if (!m_storage.load(m_schedule, errorMessage))
    {
        QMessageBox::warning(this, "Помилка завантаження", errorMessage);
    }

    showDay(m_currentDate);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    setWindowTitle("Система бронювання слотів для співбесід");
    resize(640, 480);

    QWidget *centralWidget = ui->centralwidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(12);

    QLabel *headerLabel = new QLabel("Оберіть час для співбесіди:", this);
    headerLabel->setStyleSheet("font-size: 16px; font-weight: 600; color: #2c3e50;");
    mainLayout->addWidget(headerLabel);

    QHBoxLayout *navLayout = new QHBoxLayout();

    m_prevDayBtn = new QPushButton("◀ Попередній день", this);
    m_nextDayBtn = new QPushButton("Наступний день ▶", this);
    m_dateLabel = new QLabel(this);
    m_dateLabel->setAlignment(Qt::AlignCenter);
    m_dateLabel->setStyleSheet("font-size: 15px; font-weight: bold; color: #34495e;");

    const QString navBtnStyle =
        "QPushButton {"
        "  background-color: #3498db; color: white; border: none;"
        "  padding: 8px 14px; border-radius: 6px; font-weight: 600;"
        "}"
        "QPushButton:hover { background-color: #2980b9; }"
        "QPushButton:disabled { background-color: #bdc3c7; color: #ecf0f1; }";
    m_prevDayBtn->setStyleSheet(navBtnStyle);
    m_nextDayBtn->setStyleSheet(navBtnStyle);

    connect(m_prevDayBtn, &QPushButton::clicked, this, &MainWindow::onPrevDayClicked);
    connect(m_nextDayBtn, &QPushButton::clicked, this, &MainWindow::onNextDayClicked);

    navLayout->addWidget(m_prevDayBtn);
    navLayout->addWidget(m_dateLabel, 1);
    navLayout->addWidget(m_nextDayBtn);
    mainLayout->addLayout(navLayout);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet("font-size: 13px; color: #7f8c8d;");
    mainLayout->addWidget(m_statusLabel);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(10);

    const int slotCount = Schedule::defaultSlots().size();
    for (int i = 0; i < slotCount; ++i)
    {
        QPushButton *btn = new QPushButton(this);
        btn->setProperty("slotIndex", i);
        btn->setMinimumHeight(56);

        connect(btn, &QPushButton::clicked, this, &MainWindow::onSlotButtonClicked);

        m_slotButtons.append(btn);

        const int row = i / 2;
        const int col = i % 2;
        gridLayout->addWidget(btn, row, col);
    }

    mainLayout->addLayout(gridLayout);
}

void MainWindow::showDay(const QDate &date)
{
    m_currentDate = date;
    const QVector<TimeSlot> &daySlots = m_schedule.slotsForDay(date);

    QLocale ukLocale(QLocale::Ukrainian);
    m_dateLabel->setText(ukLocale.toString(date, "dddd, d MMMM yyyy"));

    for (int i = 0; i < m_slotButtons.size() && i < daySlots.size(); ++i)
    {
        updateButtonVisuals(m_slotButtons[i], daySlots[i]);
    }

    m_statusLabel->setText(QString("Вільно слотів: %1 з %2")
                               .arg(m_schedule.freeSlotCount(date))
                               .arg(m_schedule.totalSlotCount()));

    m_prevDayBtn->setEnabled(date > m_schedule.startDate());
}

void MainWindow::persistSchedule()
{
    QString errorMessage;
    if (!m_storage.save(m_schedule, errorMessage))
    {
        QMessageBox::warning(this, "Помилка збереження", errorMessage);
    }
}

void MainWindow::onSlotButtonClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    const int index = btn->property("slotIndex").toInt();
    const QVector<TimeSlot> &daySlots = m_schedule.slotsForDay(m_currentDate);
    if (index < 0 || index >= daySlots.size()) return;

    const TimeSlot slot = daySlots[index];

    if (!slot.isBooked)
    {
        bool ok = false;
        const QString name = QInputDialog::getText(this, "Бронювання слота",
                                                   "Введіть ім'я кандидата:",
                                                   QLineEdit::Normal, "", &ok);
        if (!ok || name.trimmed().isEmpty()) return;

        if (!m_schedule.bookSlot(m_currentDate, index, name))
        {
            QMessageBox::warning(this, "Помилка", "Не вдалося забронювати цей слот.");
            return;
        }
    }
    else
    {
        const QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Скасування бронювання",
            QString("Скасувати бронювання слота %1 (%2)?").arg(slot.time, slot.bookedBy),
            QMessageBox::Yes | QMessageBox::No);

        if (reply != QMessageBox::Yes) return;

        m_schedule.cancelSlot(m_currentDate, index);
    }

    persistSchedule();
    showDay(m_currentDate);
    offerNextDayIfFull();
}

void MainWindow::offerNextDayIfFull()
{
    if (!m_schedule.isDayFullyBooked(m_currentDate)) return;

    const QMessageBox::StandardButton reply = QMessageBox::information(
        this, "Місця закінчились",
        "Усі слоти на цей день заброньовано.\nПерейти на наступний день?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        showDay(m_currentDate.addDays(1));
    }
}

void MainWindow::onNextDayClicked()
{
    showDay(m_currentDate.addDays(1));
}

void MainWindow::onPrevDayClicked()
{
    if (m_currentDate > m_schedule.startDate())
    {
        showDay(m_currentDate.addDays(-1));
    }
}

void MainWindow::updateButtonVisuals(QPushButton* button, const TimeSlot &slot)
{
    if (slot.isBooked)
    {
        button->setText(slot.time + "\n[Зайнято: " + slot.bookedBy + "]");
        button->setStyleSheet(
            "QPushButton {"
            "  background-color: #e74c3c; color: white; font-weight: bold;"
            "  padding: 12px; border-radius: 8px; border: none;"
            "}"
            "QPushButton:hover { background-color: #c0392b; }");
    }
    else
    {
        button->setText(slot.time + "\n[Вільно]");
        button->setStyleSheet(
            "QPushButton {"
            "  background-color: #2ecc71; color: white; font-weight: bold;"
            "  padding: 12px; border-radius: 8px; border: none;"
            "}"
            "QPushButton:hover { background-color: #27ae60; }");
    }
}