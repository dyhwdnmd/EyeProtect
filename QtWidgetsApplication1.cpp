#include "QtWidgetsApplication1.h"
#include <QMessageBox>
#include <QScreen>
#include <QDesktopServices>
#include <QUrl>
#include <QDesktopWidget>
#include <QApplication>
#include <QShowEvent>

QtWidgetsApplication1::QtWidgetsApplication1(QWidget *parent)
    : QMainWindow(parent), reminderTimer(new QTimer(this))
{
    ui.setupUi(this);
    setWindowTitle("EyeCare");
    
    // Set window icon
    setWindowIcon(QIcon(":/QtWidgetsApplication1/icons/eye.png"));
    
    // 确保所有控件默认可见
    ui.blueFilterGroup->setVisible(true);
    ui.brightnessGroup->setVisible(true);
    ui.modeGroup->setVisible(true);
    ui.reminderGroup->setVisible(true);
    ui.actionButton->setVisible(true);
    
    // 如果要使用动画效果，取消注释下面的代码
    // setupAnimations();
    
    // Setup connections
    setupConnections();
    
    // Setup system tray icon
    setupTrayIcon();
    
    // Initialize reminder timer
    reminderTimer->setSingleShot(false);
    updateReminderTimer(20); // Default to 20 minutes
    
    // Connect slider value changes to label updates
    connect(ui.blueFilterSlider, &QSlider::valueChanged, [this](int value) {
        ui.blueFilterValueLabel->setText(QString::number(value) + "%");
    });
    
    connect(ui.brightnessSlider, &QSlider::valueChanged, [this](int value) {
        ui.brightnessValueLabel->setText(QString::number(value) + "%");
    });
    
    connect(ui.temperatureSlider, &QSlider::valueChanged, [this](int value) {
        ui.temperatureValueLabel->setText(QString::number(value) + "%");
    });
    
    // Setup slider animation connections
    connect(ui.blueFilterSlider, &QSlider::sliderPressed, [this]() {
        ui.blueFilterSlider->setProperty("animating", true);
    });
    
    connect(ui.brightnessSlider, &QSlider::sliderPressed, [this]() {
        ui.brightnessSlider->setProperty("animating", true);
    });
    
    connect(ui.temperatureSlider, &QSlider::sliderPressed, [this]() {
        ui.temperatureSlider->setProperty("animating", true);
    });
    
    connect(ui.blueFilterSlider, &QSlider::sliderReleased, [this]() {
        ui.blueFilterSlider->setProperty("animating", false);
        animateSliderChange(ui.blueFilterSlider, ui.blueFilterSlider->value());
    });
    
    connect(ui.brightnessSlider, &QSlider::sliderReleased, [this]() {
        ui.brightnessSlider->setProperty("animating", false);
        animateSliderChange(ui.brightnessSlider, ui.brightnessSlider->value());
    });
    
    connect(ui.temperatureSlider, &QSlider::sliderReleased, [this]() {
        ui.temperatureSlider->setProperty("animating", false);
        animateSliderChange(ui.temperatureSlider, ui.temperatureSlider->value());
    });
}

QtWidgetsApplication1::~QtWidgetsApplication1()
{
    delete trayIcon;
    delete trayIconMenu;
    delete initialAnimationGroup;
    delete controlsAnimationGroup;
}

void QtWidgetsApplication1::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    
    // 动画功能已禁用
    /*
    // Only play animations the first time the window is shown
    static bool firstShow = true;
    if (firstShow) {
        QTimer::singleShot(100, this, &QtWidgetsApplication1::startInitialAnimations);
        firstShow = false;
    }
    */
}

