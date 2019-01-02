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
   pTW->setColumnCount(2);
   QStringList lst;
   if (m_langRu) {
       lst << "Структура" << "Образование";
   } else {
       lst << "Structure" << "Formation";
   }
   pTW->setHeaderLabels(lst);
   QMap<QString, QTreeWidgetItem*> map;
   for (int i = 1; i < boneData.size(); ++i) {
        int par = boneData[i].parentCell;
        QTreeWidgetItem* pTWI = new QTreeWidgetItem;
        if (map.find(boneData[par].name) != map.end()) {
            map[boneData[par].name]->addChild(pTWI);
        } else {
            pTW->addTopLevelItem(pTWI);
        }
        map[boneData[i].name] = pTWI;
        pTWI->setText(0, lang(boneData[i].name));
        if (boneData[i].isBone) {
            QString forms = "";
            for(auto it : boneData[i].boneFormations) {
                forms += it.name + "\n";
            }
            pTWI->setText(1, forms);
        }
    }
   return pTW;
}
QString AnatomyAsker::lang(QString s) {
    if (m_langRu && m_ru.find(s) != m_ru.end())
        return (m_ru[s] + " [" + s + "]");
    else
        return s;
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
void AnatomyAsker::readOsteo() {
    QString tmp;
    cell init;
    boneData.push_back(init);
    int cellIdx = 0;
    bool isFormation = false;
    while (!m_file.atEnd()) {
        tmp = m_file.readLine();
        //qDebug() << "ReadLine: " << tmp << "\n";
        int i = 0;
        while (tmp[i] == '\t' || tmp[i] == ' ')
            incr(i, tmp.length() - 1);
        if (tmp[i] == '\n')
            continue;
        else if (tmp[i] == ']' || tmp[i] == '}') {
            cellIdx = boneData[cellIdx].parentCell;
            isFormation = false;
        } else if (isFormation) {
            QString fName = "";
            while (tmp[i] != '/') {
                fName.push_back(tmp[i]);
                incr(i, tmp.length() - 1);
            }

            formation tempForm;
            tempForm.name = fName;

            fName = "";
            incr(i, tmp.length() - 1);
            while (tmp[i] != ':' && tmp[i] != ';') {
                fName.push_back(tmp[i]);
                incr(i, tmp.length() - 1);
            }
            m_ru[tempForm.name] = fName;

            while (tmp[i] != ';') {
                QString tmpNum1 = "", tmpNum2 = "";
                incr(i, tmp.length() - 1, 2);
                while (isDigit(tmp[i])) {
                    tmpNum1 += tmp[i];
                    incr(i, tmp.length() - 1);
                }
                incr(i, tmp.length() - 1);
                while (isDigit(tmp[i])) {
                    tmpNum2 += tmp[i];
                    incr(i, tmp.length() - 1);
                }
                tempForm.pixMark.push_back({tmpNum1.toInt(), tmpNum2.toInt()});
            }

            boneData[cellIdx].boneFormations.push_back(tempForm);
            if (!tempForm.pixMark.empty())
                nusedFormations.push_back({cellIdx, boneData[cellIdx].boneFormations.size() - 1});
        } else {
            cell tmpCell;
            if (tmp[i] == '!') {
                incr(i, tmp.length() - 1);
                tmpCell.isBone = true;
                isFormation = true;
            }
            QString cName = "";
            while (tmp[i] != '/') {
                cName.push_back(tmp[i]);
                incr(i, tmp.length() - 1);
            }
            incr(i, tmp.length() - 1);
            tmpCell.name = cName;
            tmpCell.parentCell = cellIdx;

            cName = "";
            while (tmp[i] != ':' && tmp[i] != ';') {
                cName.push_back(tmp[i]);
                incr(i, tmp.length() - 1);
            }
            m_ru[tmpCell.name] = cName;

            while (tmp[i] != ';') {
                QString tmpNum1 = "", tmpNum2 = "";
                incr(i, tmp.length() - 1, 2);
                while (isDigit(tmp[i])) {
                    tmpNum1 += tmp[i];
                    incr(i, tmp.length() - 1);
                }
                incr(i, tmp.length() - 1);
                while (isDigit(tmp[i])) {
                    tmpNum2 += tmp[i];
                    incr(i, tmp.length() - 1);
                }
                tmpCell.pixMark.push_back({tmpNum1.toInt(), tmpNum2.toInt()});
            }

            boneData.push_back(tmpCell);
            cellIdx = boneData.size() - 1;
            boneData[tmpCell.parentCell].nextCell.push_back(cellIdx);
            if (tmpCell.isBone && !tmpCell.pixMark.empty()) {
                nusedBones.push_back(cellIdx);
			}
        }
    }
    qDebug() << "Read boneData completed!";
}
void AnatomyAsker::genOsteoQuest() {
    std::default_random_engine dre(QDateTime::currentMSecsSinceEpoch());
    QVector<QString> ans;
    QString rightAns;
    QString question;
    int pixIdx = 0;
    if (!nusedBones.empty() && rand(0, 1)) {
        int idx = rand(0, nusedBones.size() - 1);
        int cIdx = nusedBones[idx];

        int pixvIdx = rand(0, boneData[cIdx].pixMark.size() - 1);
        pixIdx = boneData[cIdx].pixMark[pixvIdx].first;
        int markIdx = boneData[cIdx].pixMark[pixvIdx].second;
        int from = cIdx;
        cIdx = boneData[cIdx].parentCell;
        if (rand(0, 1)) {
            rightAns = to_str(markIdx);
            ans.push_back(rightAns);
            for (auto cur : boneData[cIdx].nextCell) {
                if (cur == from)
                   continue;
                for (auto pairpix: boneData[cur].pixMark) {
                    if (pairpix.first == pixIdx && pairpix.second != markIdx) {
                        ans.push_back(to_str(pairpix.second));
                        break;
                    }
                }
                if (ans.size() >= maxAns) {
                    break;
                }
            }
            if (m_langRu) {
                question = "Каким номером отмечен(а/о) " + lang(boneData[from].name) + " ?";
            } else {
                question = "What number is " + boneData[from].name + " marked with?";
            }
        } else {
            rightAns = boneData[from].name;
            ans.push_back(rightAns);
            for (auto cur : boneData[cIdx].nextCell) {
                if (cur == from || boneData[cur].name == rightAns)
                   continue;
                ans.push_back(boneData[cur].name);
                if (ans.size() >= maxAns) {
                    break;
                }
            }
            if (m_langRu) {
                question = "Что отмечено на картинке номером " + to_str(markIdx) + "?";
            } else {
                question = "What is marked on picture with number " + to_str(markIdx) + "?";
            }
        }
        nusedBones.erase(nusedBones.begin() + idx);
    } else if (!nusedFormations.empty()) {
        int idx = rand(0, nusedFormations.size() - 1);
        int cIdx = nusedFormations[idx].first;
        int fIdx = nusedFormations[idx].second;
        int pixvIdx = rand(0, boneData[cIdx].boneFormations[fIdx].pixMark.size() - 1);
        pixIdx = boneData[cIdx].boneFormations[fIdx].pixMark[pixvIdx].first;
        int markIdx = boneData[cIdx].boneFormations[fIdx].pixMark[pixvIdx].second;
        //qDebug("idx %d, cIdx %d, fIdx %d, pixvIdx %d, pixIdx %d, markIdx %d", idx, cIdx,
        //       fIdx, pixvIdx, pixIdx, markIdx);
        //qDebug("bone name %s, form name %s", boneData[cIdx].name.toStdString().c_str(),
        //       boneData[cIdx].boneFormations[fIdx].name.toStdString().c_str());

        int from = cIdx;
        if (rand(0, 1)) {
            rightAns = to_str(markIdx);
            ans.push_back(rightAns);
            for (auto i : boneData[from].boneFormations) {
                if (i.name == boneData[from].boneFormations[fIdx].name)
                    continue;
                for (auto j : i.pixMark) {
                    if (j.first == pixIdx && j.second != markIdx) {
                        ans.push_back(to_str(j.second));
                        break;
                    }
                }
                if (ans.size() >= maxAns)
                    break;
            }
            if (m_langRu) {
                question = "Каким номером обозначен(а/о) " + lang(boneData[from].boneFormations[fIdx].name) +
                        " (объект: " + lang(boneData[from].name) + ")?";
            } else {
                question = "What number is " + boneData[from].boneFormations[fIdx].name +
                        " (object: " + boneData[from].name + ") marked with?";
            }
        } else {
            rightAns = boneData[from].boneFormations[fIdx].name;
            ans.push_back(rightAns);
            for (auto i : boneData[from].boneFormations) {
                if (i.name == boneData[from].boneFormations[fIdx].name)
                    continue;
                ans.push_back(i.name);
                if (ans.size() >= maxAns)
                    break;
            }
            if (m_langRu) {
                question = "Какое образование (объект: " + lang(boneData[from].name) +
                        ") помечено на картинке номером " + to_str(markIdx) + "?";
            } else {
                question = "What " + boneData[from].name + " formation is marked on picture with number " +
                        to_str(markIdx) + "?";
            }
        }
        nusedFormations.erase(nusedFormations.begin() + idx);
    } else {
        onFinishOsteoAsk();
    }

    if (ans.size() < 2) {
        genOsteoQuest();
        return;
    }
    /* set question label */
    m_pLblQuestion->setText(question);

    /* set picture */
    m_gPix.setPixmap(":/osteoPix/osteoPix" + to_str(pixIdx) + ".png");

    /* set answer buttons */
    std::shuffle(ans.begin(), ans.end(), dre);
    upn(j, 0, ans.size() - 1) {
        m_pLblAns[j]->setText(lang(ans[j]));
        if (ans[j] == rightAns) {
            m_pBtnRight = m_pBtnAns[j];
        }
        m_pBtnAns[j]->show();
    }
    upn(j, ans.size(), maxAns - 1) {
        m_pBtnAns[j]->hide();
    }
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
    m_file.setFileName(":/osteologia.dat");
    if (!m_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        crash("File " + m_file.fileName() + " could not be opened");
    }
    readOsteo();
    m_file.close();

    if (!m_settings.value("/settings/launchedBefore", false).toBool()) {
        QDialog* pdlg = createDialog("Необходимо перезайти в приложение (первый запуск)", "OK", "-", true);
        connect(pdlg, SIGNAL(accepted()), qApp, SLOT(quit()));
        pdlg->exec();
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
    //m_pHLayoutMain->addWidget(viewOsteoTree());
}
void AnatomyAsker::onFinishOsteoAsk() {
    QDialog* pdlg = createDialog((m_langRu ? "Ваш результат: " : "Your result is: ")
                                 + to_str(q_rightAnsCnt) + "/" + to_str(q_cnt)
                                 + (m_langRu ? "\nЖдём Вас снова! :)" : "Waiting for you again! :)"),
                                 m_langRu ? "Выход" : "Quit", "-", true);
    connect(pdlg, SIGNAL(accepted()), qApp, SLOT(quit()));
    pdlg->exec();
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
