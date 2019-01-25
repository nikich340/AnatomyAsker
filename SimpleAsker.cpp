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

#include "SimpleAsker.h"
#include <QMediaPlayer>

/* PRIVATE FUNCTIONS */
QDialog* SimpleAsker::createDialog(QString info, QString pix, QString accept, QString reject, bool mod) {
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
QPushButton* SimpleAsker::setUpBtn(QLabel* pLbl) {
    QPushButton* pBtn = new QPushButton;
    QHBoxLayout *pLayout = new QHBoxLayout;
    pLbl->setWordWrap(true);
    pLbl->setAlignment(Qt::AlignCenter);
    pLayout->addWidget(pLbl);
    pBtn->setLayout(pLayout);
    pBtn->setStyleSheet("background-color: rgba(255,255,255,70)");
    return pBtn;
}
bool SimpleAsker::isDigit(QChar c) {
    return (c >= '0' && c <= '9');
}
bool SimpleAsker::isSymbol(QChar c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}
bool SimpleAsker::isUpper(QChar c) {
    return (c >= 'A' && c <= 'Z');
}
int SimpleAsker::rand(int L, int R) {
    qsrand(QDateTime::currentMSecsSinceEpoch());
    if (R < L)
        return -1;
    return L + qrand() % (R - L + 1);
}
void SimpleAsker::_dbg_start(QString func) {
    QString out = "";
    upn(i, 1, dbg_spacing) {
        out.push_back(' ');
    }
    out += func;
    dbg_spacing += 3;
    qDebug() << out << "{";
}
void SimpleAsker::_dbg_end(QString func) {
    QString out = "";
    dbg_spacing -= 3;
    upn(i, 1, dbg_spacing) {
        out.push_back(' ');
    }
    out += func;
    qDebug() << "}" << out;
}
void SimpleAsker::clearLayout(QLayout* layout) {
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
void SimpleAsker::crash(QString reason) {
    qDebug() << "Crash! Reason: " << reason << "\n";
    QDialog* pdlg = createDialog("Crash! Reason: " + reason + "\n", "-", "Quit", "Try ignore", true);
    connect(pdlg, SIGNAL(accepted()), qApp, SLOT(quit()));
    pdlg->exec();
    pdlg->deleteLater();
}
void SimpleAsker::genOsteoQuest() {
    _dbg_start(__func__);
    std::default_random_engine dre(QDateTime::currentMSecsSinceEpoch());
    if (q_unusedAsks.empty()) {
        qDebug() << "Empty quest array";
        onFinishOsteoAsk();
        dbg_spacing -= 3;
        return;
    }

    int idx = 0;
    if (m_pCheckRandomize->checkState() == Qt::Checked) {
        idx = rand(0, q_unusedAsks.size() - 1);
    }
    qDebug() << "idx " << idx;
    qDebug() << "size " << q_unusedAsks[idx].answers.size();
    for (int i = 0; i < q_unusedAsks[idx].answers.size(); ++i) {
        qDebug() << q_unusedAsks[idx].answers[i];
    }
    qDebug() << "right " << q_unusedAsks[idx].rightAns;
    std::shuffle(q_unusedAsks[idx].answers.begin(), q_unusedAsks[idx].answers.end(), dre);

    /* set question label */
    m_pLblQuestion->setText(q_unusedAsks[idx].question);

    /* set answer buttons */
    upn(i, 0, q_unusedAsks[idx].answers.size() - 1) {
        m_pBtnAns[i]->show();
        m_pLblAns[i]->setText(q_unusedAsks[idx].answers[i]);
        if (q_unusedAsks[idx].rightAns == q_unusedAsks[idx].answers[i]) {
            m_pBtnRight = m_pBtnAns[i];
        }
    }
    upn(j, q_unusedAsks[idx].answers.size(), maxAns - 1) {
        m_pBtnAns[j]->hide();
    }
    q_unusedAsks.erase(q_unusedAsks.begin() + idx);
    _dbg_end(__func__);
}
void SimpleAsker::setUpObjects() {
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
    upn(i, 0, 3) {
        m_pBtnMenu[i] = new QPushButton;
        m_pBtnMenu[i]->setStyleSheet("text-align: center; font-size: 23px; "
                                    "color: #000000; background-color: rgba(255,255,255,100)");
        m_pLayoutMenu->addWidget(m_pBtnMenu[i]);
    }
    connect(m_pBtnMenu[0], SIGNAL(clicked(bool)), this, SLOT(onPreStartOsteoAsk()));
    connect(m_pBtnMenu[1], SIGNAL(clicked(bool)), this, SLOT(onSettings()));
    connect(m_pBtnMenu[2], SIGNAL(clicked(bool)), this, SLOT(onAboutProgram()));
    connect(m_pBtnMenu[3], SIGNAL(clicked(bool)), qApp, SLOT(quit()));

    /* SETTINGS */
    m_pCheckRus = new QCheckBox;

    m_pDialogSettings = new QDialog;
    QVBoxLayout* pvbox = new QVBoxLayout;
    QPushButton* pBtnOk = new QPushButton("OK");
    m_pDialogSettings->setWindowOpacity(0.9);
    m_pDialogSettings->setMinimumWidth(QGuiApplication::primaryScreen()->geometry().width() * 0.75);
    m_pCheckRus->setTristate(false);
    connect(pBtnOk, SIGNAL(clicked(bool)), m_pDialogSettings, SLOT(accept()));
    pvbox->addWidget(m_pCheckRus);
    pvbox->addWidget(pBtnOk);
    m_pDialogSettings->setModal(true);
    m_pDialogSettings->setLayout(pvbox);

    /* PREASK */
    m_pLblStartFrom = new QLabel;
    m_pLineStartFrom = new QLineEdit("1");
    QHBoxLayout *pHboxx = new QHBoxLayout;
    pHboxx->addWidget(m_pLblStartFrom);
    pHboxx->addWidget(m_pLineStartFrom);
    m_pLineStartFrom->setValidator(new QIntValidator(1, 299));
    m_pBtnStart = new QPushButton;
    m_pCheckRandomize = new QCheckBox;
    m_pLayoutPreAsk->addLayout(pHboxx);
    m_pLayoutPreAsk->addWidget(m_pCheckRandomize);
    m_pLayoutPreAsk->addWidget(m_pBtnStart);

    /* ASK */
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
    m_pLayoutAsk->addLayout(pGridLayout);
}
void SimpleAsker::readAsks(QString pathQuest, QString pathAns) {
    _dbg_start(__func__);
    QFile in(pathQuest);
    QFile ans(pathAns);
    //out.open(QFile::WriteOnly | QFile::Text);
    if (!in.open(QFile::ReadOnly | QFile::Text)) {
        crash("File " + in.fileName() + " could not be opened in read-only mode");
    }
    if (!ans.open(QFile::ReadOnly | QFile::Text)) {
        crash("File " + ans.fileName() + " could not be opened in read-only mode");
    }
    QString tmp;
    ask tmpAsk;
    int state = -1;
    while (!in.atEnd()) {
        tmp = in.readLine();
        qDebug() << tmp;
        if ((tmp[1] == ')') || (state == 2 && !isDigit(tmp[0]))) {
            tmpAsk.answers.push_back(tmp.mid(3));
            state = 2;
            qDebug() << "! read a " << tmp.mid(3);
        } else {
            if (state == 2) {
                QString right = ans.readLine();
                qDebug() << "right " << right;
                tmpAsk.rightAns = tmpAsk.answers[right.toInt()];
                q_unusedAsks.push_back(tmpAsk);
                tmpAsk.question = "";
                tmpAsk.answers.clear();
            }
            state = 1;
            int pointI = 0;
            if (tmp.count('.') > 0) {
                while (tmp[pointI] != '.' && pointI < tmp.length())
                    ++pointI;
                pointI += 2;
            }
            tmpAsk.question += tmp.mid(pointI);
            qDebug() << "! read q " << tmp.mid(pointI);
        }
    }
    tmpAsk.rightAns = tmpAsk.answers[ans.readLine().toInt()];
    q_unusedAsks.push_back(tmpAsk);
    in.close();
    _dbg_end(__func__);
}
void SimpleAsker::updateInfoLabel() {
    m_pLblInfo->setText((m_bLangRu ? "Всего: " : "Summary: ") + to_str(q_cnt) + "/" +
                        to_str(q_sum) + (m_bLangRu ? "\nВерно: " : "\nCorrect: ") +
                        to_str(q_rightAnsCnt) + "/" + to_str(q_cnt));
}

/* PUBLIC FUNCTIONS */
SimpleAsker::SimpleAsker(QStackedWidget *pswgt) : QStackedWidget(pswgt), m_settings("nikich340", "SimpleAsker")
{
    _dbg_start(__func__);

    m_bLangRu = m_settings.value("/settings/m_bLangRu", true).toBool();

    this->setStyleSheet("QPushButton { alignment: center; text-align: center; min-height: 75px; font-size: 20px; background-color: rgba(255,255,255,150) }"
                        "QLabel { alignment: center; text-align: center; font-size: 20px; background-color: rgba(255,255,255,100) }"
                        "QTreeWidget { alignment: center; text-align: center; font-size: 23px; background-color: rgba(255,255,255,200) }"
                        "QCheckBox { alignment: center; text-align: center; min-height: 75px; font-size: 23px; background-color: rgba(255, 255, 255, 150) }"
                        "QLineEdit { alignment: center; text-align: center; min-height: 75px; font-size: 23px; background-color: rgba(255, 255, 255, 150) }");
    setUpObjects();

    connect(m_pCheckRus, SIGNAL(stateChanged(int)), this, SLOT(onUpdateLanguage(int)));

    m_pWidgetMenu->setLayout(m_pLayoutMenu);
    m_pWidgetPreAsk->setLayout(m_pLayoutPreAsk);
    m_pWidgetAsk->setLayout(m_pLayoutAsk);

    this->addWidget(m_pWidgetMenu);
    this->addWidget(m_pWidgetPreAsk);
    this->addWidget(m_pWidgetAsk);
    this->layout()->setSizeConstraint(QLayout::SetMaximumSize);

    onUpdateLanguage(m_bLangRu ? Qt::Checked : Qt::Unchecked);
    updateInfoLabel();
    onMenu();
    _dbg_end(__func__);
}
SimpleAsker::~SimpleAsker() {
    _dbg_start(__func__);
    m_settings.setValue("/settings/m_bLangRu", m_bLangRu);
    _dbg_end(__func__);
}

/* PUBLIC SLOTS */
void SimpleAsker::onAns() {
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
    _dbg_end(__func__);
}
void SimpleAsker::onAboutProgram() {
    _dbg_start(__func__);
    QString txt = QString("<font>") + (m_bLangRu ? "Версия: " : "Version: ") + VERSION + "<br>" +
            (m_bLangRu ? "Автор: Никита Гребенюк" : "Author: Nikita Grebenyuk") +
            " (@nikich340)<br>" + (m_bLangRu ? "Оригинальный исходный код: " : "Original source code: ") +
            "https://github.com/nikich340/AnatomyAsker/tree/SimpleAsker</font>";
    QDialog *pdlg = createDialog(txt, ":/common/nikich340.jpg", "OK", "-", true);
    pdlg->exec();
    pdlg->deleteLater();
    _dbg_end(__func__);
}
void SimpleAsker::onFinishAsk() {
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
            pix = ":/common/score/5.jpg";
            pPlayer->setMedia(QUrl("qrc:/common/score/5.mp3"));
        } else if (score >= 0.7) {
            pix = ":/common/score/4.jpg";
            pPlayer->setMedia(QUrl("qrc:/common/score/4.mp3"));
        } else if (score >= 0.5) {
            pix = ":/common/score/3.jpg";
            pPlayer->setMedia(QUrl("qrc:/common/score/3.mp3"));
        } else {
            pix = ":/common/score/2.jpg";
            pPlayer->setMedia(QUrl("qrc:/common/score/2.mp3"));
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
void SimpleAsker::onFinishOsteoAsk() {
    _dbg_start(__func__);
    disconnect(m_pBtnStart, SIGNAL(clicked(bool)), this, SLOT(onStartOsteoAsk()));
    disconnect(m_pBtnFinish, SIGNAL(clicked(bool)), this, SLOT(onFinishOsteoAsk()));
    disconnect(m_pBtnNext, SIGNAL(clicked(bool)), this, SLOT(onNextOsteoAsk()));
    onFinishAsk();
    _dbg_end(__func__);
}
void SimpleAsker::onMenu() {
    _dbg_start(__func__);

    setCurrentWidget(m_pWidgetMenu);

    _dbg_end(__func__);
}
void SimpleAsker::onNextOsteoAsk() {
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
void SimpleAsker::onPreStartOsteoAsk() {
    _dbg_start(__func__);

    setCurrentWidget(m_pWidgetPreAsk);

    q_unusedAsks.clear();
    readAsks(":/OOU.txt", ":/OOUanswers.txt");

    connect(m_pBtnStart, SIGNAL(clicked(bool)), this, SLOT(onStartOsteoAsk()));
    _dbg_end(__func__);
}
void SimpleAsker::onSettings() {
    _dbg_start(__func__);
    m_pCheckRus->setCheckState(m_bLangRu ? Qt::Checked : Qt::Unchecked);

    m_pDialogSettings->exec();
    _dbg_end(__func__);
}
void SimpleAsker::onStartAsk() {
    _dbg_start(__func__);

    updateInfoLabel();
    setCurrentWidget(m_pWidgetAsk);

    _dbg_end(__func__);
}
void SimpleAsker::onStartOsteoAsk() {
    _dbg_start(__func__);

    int start = m_pLineStartFrom->text().toInt() - 1;
    upn(i, 0, start - 1) {
        q_unusedAsks.erase(q_unusedAsks.begin());
    }
    q_sum = q_unusedAsks.size();
    genOsteoQuest();

    connect(m_pBtnFinish, SIGNAL(clicked(bool)), this, SLOT(onFinishOsteoAsk()));
    connect(m_pBtnNext, SIGNAL(clicked(bool)), this, SLOT(onNextOsteoAsk()));
    _dbg_end(__func__);
    onStartAsk();
}
void SimpleAsker::onUpdateLanguage(int check) {
    _dbg_start(__func__);
    if (check == Qt::Checked) {
        m_bLangRu = true;
    } else {
        m_bLangRu = false;
    }
    m_pBtnMenu[0]->setText(m_bLangRu ? "Предэкз. тест ООУ" : "Preex. test BGN");
    m_pBtnMenu[1]->setText(m_bLangRu ? "Настройки" : "Settings");
    m_pBtnMenu[2]->setText(m_bLangRu ? "О программе" : "About program");
    m_pBtnMenu[3]->setText(m_bLangRu ? "Выход" : "Quit");
    m_pBtnStart->setText(m_bLangRu ? "Начать" : "Start");
    m_pBtnNext->setText(m_bLangRu ? "Далее" : "Next");
    m_pBtnFinish->setText(m_bLangRu ? " Завершить" : " Finish");
    m_pCheckRus->setText(m_bLangRu ? "Русский язык" : "Russian language");
    m_pCheckRandomize->setText(m_bLangRu ? "Случайный порядок вопросов" : "Random order of questions");
    m_pLblStartFrom->setText(m_bLangRu ? "Начинать с вопроса: " : "Start from question: ");
    _dbg_end(__func__);
}
