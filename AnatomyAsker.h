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
#define VERSION 2.3

class AnatomyAsker : public QWidget {
Q_OBJECT
private:
    static const int maxAns = 6;
    GraphicsView* m_pGraphicsView;
    QCheckBox *m_pCheckRus, *m_pCheckLatin;
    QDialog *m_pDialogSettings = nullptr;
    QLabel *m_pLblQuestion, *m_pLblInfo, *m_pLblAns[maxAns];
    QPushButton *m_pBtnRight, *m_pBtnNext, *m_pBtnFinish, *m_pBtnPre, *m_pBtnSet[5], *m_pBtnAns[maxAns];
    QSettings m_settings;
    QTreeWidget* m_pTW = nullptr;
    QVBoxLayout* m_pLayoutMain, *m_pLayoutMenu, *m_pLayoutPreAsk, *m_pLayoutAsk;
    QWidget *m_pWidgetMenu, *m_pWidgetPreAsk, *m_pWidgetAsk;

    bool m_bLangRu = true;
    bool m_bLatin = true;
    int dbg_spacing = 0;
    int q_sum = 0;
    int q_cnt = 1;
    int q_rightAnsCnt = 0;
    int q_ansType = 0; // 0 - undefined, 1 - right, -1 - wrong

    QDomDocument osteoDoc;
    QVector<QDomElement> unusedOsteos;

    QDialog* createDialog(QString info, QString accept, QString reject, bool mod);
    QPushButton* setUpBtn(QLabel* pLbl);
    QString findMark(QVector<QPair<int, QString>>& pixVect, int pixNum);
    QString elName(QDomElement& element);
    QTreeWidget* viewOsteoTree();
    bool isDigit(QChar c);
    bool isUpper(QChar c);
    int rand(int L, int R);
    void chooseOsteoQuests(QString root);
    void clearLayout(QLayout* layout);
    void crash(QString reason);
    void genOsteoQuest();
    void parsePixMarks(QVector<QPair<int, QString>>& pixVect, QString pixStr);
    void processOsteoXml();
    void processOsteoXmlDfs(QDomElement& curEl);
    void sortOsteoXml();
    void sortOsteoXmlDfs(QDomElement& curEl, int& cnt, int delta);
    void readXml(QDomDocument& doc, QString path);
    void updateGView(bool crutch);
    void updateInfoLabel();
    void viewOsteoTreeDfs(QDomElement& parEl, QTreeWidgetItem* pTWI);
    void writeXml(QDomDocument& doc, QString path);
public:
    AnatomyAsker(QWidget *pwgt = nullptr);
    virtual ~AnatomyAsker();
public slots:
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
