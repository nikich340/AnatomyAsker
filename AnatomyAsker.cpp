#include "AnatomyAsker.h"

AnatomyAsker::AnatomyAsker(QWidget *pwgt) : QWidget(pwgt), m_settings("nikich340", "AnatomyAsker") {
    this->setStyleSheet("QPushButton { alignment: center; text-align: center; min-height: 75px; font-size: 20px }"
                        "QLabel { alignment: center; text-align: center; font-size: 20px; background-color: rgba(255,255,255,100) }");

    m_pLayoutMain = new QVBoxLayout;
    m_gScene.addItem(&m_gPix);
    m_gView.setStyleSheet("background-color: rgba(255, 255, 255, 64)");
    m_gView.setDragMode(QGraphicsView::ScrollHandDrag);
    m_gView.setScene(&m_gScene);

    m_pLblQuestion = new QLabel;
    m_pLblQuestion->setWordWrap(true);
    m_pLblQuestion->setMaximumWidth(this->width() * 0.85);
    m_pLblQuestion->setAlignment(Qt::AlignCenter);
    m_pLblQuestion->setStyleSheet("font-size: 23px; font-weight:bold;"
                                  "color: #001a00");
    m_pLblInfo = new QLabel((m_langRu ? "Сейчас: " : "Now: ") + to_str(q_rightAnsCnt) + "/" + to_str(q_cnt));
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
    connect(m_pBtnSet[0], SIGNAL(clicked(bool)), this, SLOT(onStartOsteoAsk()));
    upn(i, 0, 2) {
        m_pLayoutMain->addWidget(m_pBtnSet[i]);
    }
    setLayout(m_pLayoutMain);
}
QTreeWidget* AnatomyAsker::viewOsteoTree() {
   QTreeWidget* pTW = new QTreeWidget;
   pTW->setHeaderLabel(m_langRu ? "Структура" : "Structure");
   pTW->setColumnCount(1);

   QDomElement rootEl = osteoDoc.documentElement();
   QTreeWidgetItem* pRoot = new QTreeWidgetItem;
   pRoot->setText(0, elName(rootEl));
   pTW->addTopLevelItem(pRoot);
   viewOsteoTreeDfs(rootEl, pRoot);

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
void AnatomyAsker::readOsteoXml() {
    QFile fileOsteo(":/osteologia.xml");
    if (fileOsteo.open(QIODevice::ReadOnly)) {
        if (osteoDoc.setContent(&fileOsteo)) {
            QDomElement rootEl = osteoDoc.documentElement();
            readOsteoXmlDfs(rootEl);
        }
    }
    fileOsteo.close();
}
void AnatomyAsker::readOsteoXmlDfs(QDomElement& parEl) {
    if (parEl.tagName() == "cell") {
        if (parEl.hasAttribute("name") && parEl.hasAttribute("pixMarks")) {
            unusedOsteos.push_back(&parEl);
        }
    }
    QDomNode curN = parEl.firstChild();
    while (!curN.isNull()) {
        QDomElement curEl = curN.toElement();
        readOsteoXmlDfs(curEl);
        curN = curN.nextSibling();
    }
}
/*void AnatomyAsker::writeOsteoXml() {
    QVector<bool> used(osteoDoc.size() + 5);
    QDomDocument doc;
    QDomElement rootE = doc.createElement("osteologia");
    doc.appendChild(rootE);
    for (int i = 0; i < osteoDoc.size(); ++i) {
        if (used[i])
            continue;
        rootE.appendChild(writeOsteoXmlDfs(i, doc, used));
    }
    QFile file("osteo.xml");
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream(&file) << doc.toString();
        file.close();
    }
}
QDomElement AnatomyAsker::writeOsteoXmlDfs(int v, QDomDocument& doc, QVector<bool>& used) {
    QDomElement curE;
    if (osteoDoc[v].isBone) {
        curE = doc.createElement("cell");
    } else {
        curE = doc.createElement("group");
    }
    curE.setAttribute("nameRu", m_ru[ osteoDoc[v].name ]);
    curE.setAttribute("name", osteoDoc[v].name);
    if (!osteoDoc[v].pixMark.empty()) {
        QString pixStr = to_str(osteoDoc[v].pixMark[0].first) + "-" + to_str(osteoDoc[v].pixMark[0].second);
        upn(k, 1, osteoDoc[v].pixMark.size() - 1) {
            pixStr +=  ", " + to_str(osteoDoc[v].pixMark[k].first) + "-" + to_str(osteoDoc[v].pixMark[k].second);
        }
        curE.setAttribute("pixMarks", pixStr);
    }
    for (int j = 0; j < osteoDoc[v].nextCell.size(); ++j) {
        used[ osteoDoc[v].nextCell[j] ] = true;
        curE.appendChild(writeOsteoXmlDfs(osteoDoc[v].nextCell[j], doc, used));
    }
    for (int j = 0; j < osteoDoc[v].boneFormations.size(); ++j) {
        QDomElement curF = doc.createElement("cell");
        curF.setAttribute("nameRu", m_ru[ osteoDoc[v].boneFormations[j].name ]);
        curF.setAttribute("name", osteoDoc[v].boneFormations[j].name);
        if (!osteoDoc[v].boneFormations[j].pixMark.empty()) {
            QString pixStr = to_str(osteoDoc[v].boneFormations[j].pixMark[0].first) + "-" + to_str(osteoDoc[v].boneFormations[j].pixMark[0].second);
            upn(k, 1, osteoDoc[v].boneFormations[j].pixMark.size() - 1) {
                pixStr +=  ", " + to_str(osteoDoc[v].boneFormations[j].pixMark[k].second) + "-" + to_str(osteoDoc[v].boneFormations[j].pixMark[k].second);
            }
            curF.setAttribute("pixMarks", pixStr);
        }
        curE.appendChild(curF);
    }
    return curE;
}*/
void AnatomyAsker::genOsteoQuest() {

}
void AnatomyAsker::incr(int& i, int max, int rep) {
    i += rep;
    if (i > max) {
        crash("Incrementing goes out of range (" + to_str(i) + ")");
    }
}
void AnatomyAsker::crash(QString reason) {
    qDebug() << "Crash! Reason: " << reason << "\n";
    QDialog* pdlg = createDialog("Crash! Reason: " + reason + "\n", "Quit", "Try ignore", true);
    connect(pdlg, SIGNAL(accepted()), qApp, SLOT(quit()));
    pdlg->exec();
    pdlg->deleteLater();
}
AnatomyAsker::~AnatomyAsker() {
    m_settings.setValue("/settings/launchedBefore", true);
}
void AnatomyAsker::onStartOsteoAsk() {
    readOsteoXml();

    if (!m_settings.value("/settings/launchedBefore", false).toBool()) {
        QDialog* pdlg = createDialog("Необходимо перезайти в приложение (первый запуск)", "OK", "-", true);
        connect(pdlg, SIGNAL(accepted()), qApp, SLOT(quit()));
        pdlg->exec();
        pdlg->deleteLater();
    }
    QGridLayout* pGridLayout = new QGridLayout;
    QHBoxLayout* pHLayout = new QHBoxLayout;
    upn(i, 0, maxAns - 1) {
        m_pLblAns[i] = new QLabel;
        m_pBtnAns[i] = setUpBtn(m_pLblAns[i]);
        connect(m_pBtnAns[i], SIGNAL(clicked(bool)), this, SLOT(onAns()));
        pGridLayout->addWidget(m_pBtnAns[i], i/2, i % 2);
    }
    genOsteoQuest();

    m_gView.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_gView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    updateGView(false);

    pHLayout->addWidget(m_pLblQuestion);
    pHLayout->addWidget(m_pLblInfo);
    pGridLayout->addWidget(m_pBtnFinish, 3, 0);
    pGridLayout->addWidget(m_pBtnNext, 3, 1);
    upn(i, 0, 2) {
        m_pBtnSet[i]->hide();
    }

    m_pLayoutMain->addLayout(pHLayout);
    m_pLayoutMain->addWidget(&m_gView);
    m_pLayoutMain->addLayout(pGridLayout);
    m_pLayoutMain->addWidget(viewOsteoTree());
}
void AnatomyAsker::onFinishOsteoAsk() {
    QDialog* pdlg = createDialog((m_langRu ? "Ваш результат: " : "Your result is: ")
                                 + to_str(q_rightAnsCnt) + "/" + to_str(q_cnt)
                                 + (m_langRu ? "\nЖдём Вас снова! :)" : "Waiting for you again! :)"),
                                 m_langRu ? "Выход" : "Quit", "-", true);
    connect(pdlg, SIGNAL(accepted()), qApp, SLOT(quit()));
    pdlg->exec();
    pdlg->deleteLater();
}
void AnatomyAsker::onNextOsteoAsk() {
    q_ansType = 0;
    upn(i, 0, maxAns - 1) {
        m_pBtnAns[i]->setStyleSheet("background-color: rgba(255, 255, 255, 64)");
    }
    genOsteoQuest();
    updateGView(true);
}
void AnatomyAsker::updateGView(bool crutch) {
    int pixW = m_gPix.pixmap().width();
    int pixH = m_gPix.pixmap().height();

    m_gView.setMaximumWidth(this->width());
    //m_gView.setMaximumHeight(qMin((int) (this->height() * 0.65), pixH));
    m_gView.setSceneRect(QRect(0, 0, pixW, pixH));
    if (crutch && qMax(pixW, pixH) >= 1000) {
        m_gView.fitInView(QRect(0, 0, qMin(this->width(), (int)(pixW * 1.75)), qMin(this->height(), (int)(pixH* 1.75))), Qt::KeepAspectRatioByExpanding);
    }
}
void AnatomyAsker::onAns() {
    QPushButton* pBtn = dynamic_cast<QPushButton*>(sender());
    if (m_pBtnRight == pBtn) {
        if (!q_ansType) {
            q_ansType = 1;
            ++q_cnt;
            ++q_rightAnsCnt;
        }
        pBtn->setStyleSheet("color:blue; background-color: rgba(50, 255, 50, 100); font-weight:bold");
    } else {
        if (!q_ansType) {
            q_ansType = -1;
            ++q_cnt;
            m_pLblInfo->setText("Current: " + to_str(q_rightAnsCnt) + "/" + to_str(q_cnt));
        }
        pBtn->setStyleSheet("color:white; background-color: rgba(255,0,0,100)");
    }
    m_pLblInfo->setText((m_langRu ? "Сейчас: " : "Now: ") + to_str(q_rightAnsCnt) + "/" + to_str(q_cnt));
}
