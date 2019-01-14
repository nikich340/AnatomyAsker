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

#include "AnatomyAsker.h"
#include <QMediaPlayer>

/* PRIVATE FUNCTIONS */
QDialog* AnatomyAsker::createDialog(QString info, QString pix, QString accept, QString reject, bool mod) {
    QDialog* pdlg = new QDialog(this);
    QHBoxLayout* phbox = new QHBoxLayout;
    QVBoxLayout* pvbox = new QVBoxLayout;
    QLabel* plbl = new QLabel("<h3>" + info + "</h3>");
    plbl->setAlignment(Qt::AlignCenter);
    plbl->setWordWrap(true);

    if (accept != "-") {
        QPushButton* pAcc = new QPushButton(accept);
        connect(pAcc, SIGNAL(clicked(bool)), pdlg, SLOT(accept()));
        phbox->addWidget(pAcc);
    }

    if (reject != "-") {
        QPushButton* pRej = new QPushButton(reject);
        connect(pRej, SIGNAL(clicked(bool)), pdlg, SLOT(reject()));
        phbox->addWidget(pRej);
    }

    pvbox->addWidget(plbl);
    if (pix != "-") {
        QLabel* plblPix = new QLabel;
        qreal maxW = QGuiApplication::primaryScreen()->geometry().width() * 0.9;
        qreal maxH = QGuiApplication::primaryScreen()->geometry().height() * 0.7;
        plblPix->setScaledContents(true);
        plblPix->setPixmap(QPixmap(pix).scaled(QSize(maxW, maxH), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        pvbox->addWidget(plblPix);
    }
    pvbox->addLayout(phbox);
    pdlg->setModal(mod);
    pdlg->setLayout(pvbox);
    pdlg->show();
    return pdlg;
}
QPushButton* AnatomyAsker::setUpBtn(QLabel* pLbl) {
    QPushButton* pBtn = new QPushButton;
    QHBoxLayout *pLayout = new QHBoxLayout;
    pLbl->setWordWrap(true);
    pLbl->setAlignment(Qt::AlignCenter);
    pLayout->addWidget(pLbl);
    pBtn->setLayout(pLayout);
    pBtn->setStyleSheet("background-color: rgba(255,255,255,70)");
    return pBtn;
}
QTreeWidget* AnatomyAsker::viewOsteoTree() {
   _dbg_start(__func__);
   QTreeWidget* pTW = new QTreeWidget;
   pTW->setHeaderLabel(m_bLangRu ? "Выберите структуру для вопросов" : "Choose structure for questions");
   pTW->setColumnCount(1);

   QDomElement rootEl = osteoDoc.documentElement();
   QTreeWidgetItem* pRoot = new QTreeWidgetItem;
   pRoot->setText(0, elName(rootEl));
   pRoot->setText(1, rootEl.attribute("name"));
   pTW->addTopLevelItem(pRoot);
   viewOsteoTreeDfs(rootEl, pRoot);

   _dbg_end(__func__);
   return pTW;
}
QString AnatomyAsker::findMark(QVector<QPair<int, QString>>& pixVect, int pixNum) {
    for (auto &it: pixVect) {
        if (it.first == pixNum) {
            return it.second;
        }
    }
    return "-1";
}
QString AnatomyAsker::elName(QDomElement& element) {
    if (m_bLangRu) {
        if (m_bLatin) {
            return element.attribute("nameRu") + " [" + element.attribute("name") + "]";
        } else {
            return element.attribute("nameRu");
        }
    } else {
        return element.attribute("name");
    }
}
QString AnatomyAsker::parseLinks(QString text) {
    _dbg_start(__func__);
    QString ret = "";
    upn(i, 0, text.length() - 1) {
        if (text[i] == '@') {
            ++i;
            QString name = "";
            while (text[i] != '@' && i < text.length()) {
                name.push_back(text[i]);
                ++i;
            }
            ret += elName(findElementByName[name]);
        } else {
            ret.push_back(text[i]);
        }
    }
    qDebug() << QString(dbg_spacing, (QChar) ' ') << "End: " << __func__; dbg_spacing -= 3;
    return ret;
}
bool AnatomyAsker::isDigit(QChar c) {
    return (c >= '0' && c <= '9');
}
bool AnatomyAsker::isUpper(QChar c) {
    return (c >= 'A' && c <= 'Z');
}
int AnatomyAsker::rand(int L, int R) {
    qsrand(QDateTime::currentMSecsSinceEpoch());
    if (R < L)
        return -1;
    return L + qrand() % (R - L + 1);
}
void AnatomyAsker::_dbg_start(QString func) {
    QString out = "";
    upn(i, 1, dbg_spacing) {
        out.push_back(' ');
    }
    out += func;
    dbg_spacing += 3;
    qDebug() << out;
}
void AnatomyAsker::_dbg_end(QString func) {
    QString out = "";
    dbg_spacing -= 3;
    upn(i, 1, dbg_spacing) {
        out.push_back(' ');
    }
    out += func;
    qDebug() << out;
}
void AnatomyAsker::clearLayout(QLayout* layout) {
    while (QLayoutItem* item = layout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        if (QLayout* childLayout = item->layout()) {
            clearLayout(childLayout);
            childLayout->deleteLater();
        }
        if (item != nullptr) {
            delete item;
        }
    }
}
void AnatomyAsker::chooseOsteoQuests(QString rootPattern) {
    _dbg_start(__func__);
    QVector<QDomElement> tmpV;
    for (auto curEl : unusedOsteos) {
        QDomElement from = curEl;
        while (!curEl.isNull() && elName(curEl) != rootPattern) {
            curEl = curEl.parentNode().toElement();
        }
        if (!curEl.isNull())
            tmpV.push_back(from);
    }
    unusedOsteos = tmpV;
    _dbg_end(__func__);
}
void AnatomyAsker::crash(QString reason) {
    qDebug() << "Crash! Reason: " << reason << "\n";
    QDialog* pdlg = createDialog("Crash! Reason: " + reason + "\n", "-", "Quit", "Try ignore", true);
    connect(pdlg, SIGNAL(accepted()), qApp, SLOT(quit()));
    pdlg->exec();
    pdlg->deleteLater();
}
void AnatomyAsker::genOsteoQuest() {
    _dbg_start(__func__);
    //std::default_random_engine dre(QDateTime::currentMSecsSinceEpoch());
    QSet<QString> ans;
    QString rightAns;
    QString question;
    if (unusedOsteos.empty()) {
        qDebug() << "Empty quest array";
        onFinishOsteoAsk();
        dbg_spacing -= 3;
        return;
    }
    int idx = rand(0, unusedOsteos.size() - 1);
    QDomElement pEl = unusedOsteos[idx];
    unusedOsteos.erase(unusedOsteos.begin() + idx);

    QVector<QPair<int, QString>> pixMarks;
    qDebug() << "Main structure: " << elName(pEl);
    parsePixMarks(pixMarks, pEl.attribute("pixMarks"), true);
    int pixIdx = rand(0, pixMarks.size() - 1);
    int pix = pixMarks[pixIdx].first;
    QString mark = findMark(pixMarks, pix);
    QDomElement rEl, lEl, parEl = pEl.parentNode().toElement();
    QDomNodeList nodeList = parEl.childNodes();
    QVector<int> av;
    upn(i, 0, nodeList.size() - 1) {
        av.push_back(i);
    }
    int prob = rand(1, 100);

    if (pEl.tagName() == "canalis") {
        if (prob <= 60) {
            rightAns = elName(pEl);
            ans.insert(rightAns);
            while (ans.size() < maxAns && !av.empty()) {
                idx = rand(0, av.size() - 1);
                QDomElement curEl = nodeList.at(av[idx]).cloneNode().toElement();
                av.erase(av.begin() + idx);
                if (prob <= 20 && pEl.childNodes().at(0).toElement().text() == curEl.childNodes().at(0).toElement().text()) {
                    continue;
                }
                if (prob > 20 && prob <= 40 && pEl.childNodes().at(1).toElement().text() == curEl.childNodes().at(1).toElement().text()) {
                    continue;
                }
                ans.insert(elName(curEl));
            }
            if (m_bLangRu) {
                if (prob <= 20) {                       /* ask about canalis by begin */
                    question = "Какой канал начинается " + parseLinks(pEl.childNodes().at(0).toElement().text());
                } else if (prob > 20 && prob <= 40) {   /* ask about canalis by end */
                    question = "Какой канал заканчивается " + parseLinks(pEl.childNodes().at(1).toElement().text());
                } else if (prob > 40 && prob <= 60) {   /* ask about canalis by path */
                    question = "О каком канале идёт речь: " + parseLinks(pEl.childNodes().at(2).toElement().text());
                }
                if (mark != "0") {
                    question += " (номер " + mark + ")?";
                }
            } else {
                if (prob <= 20) {                       /* ask about canalis by begin */
                    question = "What canalis begins in " + parseLinks(pEl.childNodes().at(0).toElement().text()) + "?";
                } else if (prob > 20 && prob <= 40) {   /* ask about canalis by end */
                    question = "What canalis ends in " + parseLinks(pEl.childNodes().at(1).toElement().text()) + "?";
                } else if (prob > 40 && prob <= 60) {   /* ask about canalis by path */
                    question = "What canalis is described: " + parseLinks(pEl.childNodes().at(2).toElement().text()) + "?";
                }
                if (mark != "0") {
                    question += " (number " + mark + ")?";
                }
            }
        } else if (prob > 60 && prob <= 80) {   /* ask about begin by canalis */
            rightAns = parseLinks(pEl.childNodes().at(0).toElement().text());
            ans.insert(rightAns);
            while (ans.size() < maxAns && !av.empty()) {
                idx = rand(0, av.size() - 1);
                QDomElement curEl = nodeList.at(av[idx]).cloneNode().toElement();
                av.erase(av.begin() + idx);
                QString curBegin = parseLinks(curEl.childNodes().at(0).toElement().text());
                QString curEnd = parseLinks(curEl.childNodes().at(1).toElement().text());
                ans.insert(curBegin);
                if (ans.size() < maxAns)
                    ans.insert(curEnd);
            }
            if (m_bLangRu) {
                question = "Где начинается " + elName(pEl) + " (номер " + mark + ")?";
            } else {
                question = "Where does " + elName(pEl) + " begin (number " + mark + ")?";
            }
        } else if (prob > 80) {                 /* ask about end by canalis */
            rightAns = parseLinks(pEl.childNodes().at(1).toElement().text());
            ans.insert(rightAns);
            while (ans.size() < maxAns && !av.empty()) {
                idx = rand(0, av.size() - 1);
                QDomElement curEl = nodeList.at(av[idx]).cloneNode().toElement();
                av.erase(av.begin() + idx);
                QString curBegin = parseLinks(curEl.childNodes().at(0).toElement().text());
                QString curEnd = parseLinks(curEl.childNodes().at(1).toElement().text());
                ans.insert(curBegin);
                if (ans.size() < maxAns)
                    ans.insert(curEnd);
            }
            if (m_bLangRu) {
                question = "Где заканчивается " + elName(pEl) + " (номер " + mark + ")?";
            } else {
                question = "Where does " + elName(pEl) + "end (number " + mark + ")?";
            }
        }
    } else if (pEl.tagName() == "cell") {
        if (prob <= 50) {                       /* ask about mark by struct */
            rightAns = mark;
            ans.insert(rightAns);
            while (ans.size() < maxAns && !av.empty()) {
                idx = rand(0, av.size() - 1);
                QDomElement curEl = nodeList.at(av[idx]).cloneNode().toElement();
                parsePixMarks(pixMarks, curEl.attribute("pixMarks"), true);
                QString checkMark = findMark(pixMarks, pix);
                if (checkMark != "-1")
                    ans.insert(checkMark);

                av.erase(av.begin() + idx);
            }
            if (m_bLangRu) {
                question = "Каким номером на картинке отмечен(а/о) " + elName(pEl) + " (структура: " + elName(parEl) + ") ?";
            } else {
                question = "What number is " + elName(pEl) + " (structure: " + elName(parEl) + ") marked with ?";
            }
        } else if (prob > 50) {                 /* ask about struct by mark */
            rightAns = elName(pEl);
            ans.insert(rightAns);
            while (ans.size() < maxAns && !av.empty()) {
                idx = rand(0, av.size() - 1);
                QDomElement curEl = nodeList.at(av[idx]).cloneNode().toElement();
                ans.insert(elName(curEl));

                av.erase(av.begin() + idx);
            }
            if (m_bLangRu) {
                question = "Какое образование (структура: " + elName(parEl) + ") помечено на картинке номером " + mark + "?";
            } else {
                question = "What formation (structure: " + elName(parEl) + ") is marked on picture with number " + mark + "?";
            }
        }
    }

    if (ans.size() < 2) {
        qDebug() << "Only one answer generated...";
        genOsteoQuest();
        return;
    }
    /* set question label */
    m_pLblQuestion->setText(question);

    /* set picture */
    m_pGraphicsView->setPix(QPixmap(":/osteoPix/osteoPix" + to_str(pix) + ".png"));

    /* set answer buttons */
    int i = 0;
    for (auto &it : ans) {
        m_pLblAns[i]->setText(it);
        if (it == rightAns) {
            m_pBtnRight = m_pBtnAns[i];
        }
        m_pBtnAns[i]->show();
        ++i;
    }
    upn(j, i, maxAns - 1) {
        m_pBtnAns[j]->hide();
    }
    _dbg_end(__func__);
}
void AnatomyAsker::parsePixMarks(QVector<QPair<int, QString>>& pixVect, QString pixStr, bool clear) {
    if (clear)
        pixVect.clear();
    int i = 0;
    while (i < pixStr.length()) {
        QString num1, num2;
        while (i < pixStr.length() && !isDigit(pixStr[i])) {
            ++i;
        }
        while (i < pixStr.length() && isDigit(pixStr[i])) {
            num1.push_back(pixStr[i]);
            ++i;
        }
        while (i < pixStr.length() && !isDigit(pixStr[i]) && !isUpper(pixStr[i])) {
            ++i;
        }
        while (i < pixStr.length() && (isDigit(pixStr[i]) || isUpper(pixStr[i]))) {
            num2.push_back(pixStr[i]);
            ++i;
        }
        if (num1 == "" || num2 == "") {
            qDebug() << "parsePixMarks: null pix (" + num1 + ") or mark (" + num2 + ")";
            num1 = num2 = "0";
        }
        pixVect.push_back({num1.toInt(), num2});
    }
    if (pixVect.empty()) {
        pixVect.push_back({0, "0"});
    }
}
void AnatomyAsker::setUpObjects() {
    /* GENERAL */
    m_pLayoutMain = new QVBoxLayout;
    m_pLayoutMenu = new QVBoxLayout;
    m_pLayoutPreAsk = new QVBoxLayout;
    m_pLayoutAsk = new QVBoxLayout;
    m_pLayoutMore = new QVBoxLayout;
    m_pWidgetMenu = new QWidget;
    m_pWidgetPreAsk = new QWidget;
    m_pWidgetAsk = new QWidget;
    m_pWidgetMore = new QWidget;

    /* MENU */
    upn(i, 0, 4) {
        m_pBtnMenu[i] = new QPushButton;
        m_pBtnMenu[i]->setStyleSheet("text-align: center; font-size: 23px; "
                                    "color: #000000; background-color: rgba(255,255,255,100)");
        m_pLayoutMenu->addWidget(m_pBtnMenu[i]);
    }
    connect(m_pBtnMenu[0], SIGNAL(clicked(bool)), this, SLOT(onPreStartOsteoAsk()));
    connect(m_pBtnMenu[3], SIGNAL(clicked(bool)), this, SLOT(onSettings()));
    connect(m_pBtnMenu[4], SIGNAL(clicked(bool)), qApp, SLOT(quit()));

    /* SETTINGS */
    m_pCheckRus = new QCheckBox;
    m_pCheckLatin = new QCheckBox;

    m_pDialogSettings = new QDialog;
    QVBoxLayout* pvbox = new QVBoxLayout;
    QPushButton* pBtnOk = new QPushButton("OK");
    m_pDialogSettings->setWindowOpacity(0.9);
    m_pDialogSettings->setMinimumWidth(QGuiApplication::primaryScreen()->geometry().width() * 0.75);
    m_pCheckRus->setTristate(false);
    m_pCheckLatin->setTristate(false);
    connect(pBtnOk, SIGNAL(clicked(bool)), m_pDialogSettings, SLOT(accept()));
    pvbox->addWidget(m_pCheckRus);
    pvbox->addWidget(m_pCheckLatin);
    pvbox->addWidget(pBtnOk);
    m_pDialogSettings->setModal(true);
    m_pDialogSettings->setLayout(pvbox);

    /* PREASK */
    m_pTreeOsteo = viewOsteoTree();
    m_pTreeOsteo->setCurrentItem(m_pTreeOsteo->topLevelItem(0));
    m_pTreeOsteo->expandItem(m_pTreeOsteo->currentItem());
    connect(m_pTreeOsteo, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(onTreeCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));

    m_pBtnMore = new QPushButton;
    m_pBtnStart = new QPushButton;
    QHBoxLayout* pHbox = new QHBoxLayout;
    m_pBtnMore->setEnabled(false);
    pHbox->addWidget(m_pBtnMore);
    pHbox->addWidget(m_pBtnStart);
    m_pLayoutPreAsk->addWidget(m_pTreeOsteo);
    m_pLayoutPreAsk->addLayout(pHbox);
    connect(m_pBtnMore, SIGNAL(clicked(bool)), this, SLOT(onMore()));

    /* MORE */
    m_pGraphicsViewMore = new GraphicsView;
    m_pLblMore = new QLabel;
    m_pBtnBack = new QPushButton;
    m_pBtnNextPix = new QPushButton;
    QHBoxLayout* pHbox2 = new QHBoxLayout;

    m_pLblMore->setMinimumWidth(QGuiApplication::primaryScreen()->geometry().width() * 0.75);
    m_pLblMore->setWordWrap(true);
    m_pLblMore->setAlignment(Qt::AlignCenter);
    pHbox2->addWidget(m_pBtnBack);
    pHbox2->addWidget(m_pBtnNextPix);
    m_pLayoutMore->addWidget(m_pLblMore);
    m_pLayoutMore->addWidget(m_pGraphicsViewMore);
    m_pLayoutMore->addLayout(pHbox2);

    connect(m_pBtnNextPix, SIGNAL(clicked(bool)), this, SLOT(onMoreNextPix()));
    connect(m_pBtnBack, SIGNAL(clicked(bool)), this, SLOT(onPreStartOsteoAsk()));

    /* ASK */
    m_pGraphicsView = new GraphicsView;
    m_pLblQuestion = new QLabel;
    m_pLblInfo = new QLabel;
    m_pBtnNext = new QPushButton;
    m_pBtnFinish = new QPushButton;
    m_pBtnNext->setStyleSheet("color:white; background-color: rgba(0,0,255,175); min-height: 40px");
    m_pBtnFinish->setStyleSheet("color:white; background-color: rgba(0,0,0,175); min-height: 40px");
    upn(i, 0, maxAns - 1) {
        m_pLblAns[i] = new QLabel;
        m_pBtnAns[i] = setUpBtn(m_pLblAns[i]);
        connect(m_pBtnAns[i], SIGNAL(clicked(bool)), this, SLOT(onAns()));
    }
    m_pLblQuestion->setWordWrap(true);
    m_pLblQuestion->setMinimumWidth(QGuiApplication::primaryScreen()->geometry().width() * 0.8);
    m_pLblQuestion->setAlignment(Qt::AlignCenter);
    m_pLblQuestion->setStyleSheet("font-size: 23px; font-weight:bold;"
                                  "color: #001a00");
    m_pLblInfo->setWordWrap(true);
    m_pLblInfo->setAlignment(Qt::AlignCenter);
    m_pLblInfo->setStyleSheet("font-size: 20px; color: #990099");
    m_pLblInfo->setMinimumWidth(QGuiApplication::primaryScreen()->geometry().width() * 0.15);

    QGridLayout* pGridLayout = new QGridLayout;
    QHBoxLayout* pHLayout = new QHBoxLayout;
    upn(i, 0, maxAns - 1) {
        pGridLayout->addWidget(m_pBtnAns[i], i / 2, i % 2);
    }
    updateInfoLabel();

    pHLayout->addWidget(m_pLblQuestion);
    pHLayout->addWidget(m_pLblInfo);
    pGridLayout->addWidget(m_pBtnFinish, maxAns / 2, 0);
    pGridLayout->addWidget(m_pBtnNext, maxAns / 2, 1);

    m_pLayoutAsk->addLayout(pHLayout);
    m_pLayoutAsk->addWidget(m_pGraphicsView);
    m_pLayoutAsk->addLayout(pGridLayout);
}
void AnatomyAsker::sortOsteoXml() {
    _dbg_start(__func__);
    QDomElement rootEl = osteoDoc.documentElement();
    sortOsteoXmlDfs(rootEl);
    _dbg_end(__func__);
}
void AnatomyAsker::sortOsteoXmlDfs(QDomElement& parEl) {
    QDomNodeList nodeList = parEl.childNodes();
    QMap<QString, QDomNode> map;

    while (!nodeList.isEmpty()) {
        map[nodeList.at(0).toElement().attribute("name")] = nodeList.at(0).cloneNode();
        parEl.removeChild(nodeList.at(0));
    }
    for (auto it : map) {
        QDomElement curEl = it.toElement();
        parEl.appendChild(curEl);
        if (curEl.tagName() != "canalis")
            sortOsteoXmlDfs(curEl);
    }
}
void AnatomyAsker::processOsteoXml() {
    _dbg_start(__func__);
    QDomElement rootEl = osteoDoc.documentElement();
    processOsteoXmlDfs(rootEl);
    _dbg_end(__func__);
}
void AnatomyAsker::processOsteoXmlDfs(QDomElement& parEl) {
    findElementByName[parEl.attribute("name")] = parEl;
    if (parEl.hasAttribute("pixMarks")) {
        QVector<QPair<int, QString>> pixMarks;
        parsePixMarks(pixMarks, parEl.attribute("pixMarks"), true);
        upn(i, 0, pixMarks.size() - 1) {
            int pixN = pixMarks[i].first;
            if (findNamesByPix.contains(pixN)) {
                findNamesByPix[pixN].push_back(parEl.attribute("name"));
            } else {
                QVector<QString> tmp;
                tmp.push_back(parEl.attribute("name"));
                findNamesByPix[pixN] = tmp;
            }
        }
    }
    if (parEl.tagName() == "cell") {
        if (parEl.hasAttribute("name") && parEl.hasAttribute("pixMarks")) {
            unusedOsteos.push_back(parEl);
        }
    } else if (parEl.tagName() == "canalis") {
        unusedOsteos.push_back(parEl);
    }

    QDomNode curN = parEl.firstChild();
    while (!curN.isNull()) {
        QDomElement curEl = curN.toElement();
        processOsteoXmlDfs(curEl);
        curN = curN.nextSibling();
    }
}
void AnatomyAsker::readXml(QDomDocument& doc, QString path) {
    _dbg_start(__func__);
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        QString error_msg;
        if (!doc.setContent(&file, true, &error_msg)) {
            crash("Error opening xml: " + error_msg);
        }
    } else {
        crash("File " + file.fileName() + " could not be opened in read-only mode");
    }
    file.close();
    _dbg_end(__func__);
}
void AnatomyAsker::viewOsteoTreeDfs(QDomElement& parEl, QTreeWidgetItem* pTWIPar) {
    QDomElement curEl = parEl.firstChildElement();
    while (!curEl.isNull()) {
        QTreeWidgetItem* pTWI = new QTreeWidgetItem;
        pTWI->setText(0, elName(curEl));
        pTWI->setText(1, curEl.attribute("name"));
        pTWIPar->addChild(pTWI);
        if (curEl.tagName() != "canalis")
            viewOsteoTreeDfs(curEl, pTWI);
        curEl = curEl.nextSiblingElement();
    }
}
void AnatomyAsker::updateInfoLabel() {
    m_pLblInfo->setText((m_bLangRu ? "Всего: " : "Summary: ") + to_str(q_cnt) + "/" +
                        to_str(q_sum) + (m_bLangRu ? "\nВерно: " : "\nCorrect: ") +
                        to_str(q_rightAnsCnt) + "/" + to_str(q_cnt));
}
void AnatomyAsker::writeXml(QDomDocument& doc, QString path) {
    _dbg_start(__func__);
    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toByteArray());
    } else {
        crash("File " + file.fileName() + " could not be opened in write-only mode");
    }
    file.close();
    _dbg_end(__func__);
}

