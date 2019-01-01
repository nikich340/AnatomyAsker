#include "AnatomyAsker.h"

AnatomyAsker::AnatomyAsker(QWidget *pwgt) : QWidget(pwgt), m_settings("nikich340", "AnatomyAsker") {
    this->setStyleSheet("QPushButton { text-align:center; min-height: 80px; font-size: 20px }"
                        "QLabel { text-align:top; font-size: 20px }");
    m_file.setFileName(":/osteologia.dat");
    if (!m_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        crash("File " + m_file.fileName() + " could not be opened");
    }
    readOsteo();
    m_file.close();

    m_gScene.addItem(&m_gPix);
    m_gView.setBackgroundRole(QPalette::Dark);
    m_gView.setDragMode(QGraphicsView::ScrollHandDrag);
    m_gView.setScene(&m_gScene);

    upn(i, 0, 2) {
        m_pBtnSet[i] = new QPushButton;
    }
    m_pBtnSet[0]->setText(m_langRu ? "Остеология" : "Osteologia");
    m_pBtnSet[1]->setText(m_langRu ? "Артрология" : "Artrologia");
    m_pBtnSet[2]->setText(m_langRu ? "Миология" : "Myologia");
    m_pLblNext = new QLabel;
    m_pBtnNext = setUpBtn(m_pLblNext);
    m_pLblNext->setText(m_langRu ? "Следующий вопрос" : "Next question");
    connect(m_pBtnNext, SIGNAL(clicked(bool)), this, SLOT(onNextOsteoAsk()));
    connect(m_pBtnSet[0], SIGNAL(clicked(bool)), this, SLOT(onStartOsteoAsk()));
    upn(i, 0, 2) {
        m_pvbMain.addWidget(m_pBtnSet[i]);
    }
    setLayout(&m_pvbMain);
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
    //pLbl->setTextInteractionFlags(Qt::NoTextInteraction);
   // pLbl->setMouseTracking(false);
    //pLbl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pLayout->addWidget(pLbl);
    pBtn->setText("");
    pBtn->setLayout(pLayout);
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
            rightAns = QString::number(markIdx);
            ans.push_back(rightAns);
            for (auto cur : boneData[cIdx].nextCell) {
                if (cur == from)
                   continue;
                for (auto pairpix: boneData[cur].pixMark) {
                    if (pairpix.first == pixIdx && pairpix.second != markIdx) {
                        ans.push_back(QString::number(pairpix.second));
                        break;
                    }
                }
                if (ans.size() >= maxAns) {
                    break;
                }
            }
            if (m_langRu) {
                question = lang(boneData[from].name) + " каким номером отмечен(а/о)?";
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
                question = "Какая кость отмечена на картинке номером " +
                    QString::number(markIdx) + "?";
            } else {
                question = "What bone is marked on picture with number " +
                    QString::number(markIdx) + "?";
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
            rightAns = QString::number(markIdx);
            ans.push_back(rightAns);
            for (auto i : boneData[from].boneFormations) {
                if (i.name == boneData[from].boneFormations[fIdx].name)
                    continue;
                for (auto j : i.pixMark) {
                    if (j.first == pixIdx && j.second != markIdx) {
                        ans.push_back(QString::number(j.second));
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
                        ") помечено на картинке номером " + QString::number(markIdx) + "?";
            } else {
                question = "What " + boneData[from].name + " formation is marked on picture with number " +
                        QString::number(markIdx) + "?";
            }
        }
        nusedFormations.erase(nusedFormations.begin() + idx);
    } else {
        crash("All bones and formations were used...");
    }

    /* set question label */
    m_lblText.setText(question);

    /* set picture */
    m_gPix.setPixmap(":/osteoPix/osteoPix" + QString::number(pixIdx) + ".png");

    /* set answer buttons */
    std::shuffle(ans.begin(), ans.end(), dre);
    upn(j, 0, ans.size() - 1) {
        m_pLblAns[j]->setText(lang(ans[j]));
        if (ans[j] == rightAns) {
            connect(m_pBtnAns[j], SIGNAL(clicked(bool)), this, SLOT(onRightAns()));
        } else {
            connect(m_pBtnAns[j], SIGNAL(clicked(bool)), this, SLOT(onWrongAns()));
        }
    }
    upn(j, 0, ans.size() - 1) {
        m_pBtnAns[j]->show();
    }
    upn(j, ans.size(), maxAns - 1) {
        m_pBtnAns[j]->hide();
    }
}
void AnatomyAsker::incr(int& i, int max, int rep) {
    i += rep;
    if (i > max) {
        crash("Incrementing goes out of range (" + QString::number(i) + ")");
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
    if (!m_settings.value("/settings/launchedBefore", false).toBool()) {
        QDialog* pdlg = createDialog("Необходимо перезайти в приложение (первый запуск)", "OK", "-", true);
        connect(pdlg, SIGNAL(accepted()), qApp, SLOT(quit()));
        pdlg->exec();
    }
    QHBoxLayout* phb = new QHBoxLayout;
    QVBoxLayout* pvb = new QVBoxLayout;
    upn(i, 0, maxAns - 1) {
        m_pLblAns[i] = new QLabel;
        m_pBtnAns[i] = setUpBtn(m_pLblAns[i]);
        pvb->addWidget(m_pBtnAns[i]);
    }
    genOsteoQuest();
    m_gView.setMaximumWidth(qMin((int) (this->width() * 0.65), m_gPix.pixmap().width()));
    m_gView.setMaximumHeight(qMin((int) (this->height() * 0.85), m_gPix.pixmap().height()));
    m_lblText.setMaximumWidth(QApplication::desktop()->width());
    m_lblText.setWordWrap(true);
    pvb->addWidget(m_pBtnNext);
    upn(i, 0, 2) {
        m_pBtnSet[i]->hide();
    }

    phb->addWidget(&m_gView);
    phb->addLayout(pvb);
    //m_pvbMain.addWidget(viewOsteoTree());
    m_pvbMain.addWidget(&m_lblText);
    m_pvbMain.addLayout(phb);
}
void AnatomyAsker::onNextOsteoAsk() {
    upn(i, 0, maxAns - 1) {
        m_pBtnAns[i]->setStyleSheet("");
    }
    genOsteoQuest();
    m_gView.setMaximumWidth(qMin((int) (this->width() * 0.65), m_gPix.pixmap().width()));
    m_gView.setMaximumHeight(qMin((int) (this->height() * 0.85), m_gPix.pixmap().height()));
}
void AnatomyAsker::onRightAns() {
    QPushButton* pBtn = dynamic_cast<QPushButton*>(sender());
    pBtn->setStyleSheet("color:white;"
                        "background-color:#33cc33;"
                        "font-weight:bold");
}
void AnatomyAsker::onWrongAns() {
    QPushButton* pBtn = dynamic_cast<QPushButton*>(sender());
    pBtn->setStyleSheet("color:white;"
                        "background-color:#ff0000");
}
