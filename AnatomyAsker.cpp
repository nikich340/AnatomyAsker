#include "AnatomyAsker.h"

AnatomyAsker::AnatomyAsker(QWidget *pwgt) : QWidget(pwgt), m_settings("nikich340", "AnatomyAsker") {
    qDebug() << "Begin: " << __func__;
    this->setStyleSheet("QPushButton { alignment: center; text-align: center; min-height: 75px; font-size: 20px; background-color: rgba(255,255,255,200) }"
                        "QLabel { alignment: center; text-align: center; font-size: 20px; background-color: rgba(255,255,255,100) }"
                        "QTreeWidget { alignment: center; text-align: center; font-size: 23px; background-color: rgba(255,255,255,200) }");

    m_pLayoutMain = new QVBoxLayout;
    m_pGPix = new QGraphicsPixmapItem;
    m_gScene.addItem(m_pGPix);
    m_gView.setStyleSheet("background-color: rgba(255, 255, 255, 64)");
    m_gView.setDragMode(QGraphicsView::ScrollHandDrag);
    m_gView.setScene(&m_gScene);

    m_pLblQuestion = new QLabel;
    m_pLblQuestion->setWordWrap(true);
    m_pLblQuestion->setMaximumWidth(this->width() * 0.85);
    m_pLblQuestion->setAlignment(Qt::AlignCenter);
    m_pLblQuestion->setStyleSheet("font-size: 23px; font-weight:bold;"
                                  "color: #001a00");
    m_pLblInfo = new QLabel;
    updateInfoLabel();
    m_pLblInfo->setWordWrap(true);
    m_pLblInfo->setAlignment(Qt::AlignCenter);
    m_pLblInfo->setStyleSheet("font-size: 20px; color: #990099");
    m_pLblInfo->setMaximumWidth(this->width() * 0.15);

    upn(i, 0, 2) {
        m_pBtnSet[i] = new QPushButton;
        m_pBtnSet[i]->setStyleSheet("text-align: center; font-size: 23px; "
                                    "color: #000000; background-color: rgba(255,255,255,100)");
    }
    m_pBtnSet[0]->setText(m_langRu ? "Остеология" : "Osteologia");
    m_pBtnSet[1]->setText(m_langRu ? "Артрология" : "Artrologia");
    m_pBtnSet[2]->setText(m_langRu ? "Миология" : "Myologia");

    m_pBtnNext = new QPushButton(m_langRu ? "Следующий вопрос" : "Next question");
    m_pBtnNext->setStyleSheet("color:white; background-color: rgba(0,0,255,175); min-height: 40px");

    m_pBtnFinish = new QPushButton(m_langRu ? " Завершить" : " Finish");
    m_pBtnFinish->setStyleSheet("color:white; background-color: rgba(0,0,0,175); min-height: 40px");
    connect(m_pBtnFinish, SIGNAL(clicked(bool)), this, SLOT(onFinishOsteoAsk()));
    connect(m_pBtnNext, SIGNAL(clicked(bool)), this, SLOT(onNextOsteoAsk()));
    connect(m_pBtnSet[0], SIGNAL(clicked(bool)), this, SLOT(onPreStartOsteoAsk()));
    upn(i, 0, 2) {
        m_pLayoutMain->addWidget(m_pBtnSet[i]);
    }
    setLayout(m_pLayoutMain);
    qDebug() << "End: " << __func__;
}
QTreeWidget* AnatomyAsker::viewOsteoTree() {
   qDebug() << "Begin: " << __func__;
   QTreeWidget* pTW = new QTreeWidget;
   pTW->setHeaderLabel(m_langRu ? "Выберите структуру для вопросов" : "Choose structure for questions");
   pTW->setColumnCount(1);

   QDomElement rootEl = osteoDoc.documentElement();
   QTreeWidgetItem* pRoot = new QTreeWidgetItem;
   pRoot->setText(0, elName(rootEl));
   pTW->addTopLevelItem(pRoot);
   viewOsteoTreeDfs(rootEl, pRoot);

   qDebug() << "End: " << __func__;
   return pTW;
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
QString AnatomyAsker::elName(QDomElement& curEl) {
    return (m_langRu ? (curEl.attribute("nameRu") + " [" + curEl.attribute("name") + "]") : curEl.attribute("name"));
}
int AnatomyAsker::rand(int L, int R) {
    qsrand(QDateTime::currentMSecsSinceEpoch());
    if (R < L)
        return -1;
    return L + qrand() % (R - L + 1);
}
bool AnatomyAsker::isDigit(QChar c) {
    return (c >= '0' && c <= '9');
}
bool AnatomyAsker::isUpper(QChar c) {
    return (c >= 'A' && c <= 'Z');
}
QPushButton* AnatomyAsker::setUpBtn(QLabel* pLbl) {
    QPushButton* pBtn = new QPushButton;
    QHBoxLayout *pLayout = new QHBoxLayout;
    pLbl->setWordWrap(true);
    pLbl->setAlignment(Qt::AlignCenter);
    pLayout->addWidget(pLbl);
    pBtn->setLayout(pLayout);
    pBtn->setStyleSheet("background-color: rgba(255, 255, 255, 64)");
    return pBtn;
}
QDialog* AnatomyAsker::createDialog(QString info, QString accept, QString reject, bool mod) {
    QDialog* pdlg = new QDialog(this);
    pdlg->setWindowOpacity(0.8);
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
void AnatomyAsker::readXml(QDomDocument& doc, QString path) {
    qDebug() << "Begin: " << __func__;
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
    qDebug() << "End: " << __func__;
}
void AnatomyAsker::writeXml(QDomDocument& doc, QString path) {
    qDebug() << "Begin: " << __func__;
    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toByteArray());
    } else {
        crash("File " + file.fileName() + " could not be opened in write-only mode");
    }
    file.close();
    qDebug() << "End: " << __func__;
}
void AnatomyAsker::processOsteoXml() {
    qDebug() << "Begin: " << __func__;
    QDomElement rootEl = osteoDoc.documentElement();
    processOsteoXmlDfs(rootEl);
    qDebug() << "End: " << __func__;
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
QString AnatomyAsker::findMark(QVector<QPair<int, QString>>& pixVect, int pixNum) {
    for (auto &it: pixVect) {
        if (it.first == pixNum) {
            return it.second;
        }
    }
    return "-1";
}
void AnatomyAsker::chooseOsteoQuests(QString rootPattern) {
    qDebug() << "Begin: " << __func__;
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
    qDebug() << "End: " << __func__;
}
void AnatomyAsker::genOsteoQuest() {
    qDebug() << "Begin: " << __func__;
    std::default_random_engine dre(QDateTime::currentMSecsSinceEpoch());
    QVector<QString> ans;
    QString rightAns;
    QString question;
    if (unusedOsteos.empty()) {
        qDebug() << "Empty quest array";
        onFinishOsteoAsk();
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
        if (m_langRu) {
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
        if (m_langRu) {
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
        if (m_langRu) {
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
    qDebug() << "End: " << __func__;
}
void AnatomyAsker::crash(QString reason) {
    qDebug() << "Crash! Reason: " << reason << "\n";
    QDialog* pdlg = createDialog("Crash! Reason: " + reason + "\n", "Quit", "Try ignore", true);
    connect(pdlg, SIGNAL(accepted()), qApp, SLOT(quit()));
    pdlg->exec();
    pdlg->deleteLater();
}
AnatomyAsker::~AnatomyAsker() {
}
void AnatomyAsker::onPreStartOsteoAsk() {
    qDebug() << "Begin: " << __func__;
    if (!m_settings.value("/settings/launched" + to_str(VERSION), false).toBool()) {
        QDialog* pdlg = createDialog("Необходимо перезайти в приложение (первый запуск)", "OK", "-", true);
        connect(pdlg, SIGNAL(accepted()), qApp, SLOT(quit()));
        m_settings.setValue("/settings/launched" + to_str(VERSION), true);
        pdlg->exec();
        pdlg->deleteLater();
    }

    readXml(osteoDoc, ":/osteologia.xml");
    processOsteoXml();

    upn(i, 0, 2) {
        m_pBtnSet[i]->hide();
    }
    m_pBtnPre = new QPushButton(m_langRu ? "Начать" : "Start");
    m_pTW = viewOsteoTree();
    m_pTW->setCurrentItem(m_pTW->topLevelItem(0));
    m_pTW->expandItem(m_pTW->currentItem());
    m_pLayoutMain->addWidget(m_pTW);
    m_pLayoutMain->addWidget(m_pBtnPre);
    connect(m_pBtnPre, SIGNAL(clicked(bool)), this, SLOT(onStartOsteoAsk()));
    qDebug() << "End: " << __func__;
}
void AnatomyAsker::onStartOsteoAsk() {
    qDebug() << "Begin: " << __func__;
    QGridLayout* pGridLayout = new QGridLayout;
    QHBoxLayout* pHLayout = new QHBoxLayout;
    upn(i, 0, maxAns - 1) {
        m_pLblAns[i] = new QLabel;
        m_pBtnAns[i] = setUpBtn(m_pLblAns[i]);
        connect(m_pBtnAns[i], SIGNAL(clicked(bool)), this, SLOT(onAns()));
        pGridLayout->addWidget(m_pBtnAns[i], i/2, i % 2);
    }
    chooseOsteoQuests(m_pTW->currentItem()->text(0));
    q_sum = unusedOsteos.size();
    updateInfoLabel();
    genOsteoQuest();

    m_gView.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_gView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    updateGView(false);

    pHLayout->addWidget(m_pLblQuestion);
    pHLayout->addWidget(m_pLblInfo);
    pGridLayout->addWidget(m_pBtnFinish, 3, 0);
    pGridLayout->addWidget(m_pBtnNext, 3, 1);

    m_pTW->hide();
    m_pBtnPre->hide();

    m_pLayoutMain->addLayout(pHLayout);
    m_pLayoutMain->addWidget(&m_gView);
    m_pLayoutMain->addLayout(pGridLayout);
    qDebug() << "End: " << __func__;
}
void AnatomyAsker::onFinishOsteoAsk() {
    qDebug() << "Begin: " << __func__;
    QDialog* pdlg = createDialog((m_langRu ? "Ваш результат: " : "Your result is: ")
                                 + to_str(q_rightAnsCnt) + "/" + to_str(q_cnt)
                                 + (m_langRu ? "\nЖдём Вас снова! :)" : "Waiting for you again! :)"),
                                 m_langRu ? "Выход" : "Quit", "-", true);
    connect(pdlg, SIGNAL(accepted()), qApp, SLOT(quit()));
    pdlg->exec();
    pdlg->deleteLater();
    qDebug() << "End: " << __func__;
}
void AnatomyAsker::onNextOsteoAsk() {
    q_ansType = 0;
    updateInfoLabel();
    upn(i, 0, maxAns - 1) {
        m_pBtnAns[i]->setStyleSheet("background-color: rgba(255, 255, 255, 64)");
    }
    genOsteoQuest();
    ++q_cnt;
    updateInfoLabel();
    updateGView(true);
}
void AnatomyAsker::updateGView(bool crutch) {
    int pixW = m_pGPix->pixmap().width();
    int pixH = m_pGPix->pixmap().height();

    m_gView.setMaximumWidth(this->width());
    //m_gView.setMaximumHeight(qMin((int) (this->height() * 0.65), pixH));
    m_gView.setSceneRect(QRect(0, 0, pixW, pixH));
    if (crutch && pixH >= 1000) {
        m_gView.fitInView(QRect(0, 0, qMin(this->width(), (int)(pixW * 1.7)), qMin(this->height(), (int)(pixH* 1.7))), Qt::KeepAspectRatioByExpanding);
    }
}
void AnatomyAsker::updateInfoLabel() {
    m_pLblInfo->setText((m_langRu ? "Всего: " : "Summary: ") + to_str(q_cnt) + "/" +
                        to_str(q_sum) + (m_langRu ? "\nВерно: " : "\nCorrect: ") +
                        to_str(q_rightAnsCnt) + "/" + to_str(q_cnt));
}
void AnatomyAsker::onAns() {
    QPushButton* pBtn = dynamic_cast<QPushButton*>(sender());
    if (m_pBtnRight == pBtn) {
        if (!q_ansType) {
            q_ansType = 1;
            ++q_rightAnsCnt;
        }
        pBtn->setStyleSheet("color:blue; background-color: rgba(50, 255, 50, 100); font-weight:bold");
    } else {
        if (!q_ansType) {
            q_ansType = -1;
        }
        pBtn->setStyleSheet("color:white; background-color: rgba(255,0,0,100)");
    }
    updateInfoLabel();
}
