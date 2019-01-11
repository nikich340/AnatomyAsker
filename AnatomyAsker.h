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

#include "GraphicsView.h"
#include <QtWidgets>
#include <QtXml>

#define upn(x, init, n) for(int x = init; x <= n; ++x)
#define to_str(a) QString::number(a)
#define dbg(a) qDebug() << a
#define VERSION 3.1

class AnatomyAsker : public QStackedWidget {
Q_OBJECT
private:
    /* GENERAL */
    QSettings m_settings;
    QVBoxLayout *m_pLayoutMain, *m_pLayoutMenu, *m_pLayoutPreAsk, *m_pLayoutAsk, *m_pLayoutMore;
    QWidget *m_pWidgetMenu, *m_pWidgetPreAsk, *m_pWidgetAsk, *m_pWidgetMore;

    /* MENU */
    QCheckBox *m_pCheckRus, *m_pCheckLatin;

    /* SETTINGS */
    QDialog *m_pDialogSettings;
    QPushButton *m_pBtnMenu[5];

    /* PREASK */
    QPushButton *m_pBtnStart, *m_pBtnMore;
    QTreeWidget *m_pTreeOsteo;

    /* MORE */
    GraphicsView *m_pGraphicsViewMore;
    QLabel *m_pLblMore;
    QPushButton *m_pBtnNextPix, *m_pBtnBack;

    QVector<QPair<int, QString>> morePixVect;
    QString moreText;
    int morePixNum;

    /* ASK */
    static const int maxAns = 6;
    GraphicsView *m_pGraphicsView;
    QLabel *m_pLblQuestion, *m_pLblInfo, *m_pLblAns[maxAns];
    QPushButton *m_pBtnRight, *m_pBtnNext, *m_pBtnFinish, *m_pBtnAns[maxAns];

    /* OTHER */
    bool m_bLangRu = true, m_bLatin = true;
    int dbg_spacing = 0;
    int q_sum = 0, q_rightAnsCnt = 0, q_cnt = 1, q_ansType = 0;

    QMap<QString, QDomElement> findElementByName;
    QDomDocument osteoDoc;
    QVector<QDomElement> unusedOsteos;

    QDialog* createDialog(QString info, QString pix, QString accept, QString reject, bool mod);
    QPushButton* setUpBtn(QLabel* pLbl);
    QString findMark(QVector<QPair<int, QString>>& pixVect, int pixNum);
    QString elName(QDomElement& element);
    QString parseLinks(QString text);
    QTreeWidget* viewOsteoTree();
    bool isDigit(QChar c);
    bool isUpper(QChar c);
    int rand(int L, int R);
    void _dbg_start(QString func);
    void _dbg_end(QString func);
    void chooseOsteoQuests(QString root);
    void clearLayout(QLayout* layout);
    void crash(QString reason);
    void genOsteoQuest();
    void parsePixMarks(QVector<QPair<int, QString>>& pixVect, QString pixStr);
    void processOsteoXml();
    void processOsteoXmlDfs(QDomElement& curEl);
    void setUpObjects();
    void sortOsteoXml();
    void sortOsteoXmlDfs(QDomElement& curEl);
    void readXml(QDomDocument& doc, QString path);
    void updateGView(bool crutch);
    void updateInfoLabel();
    void viewOsteoTreeDfs(QDomElement& parEl, QTreeWidgetItem* pTWI);
    void writeXml(QDomDocument& doc, QString path);
public:
    AnatomyAsker(QStackedWidget *pswgt = nullptr);
    virtual ~AnatomyAsker();
public slots:
    void onAns();
    void onFinishAsk();
    void onFinishOsteoAsk();
    void onMenu();
    void onMore();
    void onMoreNextPix();
    void onNextOsteoAsk();
    void onPreStartOsteoAsk();
    void onSettings();
    void onStartAsk();
    void onStartOsteoAsk();
    void onTreeCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void onUpdateLanguage(int check);
};
