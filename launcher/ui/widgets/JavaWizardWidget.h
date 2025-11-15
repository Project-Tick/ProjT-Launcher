// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 *
 *  If this file includes work from previous open-source projects,
 *  their original copyright and license notices are preserved below.
 */
#pragma once
#include <QWidget>

#include <BaseVersion.h>
#include <QObjectPtr.h>
#include <java/JavaChecker.h>
#include <qcheckbox.h>
#include <QIcon>

class QLineEdit;
class VersionSelectWidget;
class QSpinBox;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QGroupBox;
class QGridLayout;
class QLabel;
class QToolButton;
class QSpacerItem;

class JavaWizardWidget : public QWidget {
    Q_OBJECT

   public:
    explicit JavaWizardWidget(QWidget* parent);
    virtual ~JavaWizardWidget();

    enum class JavaStatus { NotSet, Pending, Good, DoesNotExist, DoesNotStart, ReturnedInvalidData } javaStatus = JavaStatus::NotSet;

    enum class ValidationStatus { Bad, JavaBad, AllOK };

    void refresh();
    void initialize();
    ValidationStatus validate();
    void retranslate();

    bool permGenEnabled() const;
    int permGenSize() const;
    int minHeapSize() const;
    int maxHeapSize() const;
    QString javaPath() const;
    bool autoDetectJava() const;
    bool autoDownloadJava() const;

    void updateThresholds();

   protected slots:
    void onSpinBoxValueChanged(int);
    void memoryValueChanged();
    void javaPathEdited(const QString& path);
    void javaVersionSelected(BaseVersion::Ptr version);
    void on_javaBrowseBtn_clicked();
    void on_javaStatusBtn_clicked();
    void javaDownloadBtn_clicked();
    void checkFinished(const JavaChecker::Result& result);

   protected: /* methods */
    void checkJavaPathOnEdit(const QString& path);
    void checkJavaPath(const QString& path);
    void setJavaStatus(JavaStatus status);
    void setupUi();

   private: /* data */
    VersionSelectWidget* m_versionWidget = nullptr;
    QVBoxLayout* m_verticalLayout = nullptr;
    QSpacerItem* m_verticalSpacer = nullptr;

    QLineEdit* m_javaPathTextBox = nullptr;
    QPushButton* m_javaBrowseBtn = nullptr;
    QToolButton* m_javaStatusBtn = nullptr;
    QHBoxLayout* m_horizontalLayout = nullptr;

    QGroupBox* m_memoryGroupBox = nullptr;
    QGridLayout* m_gridLayout_2 = nullptr;
    QSpinBox* m_maxMemSpinBox = nullptr;
    QLabel* m_labelMinMem = nullptr;
    QLabel* m_labelMaxMem = nullptr;
    QLabel* m_labelMaxMemIcon = nullptr;
    QSpinBox* m_minMemSpinBox = nullptr;
    QLabel* m_labelPermGen = nullptr;
    QSpinBox* m_permGenSpinBox = nullptr;

    QHBoxLayout* m_horizontalBtnLayout = nullptr;
    QPushButton* m_javaDownloadBtn = nullptr;
    QIcon goodIcon;
    QIcon yellowIcon;
    QIcon badIcon;

    QGroupBox* m_autoJavaGroupBox = nullptr;
    QVBoxLayout* m_veriticalJavaLayout = nullptr;
    QCheckBox* m_autodetectJavaCheckBox = nullptr;
    QCheckBox* m_autodownloadCheckBox = nullptr;

    unsigned int observedMinMemory = 0;
    unsigned int observedMaxMemory = 0;
    unsigned int observedPermGenMemory = 0;
    QString queuedCheck;
    uint64_t m_availableMemory = 0ull;
    shared_qobject_ptr<JavaChecker> m_checker;
    JavaChecker::Result m_result;
    QTimer* m_memoryTimer;
};
