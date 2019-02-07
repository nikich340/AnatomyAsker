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
#include <QMediaPlayer>
#include <set>
#include <queue>

#define upn(x, init, n) for(int x = init; x <= n; ++x)
#define to_str(a) QString::number(a)
#define dbg(a) qDebug() << a
#define VERSION "4.0"

enum class Section { OSTEOLOGIA, ARTROSYNDESMOLOGIA, MYOLOGIA };
using namespace std;

class AnatomyAsker : public QStackedWidget {
Q_OBJECT
private:
    /* GENERAL */
    QMediaPlayer *m_pMediaPlayer;
    QSettings m_settings;
    QVBoxLayout *m_pLayoutMain, *m_pLayoutMenu, *m_pLayoutPreAsk, *m_pLayoutAsk, *m_pLayoutMore;
    QWidget *m_pWidgetMenu, *m_pWidgetPreAsk, *m_pWidgetAsk, *m_pWidgetMore;
    Section m_section;

    /* MENU */
    QCheckBox *m_pCheckRus, *m_pCheckLatin;
    QPushButton *m_pBtnMenu[6];

    /* SETTINGS */
    QDialog *m_pDialogSettings;

    /* PREASK */
    QPushButton *m_pBtnStart, *m_pBtnMore;
    QTreeWidget *m_pTreeOsteo, *m_pTreeArtro;

    /* MORE */
    GraphicsView *m_pGraphicsViewMore;
    QLabel *m_pLblMore;
    QPushButton *m_pBtnNextPix, *m_pBtnBack;

    QVector<QPair<int, QString>> morePixVect;
    QString moreText;
    int morePixNum;

    /* ASK */
    static const int maxAns = 6;
    static const int similarity = 10;
    GraphicsView *m_pGraphicsView;
    QLabel *m_pLblQuestion, *m_pLblInfo, *m_pLblAns[maxAns];
    QPushButton *m_pBtnRight, *m_pBtnNext, *m_pBtnFinish, *m_pBtnAns[maxAns];

    /* OTHER */
    bool m_bLangRu = true, m_bLatin = true;
    int dbg_spacing = 0;
    int q_sum = 0, q_rightAnsCnt = 0, q_cnt = 1, q_ansType = 0;

    QDomDocument osteoDoc, artroDoc;

    QVector<QString> nameByIndex;
    QMap<QString, int> indexByName;
    QVector<QDomElement> elementByIndex;
    QMap<QString, QDomElement> elementByName;
    map<int, vector<int>> indexesByPix;
    map<int, vector<pair<int, int>>> osteoMap, artroMap;
    set<int> formationIndexes;
    vector<int> unusedFormations;

    QDialog* createDialog(QString info, QString pix, QString accept, QString reject, bool mod);
    QPushButton* setUpBtn(QLabel* pLbl);
    QString findMark(QVector<QPair<int, QString>>& pixVect, int pixNum);
    QString elName(QDomElement& element);
    QString parseLinks(QString text);
    QTreeWidget* viewArtroTree();
    QTreeWidget* viewOsteoTree();
    bool isDigit(QChar c);
    bool isSymbol(QChar c);
    bool isUpper(QChar c);
    int rand(int L, int R);
    void _dbg_start(QString func);
    void _dbg_end(QString func);
    void chooseQuests(vector<int>& vect, QString root);
    void clearLayout(QLayout* layout);
    void crash(QString reason);
    void genQuest();
    void initXml(Section curSection);
    void initXmlDfs(QDomElement& curEl, map<int, vector<int>>& G, int parentVertex);
    void parsePixMarks(QVector<QPair<int, QString>>& pixVect, QString pixStr, bool clear);
    void processXml(Section curSection);
    void processXmlDfs(QDomElement& curEl);
    void setUpObjects();
    void sortOsteoXml();
    void sortOsteoXmlDfs(QDomElement& curEl);
    void readXml(QDomDocument& doc, QString path);
    void updateGView(bool crutch);
    void updateInfoLabel();
    void viewTreeDfs(QDomElement& parEl, QTreeWidgetItem* pTWI);
    void writeXml(QDomDocument& doc, QString path);
public:
    AnatomyAsker(QStackedWidget *pswgt = nullptr);
    virtual ~AnatomyAsker();
public slots:
    void onAboutProgram();
    void onAns();
    void onChooseOsteologia();
    void onChooseArtrosyndesmologia();
    void onFinishAsk();
    void onMenu();
    void onMore();
    void onMoreNextPix();
    void onNextAsk();
    void onPreStartAsk();
    void onSettings();
    void onStartAsk();
    void onTreeCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void onUpdateLanguage(int check);
};