void QtWidgetsApplication1::setupAnimations()
{
    // Title animation
    titleAnimation = new QPropertyAnimation(titleOpacityEffect, "opacity", this);
    titleAnimation->setDuration(800);
    titleAnimation->setStartValue(0.0);
    titleAnimation->setEndValue(1.0);
    titleAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    // Subtitle animation
    subtitleAnimation = new QPropertyAnimation(subtitleOpacityEffect, "opacity", this);
    subtitleAnimation->setDuration(800);
    subtitleAnimation->setStartValue(0.0);
    subtitleAnimation->setEndValue(1.0);
    subtitleAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    // Initial animation group (sequential)
    initialAnimationGroup = new QSequentialAnimationGroup(this);
    initialAnimationGroup->addAnimation(titleAnimation);
    initialAnimationGroup->addPause(200);
    initialAnimationGroup->addAnimation(subtitleAnimation);
    
    // Controls animation group (parallel)
    controlsAnimationGroup = new QParallelAnimationGroup(this);
    
    // Add animations for each control group
    QList<QWidget*> widgets = {ui.blueFilterGroup, ui.brightnessGroup, ui.modeGroup, ui.reminderGroup, ui.actionButton};
    
    for (int i = 0; i < widgets.size(); ++i) {
        QWidget* widget = widgets[i];
        
        // Create position animation
        QPropertyAnimation *posAnim = new QPropertyAnimation(widget, "pos", this);
        posAnim->setDuration(600 + i * 100);
        posAnim->setStartValue(QPoint(widget->x() + 50, widget->y()));
        posAnim->setEndValue(widget->pos());
        posAnim->setEasingCurve(QEasingCurve::OutBack);
        
        // Create opacity animation
        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(effect);
        effect->setOpacity(0);
        
        QPropertyAnimation *opacityAnim = new QPropertyAnimation(effect, "opacity", this);
        opacityAnim->setDuration(600 + i * 100);
        opacityAnim->setStartValue(0.0);
        opacityAnim->setEndValue(1.0);
        opacityAnim->setEasingCurve(QEasingCurve::OutCubic);
        
        // Add to parallel animation group
        controlsAnimationGroup->addAnimation(posAnim);
        controlsAnimationGroup->addAnimation(opacityAnim);
    }
}

void QtWidgetsApplication1::startInitialAnimations()
{
    // Play title and subtitle animations
    initialAnimationGroup->start();
    
    // Show controls and play controls animations after title animation finishes
    connect(initialAnimationGroup, &QSequentialAnimationGroup::finished, 
        this, [this]() {
        ui.blueFilterGroup->setVisible(true);
        ui.brightnessGroup->setVisible(true);
        ui.modeGroup->setVisible(true);
        ui.reminderGroup->setVisible(true);
        ui.actionButton->setVisible(true);
        
        controlsAnimationGroup->start();
    });
}

