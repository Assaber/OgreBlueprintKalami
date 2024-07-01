#include "GlobalSettingsWidget.h"
#include "common_obk.h"
#include "AppConfig.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPixmap>
#include <QLabel>
#include <QScrollArea>
#include <QGridLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>

GlobalSettingsWidget::GlobalSettingsWidget()
    : super(nullptr)
{
    setWindowTitle("Config Setting");

    setFixedSize(600, 400);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);

    QLabel* logoLabel = new QLabel(this);
    logoLabel->setPixmap(QPixmap(":/icon/img/ogrelogo-small.jpg").scaledToWidth(600));
    mainLayout->addWidget(logoLabel);

    // settings
    QScrollArea* settingSrollArea = new QScrollArea(this);
    settingSrollArea->setStyleSheet("QScrollArea{ border: none;}");
    settingSrollArea->setWidgetResizable(true);
    mainLayout->addWidget(settingSrollArea);

    QWidget* settingWidget = new QWidget(this);
    QGridLayout* innerLayout = new QGridLayout(settingWidget);

    innerLayout->addWidget(new QLabel("Render System", this), 0, 0, Qt::AlignRight);
    QComboBox* renderSystemSelector = new QComboBox(this);
    QStringList rsNameList = AppConfig::getRenderSystemNames();
    QString rsName = AppConfig::getSingleton()->getRenderSystemName(AppConfig::getSingleton()->getRenderSystem());
    renderSystemSelector->addItems(rsNameList);
    if (rsNameList.indexOf(rsName) > -1) {
        renderSystemSelector->setCurrentText(rsName);
    }
    innerLayout->addWidget(renderSystemSelector, 0, 1);

    innerLayout->addWidget(new QLabel("Media Location", this), 1, 0, Qt::AlignRight);
    QHBoxLayout* locationLayout = new QHBoxLayout();
    locationLayout->setMargin(0);
    QLineEdit* locationDisplay = new QLineEdit(this);
    locationDisplay->setText(AppConfig::getSingleton()->getMediaLocation());
    locationDisplay->setReadOnly(true);
    QPushButton* loadLocationBtn = new QPushButton(this);
    loadLocationBtn->setFixedWidth(30);
    loadLocationBtn->setText("...");
    QObject::connect(loadLocationBtn, &QPushButton::clicked, this, [this, locationDisplay]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select media location", locationDisplay->text());
        if (!dir.isEmpty()) {
            locationDisplay->setText(dir);
        }
        });
    locationLayout->addWidget(locationDisplay);
    locationLayout->addWidget(loadLocationBtn);
    innerLayout->addLayout(locationLayout, 1, 1);

    innerLayout->setColumnStretch(0, 3);
    innerLayout->setColumnStretch(1, 7);

    settingSrollArea->setWidget(settingWidget);

    // buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch(1);
    QPushButton* okBtn = new QPushButton(this);
    okBtn->setText("Ok");
    QObject::connect(okBtn, &QPushButton::clicked, this, [this, renderSystemSelector, locationDisplay]() {
        auto app = AppConfig::getSingleton();
        app->setRenderSystem(AppConfig::getRenderSystemByName(renderSystemSelector->currentText()));
        app->setMediaLocation(locationDisplay->text());

        if (!app->saveConfig()) 
        {
            QMessageBox::critical(this, "Error", "Can not save config to local.\n Please check if there are any blanks, or illegal input");
            return;
        }
        
        this->accept();
        });
    btnLayout->addWidget(okBtn);
    QPushButton* cancelBtn = new QPushButton(this);
    cancelBtn->setText("Cancel");
    QObject::connect(cancelBtn, &QPushButton::clicked, this, [this]() {
        this->reject();
        });
    btnLayout->addWidget(cancelBtn);

    mainLayout->addLayout(btnLayout);
}
