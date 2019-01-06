#include "AnatomyAsker.h"

/* PRIVATE FUNCTIONS */
QDialog* AnatomyAsker::createDialog(QString info, QString accept, QString reject, bool mod) {
    QDialog* pdlg = new QDialog(this);
    pdlg->setWindowOpacity(0.9);
    QHBoxLayout* phbox = new QHBoxLayout;
    QVBoxLayout* pvbox = new QVBoxLayout;
    QLabel* plbl = new QLabel(info);
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
   qDebug() << QString(dbg_spacing, (QChar) ' ') << "Begin: " << __func__; dbg_spacing += 3;
   QTreeWidget* pTW = new QTreeWidget;
   pTW->setHeaderLabel(m_bLangRu ? "Выберите структуру для вопросов" : "Choose structure for questions");
   pTW->setColumnCount(1);

   QDomElement rootEl = osteoDoc.documentElement();
   QTreeWidgetItem* pRoot = new QTreeWidgetItem;
   pRoot->setText(0, elName(rootEl));
   pTW->addTopLevelItem(pRoot);
   viewOsteoTreeDfs(rootEl, pRoot);

   dbg_spacing -= 3; qDebug() << QString(dbg_spacing, (QChar) ' ') << "End:   " << __func__;
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
            qDebug() << "Not clear";
            delete item;
        }
    }
}
void AnatomyAsker::chooseOsteoQuests(QString rootPattern) {
    qDebug() << QString(dbg_spacing, (QChar) ' ') << "Begin: " << __func__; dbg_spacing += 3;
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
    dbg_spacing -= 3; qDebug() << QString(dbg_spacing, (QChar) ' ') << "End:   " << __func__;
}
void AnatomyAsker::crash(QString reason) {
    qDebug() << "Crash! Reason: " << reason << "\n";
    QDialog* pdlg = createDialog("Crash! Reason: " + reason + "\n", "Quit", "Try ignore", true);
    connect(pdlg, SIGNAL(accepted()), qApp, SLOT(quit()));
    pdlg->exec();
    pdlg->deleteLater();
}
void AnatomyAsker::genOsteoQuest() {
    qDebug() << QString(dbg_spacing, (QChar) ' ') << "Begin: " << __func__; dbg_spacing += 3;
    std::default_random_engine dre(QDateTime::currentMSecsSinceEpoch());
    QVector<QString> ans;
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
    parsePixMarks(pixMarks, pEl.attribute("pixMarks"));
    int pixIdx = rand(0, pixMarks.size() - 1);
    int pix = pixMarks[pixIdx].first;
    QString mark = findMark(pixMarks, pix);
    QDomElement rEl, lEl, parEl = pEl.parentNode().toElement();
    int type = rand(1, 2);

    if (type == 1) { /* ask about mark by struct */
        rightAns = mark;
        ans.push_back(rightAns);
        rEl = pEl.previousSiblingElement();
        lEl = pEl.nextSiblingElement();
        while (ans.size() < maxAns && (!rEl.isNull() || !lEl.isNull())) {
            if (!rEl.isNull()) {
                parsePixMarks(pixMarks, rEl.attribute("pixMarks"));
                QString checkMark = findMark(pixMarks, pix);
                if (checkMark != "-1" && checkMark != mark)
                    ans.push_back(checkMark);
                rEl = rEl.previousSiblingElement();
            }
            if (!lEl.isNull() && ans.size() < maxAns) {
                parsePixMarks(pixMarks, lEl.attribute("pixMarks"));
                QString checkMark = findMark(pixMarks, pix);
                if (checkMark != "-1" && checkMark != mark)
                    ans.push_back(checkMark);
                lEl = lEl.nextSiblingElement();
            }
        }
        if (m_bLangRu) {
            question = "Каким номером на картинке отмечен(а/о) " + elName(pEl) + " (структура: " + elName(parEl) + ") ?";
        } else {
            question = "What number is " + elName(pEl) + " (structure: " + elName(parEl) + ") marked with ?";
        }
    } else if (type == 2) { /* ask about struct by mark */
        rightAns = elName(pEl);
        ans.push_back(rightAns);
        rEl = pEl.previousSiblingElement();
        lEl = pEl.nextSiblingElement();
        while (ans.size() < maxAns && (!rEl.isNull() || !lEl.isNull())) {
            if (!rEl.isNull()) {
                parsePixMarks(pixMarks, rEl.attribute("pixMarks"));
                QString checkMark = findMark(pixMarks, pix);
                if (checkMark != mark)
                    ans.push_back(elName(rEl));
                rEl = rEl.previousSiblingElement();
            }
            if (!lEl.isNull() && ans.size() < maxAns) {
                parsePixMarks(pixMarks, lEl.attribute("pixMarks"));
                QString checkMark = findMark(pixMarks, pix);
                if (checkMark != mark)
                    ans.push_back(elName(lEl));
                lEl = lEl.nextSiblingElement();
            }
        }
        if (m_bLangRu) {
            question = "Какое образование (структура: " + elName(parEl) + ") помечено на картинке номером " + mark + "?";
        } else {
            question = "What formation (structure: " + elName(parEl) + ") is marked on picture with number " + mark + "?";
        }
    }/* else if (type == 3) { ask about parent struct
        rightAns = elName(parEl);
        ans.push_back(rightAns);
        rEl = parEl.previousSiblingElement();
        lEl = parEl.nextSiblingElement();
        while (ans.size() < maxAns && (!rEl.isNull() || !lEl.isNull())) {
            if (!rEl.isNull()) {
                ans.push_back(elName(rEl));
                rEl = rEl.previousSiblingElement();
            }
            if (!lEl.isNull() && ans.size() < maxAns) {
                ans.push_back(elName(lEl));
                lEl = lEl.nextSiblingElement();
            }
        }
        if (m_bLangRu) {
            question = "К какой структуре принадлежит " + elName(pEl) + " (номер " + mark + " на картинке)?";
        } else {
            question = "What structure contains " + elName(pEl) + " (number " + mark + " on picture)?";
        }
    }*/

    if (ans.size() < 2) {
        genOsteoQuest();
        return;
    }
    /* set question label */
    m_pLblQuestion->setText(question);

    /* set picture */
    m_pGPix->setPixmap(":/osteoPix/osteoPix" + to_str(pix) + ".png");

    /* set answer buttons */
    std::shuffle(ans.begin(), ans.end(), dre);
    upn(j, 0, ans.size() - 1) {
        m_pLblAns[j]->setText(ans[j]);
        if (ans[j] == rightAns) {
            m_pBtnRight = m_pBtnAns[j];
        }
        m_pBtnAns[j]->show();
    }
    upn(j, ans.size(), maxAns - 1) {
        m_pBtnAns[j]->hide();
    }
    dbg_spacing -= 3; qDebug() << QString(dbg_spacing, (QChar) ' ') << "End:   " << __func__;
}
void AnatomyAsker::parsePixMarks(QVector<QPair<int, QString>>& pixVect, QString pixStr) {
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
            crash("parsePixMarks: null pix (" + num1 + ") or mark (" + num2 + ")");
        }
        pixVect.push_back({num1.toInt(), num2});
    }
}
void AnatomyAsker::processOsteoXml() {
    qDebug() << QString(dbg_spacing, (QChar) ' ') << "Begin: " << __func__; dbg_spacing += 3;
    QDomElement rootEl = osteoDoc.documentElement();
    processOsteoXmlDfs(rootEl);
    dbg_spacing -= 3; qDebug() << QString(dbg_spacing, (QChar) ' ') << "End:   " << __func__;
}
void AnatomyAsker::processOsteoXmlDfs(QDomElement& parEl) {
    if (parEl.tagName() == "cell") {
        if (parEl.hasAttribute("name") && parEl.hasAttribute("pixMarks")) {
            unusedOsteos.push_back(parEl);
        }
    }

    QDomNode curN = parEl.firstChild();
    while (!curN.isNull()) {
        QDomElement curEl = curN.toElement();
        processOsteoXmlDfs(curEl);
        curN = curN.nextSibling();
    }
}
void AnatomyAsker::readXml(QDomDocument& doc, QString path) {
    qDebug() << QString(dbg_spacing, (QChar) ' ') << "Begin: " << __func__; dbg_spacing += 3;
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
    dbg_spacing -= 3; qDebug() << QString(dbg_spacing, (QChar) ' ') << "End:   " << __func__;
}
void AnatomyAsker::viewOsteoTreeDfs(QDomElement& parEl, QTreeWidgetItem* pTWIPar) {
    QDomNode curN = parEl.firstChild();
    while (!curN.isNull()) {
        QTreeWidgetItem* pTWI = new QTreeWidgetItem;
        QDomElement curEl = curN.toElement();
        pTWI->setText(0, elName(curEl));
        pTWIPar->addChild(pTWI);
        viewOsteoTreeDfs(curEl, pTWI);
        curN = curN.nextSibling();
    }
}
void AnatomyAsker::updateGView(bool crutch) {
    int pixW = m_pGPix->pixmap().width();
    int pixH = m_pGPix->pixmap().height();

    m_pGView->setMaximumWidth(this->width());
    m_pGView->setSceneRect(QRect(0, 0, pixW, pixH));
    if (crutch && pixH >= 1000) {
        m_pGView->fitInView(QRect(0, 0, qMin(this->width(), (int)(pixW * 1.7)), qMin(this->height(), (int)(pixH* 1.7))), Qt::KeepAspectRatioByExpanding);
    }
}
void AnatomyAsker::updateInfoLabel() {
    m_pLblInfo->setText((m_bLangRu ? "Всего: " : "Summary: ") + to_str(q_cnt) + "/" +
                        to_str(q_sum) + (m_bLangRu ? "\nВерно: " : "\nCorrect: ") +
                        to_str(q_rightAnsCnt) + "/" + to_str(q_cnt));
}
void AnatomyAsker::writeXml(QDomDocument& doc, QString path) {
    qDebug() << QString(dbg_spacing, (QChar) ' ') << "Begin: " << __func__; dbg_spacing += 3;
    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toByteArray());
    } else {
        crash("File " + file.fileName() + " could not be opened in write-only mode");
    }
    file.close();
    dbg_spacing -= 3; qDebug() << QString(dbg_spacing, (QChar) ' ') << "End:   " << __func__;
}