void QtWidgetsApplication1::animateSliderChange(QSlider *slider, int value)
{
    // Don't animate if slider is being dragged
    if (slider->property("animating").toBool()) {
        return;
    }
    
    // Create a simple animation effect for smooth slider value changes
    QPropertyAnimation *animation = new QPropertyAnimation(slider, "value", this);
    animation->setDuration(300);
    animation->setStartValue(slider->value());
    animation->setEndValue(value);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void QtWidgetsApplication1::setupConnections()
{
    // Connect sliders and other controls to their respective slots
    connect(ui.blueFilterSlider, &QSlider::valueChanged, this, &QtWidgetsApplication1::onBlueFilterSliderChanged);
    connect(ui.brightnessSlider, &QSlider::valueChanged, this, &QtWidgetsApplication1::onBrightnessSliderChanged);
    connect(ui.temperatureSlider, &QSlider::valueChanged, this, &QtWidgetsApplication1::onTemperatureSliderChanged);
    connect(ui.modeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &QtWidgetsApplication1::onModeComboBoxChanged);
    connect(ui.reminderCheckBox, &QCheckBox::toggled, this, &QtWidgetsApplication1::onReminderCheckBoxChanged);
    connect(ui.reminderIntervalComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &QtWidgetsApplication1::onReminderIntervalChanged);
    connect(ui.actionButton, &QPushButton::clicked, this, &QtWidgetsApplication1::onActionButtonClicked);
    connect(reminderTimer, &QTimer::timeout, this, &QtWidgetsApplication1::showEyeBreakReminder);
}

void QtWidgetsApplication1::setupTrayIcon()
{
    trayIconMenu = new QMenu(this);
    
    QAction *showAction = new QAction(tr("Open EyeCare"), this);
    connect(showAction, &QAction::triggered, this, &QWidget::show);
    trayIconMenu->addAction(showAction);
    
    QAction *hideAction = new QAction(tr("Minimize to Tray"), this);
    connect(hideAction, &QAction::triggered, this, &QWidget::hide);
    trayIconMenu->addAction(hideAction);
    
    trayIconMenu->addSeparator();
    
    QAction *quitAction = new QAction(tr("Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    trayIconMenu->addAction(quitAction);
    
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/QtWidgetsApplication1/icons/eye.png"));
    trayIcon->setToolTip(tr("EyeCare - Protecting your eyes"));
    trayIcon->show();
    
    connect(trayIcon, &QSystemTrayIcon::activated, this, &QtWidgetsApplication1::trayIconActivated);
}

void QtWidgetsApplication1::onBlueFilterSliderChanged(int value)
{
    applyBlueFilter(value);
}

void QtWidgetsApplication1::onBrightnessSliderChanged(int value)
{
    adjustBrightness(value);
}

void QtWidgetsApplication1::onTemperatureSliderChanged(int value)
{
    adjustColorTemperature(value);
}

void QtWidgetsApplication1::onModeComboBoxChanged(int index)
{
    // Enable or disable temperature slider based on mode
    bool isCustomMode = (index == 3); // Index 3 is "Custom"
    ui.temperatureSlider->setEnabled(isCustomMode);
    ui.temperatureLabel->setEnabled(isCustomMode);
    ui.temperatureValueLabel->setEnabled(isCustomMode);
    
    // Apply predefined settings based on mode
    switch (index) {
    case 0: // Auto
        // Logic for auto mode based on time of day would go here
        break;
    case 1: // Warm (Evening)
        ui.temperatureSlider->setValue(75);
        adjustColorTemperature(75);
        break;
    case 2: // Cool (Daytime)
        ui.temperatureSlider->setValue(25);
        adjustColorTemperature(25);
        break;
    case 3: // Custom
        // Use current slider value
        adjustColorTemperature(ui.temperatureSlider->value());
        break;
    }
}

void QtWidgetsApplication1::onReminderCheckBoxChanged(bool checked)
{
    if (checked) {
        int minutes = 20; // Default
        switch (ui.reminderIntervalComboBox->currentIndex()) {
        case 0: minutes = 20; break;
        case 1: minutes = 30; break;
        case 2: minutes = 60; break;
        }
        updateReminderTimer(minutes);
    } else {
        reminderTimer->stop();
    }
}

void QtWidgetsApplication1::onReminderIntervalChanged(int index)
{
    if (ui.reminderCheckBox->isChecked()) {
        int minutes = 20; // Default
        switch (index) {
        case 0: minutes = 20; break;
        case 1: minutes = 30; break;
        case 2: minutes = 60; break;
        }
        updateReminderTimer(minutes);
    }
}

void QtWidgetsApplication1::onActionButtonClicked()
{
    // Apply button click animation
    QPropertyAnimation *animation = new QPropertyAnimation(ui.actionButton, "geometry", this);
    animation->setDuration(300);
    QRect startGeom = ui.actionButton->geometry();
    QRect endGeom = startGeom;
    animation->setStartValue(startGeom);
    animation->setKeyValueAt(0.5, QRect(startGeom.x() + 5, startGeom.y() + 5, startGeom.width() - 10, startGeom.height() - 10));
    animation->setEndValue(endGeom);
    animation->setEasingCurve(QEasingCurve::OutBounce);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    
    // Apply all current settings
    applyBlueFilter(ui.blueFilterSlider->value());
    adjustBrightness(ui.brightnessSlider->value());
    
    int modeIndex = ui.modeComboBox->currentIndex();
    if (modeIndex == 3) { // Custom mode
        adjustColorTemperature(ui.temperatureSlider->value());
    } else {
        onModeComboBoxChanged(modeIndex);
    }
    
    QMessageBox::information(this, 
        tr("Settings Applied"), 
        tr("Your eye protection settings have been applied successfully."));
}

void QtWidgetsApplication1::showEyeBreakReminder()
{
    if (!isVisible()) {
        trayIcon->showMessage(
            tr("Eye Break Reminder"), 
            tr("Time for a 20-second break! Look at something 20 feet away for 20 seconds."),
            QSystemTrayIcon::Information, 
            5000);
    } else {
        QMessageBox reminderBox;
        reminderBox.setWindowTitle(tr("Eye Break Reminder"));
        reminderBox.setText(tr("Time for a 20-second break!"));
        reminderBox.setInformativeText(
            tr("Look at something 20 feet away for 20 seconds to reduce eye strain."));
        reminderBox.setIcon(QMessageBox::Information);
        reminderBox.exec();
    }
}

void QtWidgetsApplication1::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        if (!isVisible()) {
            show();
        } else {
            hide();
        }
    }
}

void QtWidgetsApplication1::applyBlueFilter(int intensity)
{
    // In a real implementation, this would use platform-specific APIs
    // to adjust the blue light filter. This is a placeholder.
    qDebug("Applying blue filter with intensity: %d%%", intensity);
}

void QtWidgetsApplication1::adjustBrightness(int level)
{
    // In a real implementation, this would use platform-specific APIs
    // to adjust screen brightness. This is a placeholder.
    qDebug("Adjusting brightness to: %d%%", level);
}

void QtWidgetsApplication1::adjustColorTemperature(int temperature)
{
    // In a real implementation, this would use platform-specific APIs
    // to adjust color temperature. This is a placeholder.
    qDebug("Adjusting color temperature to: %d%%", temperature);
}

void QtWidgetsApplication1::updateReminderTimer(int minutes)
{
    if (ui.reminderCheckBox->isChecked()) {
        reminderTimer->start(minutes * 60 * 1000); // Convert minutes to milliseconds
    } else {
        reminderTimer->stop();
    }
}