/* PUBLIC FUNCTIONS */
AnatomyAsker::AnatomyAsker(QStackedWidget *pswgt) : QStackedWidget(pswgt), m_settings("nikich340", "AnatomyAsker")
{
    _dbg_start(__func__);

    m_bLangRu = m_settings.value("/settings/m_bLangRu", false).toBool();
    m_bLatin = m_settings.value("/settings/m_bLatin", true).toBool();

    readXml(osteoDoc, ":/osteologia.xml");
    processOsteoXml();

    this->setStyleSheet("QPushButton { alignment: center; text-align: center; min-height: 75px; font-size: 20px; background-color: rgba(255,255,255,150) }"
                        "QLabel { alignment: center; text-align: center; font-size: 20px; background-color: rgba(255,255,255,100) }"
                        "QTreeWidget { alignment: center; text-align: center; font-size: 23px; background-color: rgba(255,255,255,200) }"
                        "GraphicsView { background-color: rgba(255, 255, 255, 64) }");
    setUpObjects();

    connect(m_pCheckRus, SIGNAL(stateChanged(int)), this, SLOT(onUpdateLanguage(int)));

    m_pWidgetMenu->setLayout(m_pLayoutMenu);
    m_pWidgetPreAsk->setLayout(m_pLayoutPreAsk);
    m_pWidgetAsk->setLayout(m_pLayoutAsk);
    m_pWidgetMore->setLayout(m_pLayoutMore);

    this->addWidget(m_pWidgetMenu);
    this->addWidget(m_pWidgetPreAsk);
    this->addWidget(m_pWidgetAsk);
    this->addWidget(m_pWidgetMore);
    this->layout()->setSizeConstraint(QLayout::SetMaximumSize);

    onUpdateLanguage(m_bLangRu ? Qt::Checked : Qt::Unchecked);
    updateInfoLabel();
    onMenu();
    _dbg_end(__func__);
}
AnatomyAsker::~AnatomyAsker() {
    _dbg_start(__func__);
    m_settings.setValue("/settings/m_bLangRu", m_bLangRu);
    m_settings.setValue("/settings/m_bLatin", m_bLatin);
    _dbg_end(__func__);
}