/* PUBLIC FUNCTIONS */
AnatomyAsker::AnatomyAsker(QWidget *pwgt) : QWidget(pwgt), m_pCheckRus(new QCheckBox), m_pCheckLatin(new QCheckBox),
  m_pGPix(new QGraphicsPixmapItem), m_pGScene(new QGraphicsScene), m_pGView(new QGraphicsView),
  m_pLblQuestion(new QLabel), m_pLblInfo(new QLabel), m_pBtnNext(new QPushButton), m_pBtnFinish(new QPushButton),
  m_pBtnPre(new QPushButton), m_settings("nikich340", "AnatomyAsker"), m_pLayoutMain(new QVBoxLayout),
  m_pLayoutMenu(new QVBoxLayout), m_pLayoutPreAsk(new QVBoxLayout), m_pLayoutAsk(new QVBoxLayout),
  m_pWidgetMenu(new QWidget), m_pWidgetPreAsk(new QWidget), m_pWidgetAsk(new QWidget)
{
    qDebug() << QString(dbg_spacing, (QChar) ' ') << "Begin: " << __func__; dbg_spacing += 3;
    m_bLangRu = m_settings.value("/settings/m_bLangRu", false).toBool();
    m_bLatin = m_settings.value("/settings/m_bLatin", true).toBool();

    this->setStyleSheet("QPushButton { alignment: center; text-align: center; min-height: 75px; font-size: 20px; background-color: rgba(255,255,255,100) }"
                        "QLabel { alignment: center; text-align: center; font-size: 20px; background-color: rgba(255,255,255,100) }"
                        "QTreeWidget { alignment: center; text-align: center; font-size: 23px; background-color: rgba(255,255,255,200) }");

    m_pGScene->addItem(m_pGPix);
    m_pGView->setStyleSheet("background-color: rgba(255, 255, 255, 64)");
    m_pGView->setDragMode(QGraphicsView::ScrollHandDrag);
    m_pGView->setScene(m_pGScene);

    m_pLblQuestion->setWordWrap(true);
    m_pLblQuestion->setMaximumWidth(this->width() * 0.85);
    m_pLblQuestion->setAlignment(Qt::AlignCenter);
    m_pLblQuestion->setStyleSheet("font-size: 23px; font-weight:bold;"
                                  "color: #001a00");

    m_pLblInfo->setWordWrap(true);
    m_pLblInfo->setAlignment(Qt::AlignCenter);
    m_pLblInfo->setStyleSheet("font-size: 20px; color: #990099");
    m_pLblInfo->setMaximumWidth(this->width() * 0.15);

    upn(i, 0, 4) {
        m_pBtnSet[i] = new QPushButton;
        m_pBtnSet[i]->setStyleSheet("text-align: center; font-size: 23px; "
                                    "color: #000000; background-color: rgba(255,255,255,100)");
    }

    upn(i, 0, maxAns - 1) {
        m_pLblAns[i] = new QLabel;
        m_pBtnAns[i] = setUpBtn(m_pLblAns[i]);
        connect(m_pBtnAns[i], SIGNAL(clicked(bool)), this, SLOT(onAns()));
    }

    m_pBtnNext->setStyleSheet("color:white; background-color: rgba(0,0,255,175); min-height: 40px");
    m_pBtnFinish->setStyleSheet("color:white; background-color: rgba(0,0,0,175); min-height: 40px");
    connect(m_pBtnSet[0], SIGNAL(clicked(bool)), this, SLOT(onPreStartOsteoAsk()));
    connect(m_pBtnSet[3], SIGNAL(clicked(bool)), this, SLOT(onSettings()));
    connect(m_pCheckRus, SIGNAL(stateChanged(int)), this, SLOT(onUpdateLanguage(int)));
    connect(m_pBtnSet[4], SIGNAL(clicked(bool)), qApp, SLOT(quit()));
    upn(i, 0, 4) {
        m_pLayoutMenu->addWidget(m_pBtnSet[i]);
    }

    m_pWidgetMenu->setLayout(m_pLayoutMenu);
    m_pWidgetPreAsk->setLayout(m_pLayoutPreAsk);
    m_pWidgetAsk->setLayout(m_pLayoutAsk);

    m_pLayoutMain->addWidget(m_pWidgetMenu);
    m_pLayoutMain->addWidget(m_pWidgetPreAsk);
    m_pLayoutMain->addWidget(m_pWidgetAsk);
    this->setLayout(m_pLayoutMain);

    onUpdateLanguage(m_bLangRu ? Qt::Checked : Qt::Unchecked);
    updateInfoLabel();
    onMenu();
    dbg_spacing -= 3; qDebug() << QString(dbg_spacing, (QChar) ' ') << "End:   " << __func__;
}
AnatomyAsker::~AnatomyAsker() {
    qDebug() << QString(dbg_spacing, (QChar) ' ') << "Begin: " << __func__; dbg_spacing += 3;
    m_settings.setValue("/settings/m_bLangRu", m_bLangRu);
    m_settings.setValue("/settings/m_bLatin", m_bLatin);
    dbg_spacing -= 3; qDebug() << QString(dbg_spacing, (QChar) ' ') << "End:   " << __func__;
}

