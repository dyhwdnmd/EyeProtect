#pragma once

#include <QtWidgets/QMainWindow>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QSlider>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QGraphicsOpacityEffect>
#include <QEasingCurve>
#include "ui_QtWidgetsApplication1.h"

class QtWidgetsApplication1 : public QMainWindow
{
    Q_OBJECT

public:
    QtWidgetsApplication1(QWidget *parent = nullptr);
    ~QtWidgetsApplication1();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onBlueFilterSliderChanged(int value);
    void onBrightnessSliderChanged(int value);
    void onTemperatureSliderChanged(int value);
    void onModeComboBoxChanged(int index);
    void onReminderCheckBoxChanged(bool checked);
    void onReminderIntervalChanged(int index);
    void onActionButtonClicked();
    void showEyeBreakReminder();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void startInitialAnimations();

private:
    Ui::EyeCareApp ui;
    QTimer *reminderTimer;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    
    // 动画相关
    QPropertyAnimation *titleAnimation;
    QPropertyAnimation *subtitleAnimation;
    QSequentialAnimationGroup *initialAnimationGroup;
    QParallelAnimationGroup *controlsAnimationGroup;
    QGraphicsOpacityEffect *titleOpacityEffect;
    QGraphicsOpacityEffect *subtitleOpacityEffect;
    
    void setupConnections();
    void setupTrayIcon();
    void setupAnimations();
    void addIconsToUI();
    void applyBlueFilter(int intensity);
    void adjustBrightness(int level);
    void adjustColorTemperature(int temperature);
    void updateReminderTimer(int minutes);
    void animateSliderChange(QSlider *slider, int value);
};

