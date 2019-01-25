/* Copyright (c) 2019, Nikita Grebenyuk (@nikich340). All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#pragma once

#include <QtWidgets>

#define upn(x, init, n) for(int x = init; x <= n; ++x)
#define to_str(a) QString::number(a)
#define VERSION "1.0"

class SimpleAsker : public QStackedWidget {
Q_OBJECT
private:
    /* GENERAL */
    QSettings m_settings;
    QVBoxLayout *m_pLayoutMain, *m_pLayoutMenu, *m_pLayoutPreAsk, *m_pLayoutAsk, *m_pLayoutMore;
    QWidget *m_pWidgetMenu, *m_pWidgetPreAsk, *m_pWidgetAsk, *m_pWidgetMore;

    /* MENU */
    QCheckBox *m_pCheckRus;
    QPushButton *m_pBtnMenu[4];

    /* SETTINGS */
    QDialog *m_pDialogSettings;

    /* PREASK */
    QLabel *m_pLblStartFrom;
    QLineEdit *m_pLineStartFrom;
    QCheckBox *m_pCheckRandomize;
    QPushButton *m_pBtnStart;

    /* ASK */
    static const int maxAns = 6;
    QLabel *m_pLblQuestion, *m_pLblInfo, *m_pLblAns[maxAns];
    QPushButton *m_pBtnRight, *m_pBtnNext, *m_pBtnFinish, *m_pBtnAns[maxAns];

    /* OTHER */
    bool m_bLangRu = true, m_bLatin = true;
    int dbg_spacing = 0;
    int q_sum = 0, q_rightAnsCnt = 0, q_cnt = 1, q_ansType = 0;

    struct ask {
        QString question;
        QVector<QString> answers;
        QString rightAns;
    };

    QVector<ask> q_unusedAsks;

    QDialog* createDialog(QString info, QString pix, QString accept, QString reject, bool mod);
    QPushButton* setUpBtn(QLabel* pLbl);
    bool isDigit(QChar c);
    bool isSymbol(QChar c);
    bool isUpper(QChar c);
    int rand(int L, int R);
    void _dbg_start(QString func);
    void _dbg_end(QString func);
    void clearLayout(QLayout* layout);
    void crash(QString reason);
    void genOsteoQuest();
    void processOsteoXml();
    void setUpObjects();
    void readAsks(QString pathQuest, QString pathAns);
    void updateInfoLabel();
public:
    SimpleAsker(QStackedWidget *pswgt = nullptr);
    virtual ~SimpleAsker();
public slots:
    void onAboutProgram();
    void onAns();
    void onFinishAsk();
    void onFinishOsteoAsk();
    void onMenu();
    void onNextOsteoAsk();
    void onPreStartOsteoAsk();
    void onSettings();
    void onStartAsk();
    void onStartOsteoAsk();
    void onUpdateLanguage(int check);
};