/* PUBLIC SLOTS */
void AnatomyAsker::onAns() {
    qDebug() << QString(dbg_spacing, (QChar) ' ') << "Begin: " << __func__; dbg_spacing += 3;
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
    qDebug() << QString(dbg_spacing, (QChar) ' ') << "Begin: " << __func__; dbg_spacing += 3;
    QDialog* pdlg = createDialog((m_bLangRu ? "Ваш результат: " : "Your result is: ")
                                 + to_str(q_rightAnsCnt) + "/" + to_str(q_cnt)
                                 + (m_bLangRu ? "\nЖдём Вас снова! :)" : "Waiting for you again! :)"),
                                 m_bLangRu ? "Меню" : "Menu", m_bLangRu ? "Выход" : "Quit", true);
    connect(pdlg, SIGNAL(accepted()), this, SLOT(onMenu()));
    connect(pdlg, SIGNAL(rejected()), qApp, SLOT(quit()));
    pdlg->exec();
    pdlg->deleteLater();
    q_sum = q_cnt = q_rightAnsCnt = q_ansType = 0;
    dbg_spacing -= 3; qDebug() << QString(dbg_spacing, (QChar) ' ') << "End:   " << __func__;
}
void AnatomyAsker::onFinishOsteoAsk() {
    qDebug() << QString(dbg_spacing, (QChar) ' ') << "Begin: " << __func__; dbg_spacing += 3;
    disconnect(m_pBtnNext, SIGNAL(clicked(bool)), this, SLOT(onNextOsteoAsk()));
    disconnect(m_pBtnPre, SIGNAL(clicked(bool)), this, SLOT(onStartOsteoAsk()));
    disconnect(m_pBtnFinish, SIGNAL(clicked(bool)), this, SLOT(onFinishOsteoAsk()));
    onFinishAsk();
    dbg_spacing -= 3; qDebug() << QString(dbg_spacing, (QChar) ' ') << "End:   " << __func__;
}
void AnatomyAsker::onMenu() {
    qDebug() << QString(dbg_spacing, (QChar) ' ') << "Begin: " << __func__; dbg_spacing += 3;
    m_pWidgetMenu->show();
    m_pWidgetPreAsk->hide();
    m_pWidgetAsk->hide();
    dbg_spacing -= 3; qDebug() << QString(dbg_spacing, (QChar) ' ') << "End:   " << __func__;
}
void AnatomyAsker::onNextOsteoAsk() {
    qDebug() << QString(dbg_spacing, (QChar) ' ') << "Begin: " << __func__; dbg_spacing += 3;
    q_ansType = 0;
    upn(i, 0, maxAns - 1) {
        m_pBtnAns[i]->setStyleSheet("background-color: rgba(255,255,255,70)");
    }
    genOsteoQuest();
    ++q_cnt;
    updateInfoLabel();
    updateGView(true);
    dbg_spacing -= 3; qDebug() << QString(dbg_spacing, (QChar) ' ') << "End:   " << __func__;
}
void AnatomyAsker::onPreStartOsteoAsk() {
    qDebug() << QString(dbg_spacing, (QChar) ' ') << "Begin: " << __func__; dbg_spacing += 3;
    if (!m_settings.value("/settings/launched" + to_str(VERSION), false).toBool()) {
        QDialog* pdlg = createDialog("Необходимо перезайти в приложение (первый запуск)", "OK", "-", true);
        connect(pdlg, SIGNAL(accepted()), qApp, SLOT(quit()));
        m_settings.setValue("/settings/launched" + to_str(VERSION), true);
        pdlg->exec();
        pdlg->deleteLater();
    }

    m_pWidgetMenu->hide();
    m_pWidgetPreAsk->show();
    m_pWidgetAsk->hide();

    readXml(osteoDoc, ":/osteologia.xml");
    processOsteoXml();
    if (m_pTW != nullptr) {
        delete m_pTW;
    }
    connect(m_pBtnFinish, SIGNAL(clicked(bool)), this, SLOT(onFinishOsteoAsk()));
    connect(m_pBtnPre, SIGNAL(clicked(bool)), this, SLOT(onStartOsteoAsk()));
    m_pTW = viewOsteoTree();
    m_pTW->setCurrentItem(m_pTW->topLevelItem(0));
    m_pTW->expandItem(m_pTW->currentItem());
    m_pLayoutPreAsk->addWidget(m_pTW);
    m_pLayoutPreAsk->addWidget(m_pBtnPre);
    dbg_spacing -= 3; qDebug() << QString(dbg_spacing, (QChar) ' ') << "End:   " << __func__;
}
void AnatomyAsker::onSettings() {
    qDebug() << QString(dbg_spacing, (QChar) ' ') << "Begin: " << __func__; dbg_spacing += 3;
    if (m_pDialogSettings == nullptr) {
        m_pDialogSettings = new QDialog;
        m_pDialogSettings->setWindowOpacity(0.9);
        m_pDialogSettings->setMinimumWidth(this->width() * 0.8);
        QVBoxLayout* pvbox = new QVBoxLayout;
        QPushButton* pBtnOk = new QPushButton("OK");
        m_pCheckRus->setTristate(false);
        m_pCheckLatin->setTristate(false);
        m_pDialogSettings->setModal(true);
        m_pDialogSettings->setLayout(pvbox);
        connect(pBtnOk, SIGNAL(clicked(bool)), m_pDialogSettings, SLOT(accept()));
        pvbox->addWidget(m_pCheckRus);
        pvbox->addWidget(m_pCheckLatin);
        pvbox->addWidget(pBtnOk);
    }
    m_pCheckRus->setCheckState(m_bLangRu ? Qt::Checked : Qt::Unchecked);
    m_pCheckLatin->setCheckState(m_bLatin ? Qt::Checked : Qt::Unchecked);

    m_pDialogSettings->exec();
    dbg_spacing -= 3; qDebug() << QString(dbg_spacing, (QChar) ' ') << "End:   " << __func__;
}
void AnatomyAsker::onStartAsk() {
    qDebug() << QString(dbg_spacing, (QChar) ' ') << "Begin: " << __func__; dbg_spacing += 3;
    QGridLayout* pGridLayout = new QGridLayout;
    QHBoxLayout* pHLayout = new QHBoxLayout;
    upn(i, 0, maxAns - 1) {
        pGridLayout->addWidget(m_pBtnAns[i], i / 2, i % 2);
    }
    updateInfoLabel();

    m_pGView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pGView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    updateGView(false);

    pHLayout->addWidget(m_pLblQuestion);
    pHLayout->addWidget(m_pLblInfo);
    pGridLayout->addWidget(m_pBtnFinish, maxAns / 2, 0);
    pGridLayout->addWidget(m_pBtnNext, maxAns / 2, 1);

    m_pWidgetMenu->hide();
    m_pWidgetPreAsk->hide();
    m_pWidgetAsk->show();

    m_pLayoutAsk->addLayout(pHLayout);
    m_pLayoutAsk->addWidget(m_pGView);
    m_pLayoutAsk->addLayout(pGridLayout);
    dbg_spacing -= 3; qDebug() << QString(dbg_spacing, (QChar) ' ') << "End:   " << __func__;
}
void AnatomyAsker::onStartOsteoAsk() {
    qDebug() << QString(dbg_spacing, (QChar) ' ') << "Begin: " << __func__; dbg_spacing += 3;

    chooseOsteoQuests(m_pTW->currentItem()->text(0));
    q_sum = unusedOsteos.size();
    genOsteoQuest();
    connect(m_pBtnNext, SIGNAL(clicked(bool)), this, SLOT(onNextOsteoAsk()));
    dbg_spacing -= 3; qDebug() << QString(dbg_spacing, (QChar) ' ') << "End:   " << __func__;
    onStartAsk();
}
void AnatomyAsker::onUpdateLanguage(int check) {
    qDebug() << QString(dbg_spacing, (QChar) ' ') << "Begin: " << __func__; dbg_spacing += 3;
    if (check == Qt::Checked) {
        m_bLangRu = true;
    } else {
        m_bLangRu = false;
    }
    m_pCheckLatin->setCheckable(m_bLangRu);
    m_pBtnSet[0]->setText(m_bLangRu ? "Остеология" : "Osteologia");
    m_pBtnSet[1]->setText(m_bLangRu ? "Артрология" : "Artrologia");
    m_pBtnSet[2]->setText(m_bLangRu ? "Миология" : "Myologia");
    m_pBtnSet[3]->setText(m_bLangRu ? "Настройки" : "Settings");
    m_pBtnSet[4]->setText(m_bLangRu ? "Выход" : "Quit");
    m_pBtnPre->setText(m_bLangRu ? "Начать" : "Start");
    m_pBtnNext->setText(m_bLangRu ? "Следующий вопрос" : "Next question");
    m_pBtnFinish->setText(m_bLangRu ? " Завершить" : " Finish");
    m_pCheckRus->setText(m_bLangRu ? "Русский язык" : "Russian language");
    m_pCheckLatin->setText(m_bLangRu ? "Добавлять латинские названия" : "Add latin name");
    dbg_spacing -= 3; qDebug() << QString(dbg_spacing, (QChar) ' ') << "End:   " << __func__;
}