/* PUBLIC SLOTS */
void AnatomyAsker::onAns() {
    _dbg_start(__func__);
    QPushButton* pBtn = dynamic_cast<QPushButton*>(sender());
    if (m_pBtnRight == pBtn) {
        if (!q_ansType) {
            q_ansType = 1;
            ++q_rightAnsCnt;
        }
        pBtn->setStyleSheet("color:blue; background-color: rgba(50,255,50,100); font-weight:bold");
    } else {
        if (!q_ansType) {
            q_ansType = -1;
        }
        pBtn->setStyleSheet("color:white; background-color: rgba(255,0,0,100)");
    }
    updateInfoLabel();
}
void AnatomyAsker::onFinishAsk() {
    _dbg_start(__func__);
    qreal score = (qreal) q_rightAnsCnt / (qreal) q_cnt;
    QDialog* pdlg;
    QMediaPlayer* pPlayer = new QMediaPlayer;
    if (q_cnt < 5) {
        pdlg = createDialog((m_bLangRu ? "Ваш результат: " : "Your result is: ")
                            + to_str(q_rightAnsCnt) + "/" + to_str(q_cnt)
                            + (m_bLangRu ? "\nОтветьте на 5 и более вопросов и получите оценку! :)" :
                                           "\nAnswer on 5 and more questions and get score! :)"), "-",
                            m_bLangRu ? "Меню" : "Menu", m_bLangRu ? "Выход" : "Quit", true);
    } else {
        QString pix;
        if (score >= 0.9) {
            pix = ":/score/5.jpg";
            pPlayer->setMedia(QUrl("qrc:/score/5.mp3"));
        } else if (score >= 0.7) {
            pix = ":/score/4.jpg";
            pPlayer->setMedia(QUrl("qrc:/score/4.mp3"));
        } else if (score >= 0.5) {
            pix = ":/score/3.jpg";
            pPlayer->setMedia(QUrl("qrc:/score/3.mp3"));
        } else {
            pix = ":/score/2.jpg";
            pPlayer->setMedia(QUrl("qrc:/score/2.mp3"));
        }
        pdlg = createDialog((m_bLangRu ? "Ваш результат: " : "Your result is: ")
                            + to_str(q_rightAnsCnt) + "/" + to_str(q_cnt), pix,
                            m_bLangRu ? "Меню" : "Menu", m_bLangRu ? "Выход" : "Quit", true);
    }

    connect(pdlg, SIGNAL(accepted()), this, SLOT(onMenu()));
    connect(pdlg, SIGNAL(rejected()), qApp, SLOT(quit()));
    if (!pPlayer->media().isNull()) {
        pPlayer->play();
    }
    pdlg->exec();
    pdlg->deleteLater();
    pPlayer->stop();
    pPlayer->deleteLater();

    upn(i, 0, maxAns - 1) {
        m_pBtnAns[i]->setStyleSheet("background-color: rgba(255,255,255,70)");
    }
    q_sum = q_rightAnsCnt = q_ansType = 0;
    q_cnt = 1;
    _dbg_end(__func__);
}
void AnatomyAsker::onFinishOsteoAsk() {
    _dbg_start(__func__);
    disconnect(m_pBtnStart, SIGNAL(clicked(bool)), this, SLOT(onStartOsteoAsk()));
    disconnect(m_pBtnFinish, SIGNAL(clicked(bool)), this, SLOT(onFinishOsteoAsk()));
    disconnect(m_pBtnNext, SIGNAL(clicked(bool)), this, SLOT(onNextOsteoAsk()));
    onFinishAsk();
    _dbg_end(__func__);
}
void AnatomyAsker::onMenu() {
    _dbg_start(__func__);

    setCurrentWidget(m_pWidgetMenu);

    _dbg_end(__func__);
}
void AnatomyAsker::onMore() {
    _dbg_start(__func__);
    QDomElement curEl = findElementByName[m_pTreeOsteo->currentItem()->text(1)];
    QDomElement parEl = curEl.parentNode().toElement();

    morePixVect.clear();
    morePixNum = 0;
    if (curEl.hasAttribute("mainPix")) {
        morePixVect.push_back({curEl.attribute("mainPix").toInt(), "..."});
    }
    parsePixMarks(morePixVect, curEl.attribute("pixMarks"), false);
    m_pGraphicsViewMore->setPix(QPixmap(":/osteoPix/osteoPix" + to_str(morePixVect[0].first) + ".png"));
    m_pBtnNextPix->setEnabled(morePixVect.size() > 1);
    moreText = "<h4><span style=\"color: #0000ff;\">" + elName(curEl) +
            "<span style=\"color: #ff00ff;\"> (" + elName(parEl) + ")<br /></span></span>";
    if (curEl.tagName() == "canalis") {
        if (m_bLangRu) {
            moreText += "<strong><span style=\"color: #800080;\">Начало:</span></strong> " + parseLinks(curEl.childNodes().at(0).toElement().text()) +
                    "<br /><strong><span style=\"color: #800080;\">Конец:</span></strong> " + parseLinks(curEl.childNodes().at(1).toElement().text()) +
                    "<br /><strong><span style=\"color: #800080;\">Описание:</span></strong> " + parseLinks(curEl.childNodes().at(2).toElement().text());
        } else {
            moreText += "<strong><span style=\"color: #800080;\">Begin:</span></strong> " + parseLinks(curEl.childNodes().at(0).toElement().text()) +
                    "<br /><strong><span style=\"color: #800080;\">End:</span></strong> " + parseLinks(curEl.childNodes().at(1).toElement().text()) +
                    "<br /><strong><span style=\"color: #800080;\">Description:</span></strong> " + parseLinks(curEl.childNodes().at(2).toElement().text());
        }
    }
    if (morePixVect[0].second != "0") {
        if (m_bLangRu) {
            m_pLblMore->setText(moreText + "<br /><strong><span style=\"color: #e60000;\">Номер " + morePixVect[0].second + " на текущей картинке</span></h4>");
        } else {
            m_pLblMore->setText(moreText + "<br /><strong><span style=\"color: #e60000;\">Mark " + morePixVect[0].second + " on current picture</span></h4>");
        }
    } else {
        m_pLblMore->setText(moreText);
    }

    setCurrentWidget(m_pWidgetMore);

    _dbg_end(__func__);
}
void AnatomyAsker::onMoreNextPix() {
    if (morePixVect.size() == 1 && morePixVect[0].second == "0") {
        return;
    }
    ++morePixNum;
    if (morePixNum > morePixVect.size() - 1) {
        morePixNum = 0;
    }
    m_pGraphicsViewMore->setPix(QPixmap(":/osteoPix/osteoPix" + to_str(morePixVect[morePixNum].first) + ".png"));
    if (m_bLangRu) {
        m_pLblMore->setText(moreText + "<br /><strong><span style=\"color: #e60000;\">Номер " + morePixVect[morePixNum].second + " на текущей картинке</span></h4>");
    } else {
        m_pLblMore->setText(moreText + "<br /><strong><span style=\"color: #e60000;\">Mark " + morePixVect[morePixNum].second + " on current picture</span></h4>");
    }
}
void AnatomyAsker::onNextOsteoAsk() {
    _dbg_start(__func__);
    q_ansType = 0;
    upn(i, 0, maxAns - 1) {
        m_pBtnAns[i]->setStyleSheet("background-color: rgba(255,255,255,70)");
    }
    genOsteoQuest();
    ++q_cnt;
    updateInfoLabel();
    _dbg_end(__func__);
}
void AnatomyAsker::onPreStartOsteoAsk() {
    _dbg_start(__func__);
    if (!m_settings.value("/settings/launched" + to_str(VERSION), false).toBool()) {
        QDialog* pdlg = createDialog("Необходимо перезайти в приложение (первый запуск)", "-", "OK", "-", true);
        connect(pdlg, SIGNAL(accepted()), qApp, SLOT(quit()));
        m_settings.setValue("/settings/launched" + to_str(VERSION), true);
        pdlg->exec();
        pdlg->deleteLater();
    }

    setCurrentWidget(m_pWidgetPreAsk);

    unusedOsteos.clear();
    processOsteoXml();
    //sortOsteoXml();
    //writeXml(osteoDoc, "osteosort.xml");

    connect(m_pBtnStart, SIGNAL(clicked(bool)), this, SLOT(onStartOsteoAsk()));
    _dbg_end(__func__);
}
void AnatomyAsker::onSettings() {
    _dbg_start(__func__);
    m_pCheckRus->setCheckState(m_bLangRu ? Qt::Checked : Qt::Unchecked);
    m_pCheckLatin->setCheckState(m_bLatin ? Qt::Checked : Qt::Unchecked);

    m_pDialogSettings->exec();
    _dbg_end(__func__);
}
void AnatomyAsker::onStartAsk() {
    _dbg_start(__func__);

    updateInfoLabel();
    setCurrentWidget(m_pWidgetAsk);

    _dbg_end(__func__);
}
void AnatomyAsker::onStartOsteoAsk() {
    _dbg_start(__func__);

    chooseOsteoQuests(m_pTreeOsteo->currentItem()->text(0));
    q_sum = unusedOsteos.size();
    genOsteoQuest();

    connect(m_pBtnFinish, SIGNAL(clicked(bool)), this, SLOT(onFinishOsteoAsk()));
    connect(m_pBtnNext, SIGNAL(clicked(bool)), this, SLOT(onNextOsteoAsk()));
    _dbg_end(__func__);
    onStartAsk();
}
void AnatomyAsker::onTreeCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    QDomElement curEl = findElementByName[current->text(1)];
    if (curEl.tagName() == "canalis" || (curEl.hasAttribute("pixMarks") || curEl.hasAttribute("mainPix"))) {
        m_pBtnMore->setEnabled(true);
    } else {
        m_pBtnMore->setEnabled(false);
    }
}
void AnatomyAsker::onUpdateLanguage(int check) {
    _dbg_start(__func__);
    if (check == Qt::Checked) {
        m_bLangRu = true;
    } else {
        m_bLangRu = false;
    }
    m_pCheckLatin->setCheckable(m_bLangRu);
    m_pBtnBack->setText(m_bLangRu ? "Назад" : "Back");
    m_pBtnMenu[0]->setText(m_bLangRu ? "Остеология" : "Osteologia");
    m_pBtnMenu[1]->setText(m_bLangRu ? "Артрология" : "Artrologia");
    m_pBtnMenu[2]->setText(m_bLangRu ? "Миология" : "Myologia");
    m_pBtnMenu[3]->setText(m_bLangRu ? "Настройки" : "Settings");
    m_pBtnMenu[4]->setText(m_bLangRu ? "Выход" : "Quit");
    m_pBtnStart->setText(m_bLangRu ? "Начать" : "Start");
    m_pBtnMore->setText(m_bLangRu ? "Подробнее" : "More");
    m_pBtnNext->setText(m_bLangRu ? "Далее" : "Next");
    m_pBtnNextPix->setText(m_bLangRu ? "Следующая картинка" : "Next picture");
    m_pBtnFinish->setText(m_bLangRu ? " Завершить" : " Finish");
    m_pCheckRus->setText(m_bLangRu ? "Русский язык" : "Russian language");
    m_pCheckLatin->setText(m_bLangRu ? "Добавлять латинские названия" : "Add latin name");
    _dbg_end(__func__);
}
