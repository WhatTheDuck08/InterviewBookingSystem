#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QInputDialog>
#include <QLineEdit>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_slots =
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

    setupUI();
    loadScheduleFromFile();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    this->setWindowTitle("Система бронювання слотів для співбесід");
    this->resize(600, 400);

    QWidget *centralWidget = ui->centralwidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QLabel *headerLabel = new QLabel("Оберіть час для співбесіди:", this);
    headerLabel->setStyleSheet("font-size: 16px; font-weight: bold; margin-bottom: 10px;");
    mainLayout->addWidget(headerLabel);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(10);

    for (int i = 0; i < m_slots.size(); ++i)
    {
        QPushButton *btn = new QPushButton(this);
        btn->setProperty("slotIndex", i);

        connect(btn, &QPushButton::clicked, this, &MainWindow::onSlotButtonClicked);

        m_slotButtons.append(btn);
        updateButtonVisuals(btn, m_slots[i].isBooked);

        int row = i / 2;
        int col = i % 2;
        gridLayout->addWidget(btn, row, col);
    }

    mainLayout->addLayout(gridLayout);
}

void MainWindow::onSlotButtonClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int index = btn->property("slotIndex").toInt();

    if (!m_slots[index].isBooked)
    {
        bool ok;
        QString name = QInputDialog::getText(this, "Бронювання слота","Введіть ваше ім'я:", QLineEdit::Normal, "", &ok);

        if (ok && !name.trimmed().isEmpty())
        {
            m_slots[index].isBooked = true;
            m_slots[index].bookedBy = name.trimmed();
        }
        else
        {
            return;
        }
    }
    else
    {

        m_slots[index].isBooked = false;
        m_slots[index].bookedBy = "";
    }

    updateButtonVisuals(btn, m_slots[index].isBooked);
    saveScheduleToFile();
}

void MainWindow::updateButtonVisuals(QPushButton* button, bool isBooked)
{
    int index = button->property("slotIndex").toInt();
    QString timeStr = m_slots[index].time;

    if (isBooked)
    {
        QString clientName = m_slots[index].bookedBy;
        button->setText(timeStr + "\n[Зайнято: " + clientName + "]");
        button->setStyleSheet("background-color: #e74c3c; color: white; font-weight: bold; padding: 12px; border-radius: 6px;");
    }
    else
    {
        button->setText(timeStr + "\n[Вільно]");
        button->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold; padding: 12px; border-radius: 6px;");
    }
}

void MainWindow::saveScheduleToFile()
{
    QJsonArray slotsArray;
    for (int i = 0; i < m_slots.size(); ++i)
    {
        QJsonObject slotObject;
        slotObject["time"] = m_slots[i].time;
        slotObject["isBooked"] = m_slots[i].isBooked;
        slotObject["bookedBy"] = m_slots[i].bookedBy;
        slotsArray.append(slotObject);
    }
    QJsonDocument doc(slotsArray);
    QFile file(m_fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(doc.toJson());
        file.close();
    }
}

void MainWindow::loadScheduleFromFile()
{
    QFile file(m_fileName);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) return;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) return;

    QJsonArray slotsArray = doc.array();
    for (int i = 0; i < slotsArray.size() && i < m_slots.size(); ++i)
    {
        QJsonObject slotObject = slotsArray[i].toObject();
        m_slots[i].isBooked = slotObject["isBooked"].toBool();
        m_slots[i].bookedBy = slotObject["bookedBy"].toString();
        updateButtonVisuals(m_slotButtons[i], m_slots[i].isBooked);
    }
}