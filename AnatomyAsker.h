#pragma once

#include <QtWidgets>
#include <QtXml>

#define upn(x, init, n) for(int x = init; x <= n; ++x)
#define to_str(a) QString::number(a)
#define VERSION 2.0

class AnatomyAsker : public QWidget {
Q_OBJECT
private:
    static const int maxAns = 6;
    QSettings m_settings;
    QVBoxLayout* m_pLayoutMain;
    QPushButton *m_pBtnRight, *m_pBtnNext, *m_pBtnFinish, *m_pBtnSet[3], *m_pBtnAns[maxAns];
    QLabel *m_pLblQuestion, *m_pLblInfo, *m_pLblAns[maxAns];
    QGraphicsPixmapItem m_gPix;
    QGraphicsScene m_gScene;
    QGraphicsView m_gView;
    bool m_langRu = true;
    int q_sum = 5;
    int q_cnt = 1;
    int q_rightAnsCnt = 0;
    int q_ansType = 0; // 0 - undefined, 1 - right, -1 - wrong

    QDomDocument osteoDoc;
    QVector<QDomElement> unusedOsteos;

    QString elName(QDomElement& curEl);
    bool isDigit(QChar c);
    int rand(int L, int R);
    QPushButton* setUpBtn(QLabel* pLbl);
    QDialog* createDialog(QString info, QString accept, QString reject, bool mod);
    QTreeWidget* viewOsteoTree();
    int findMark(QVector<QPair<int, int>>& pixVect, int pixNum);
    void parsePixMarks(QVector<QPair<int, int>>& pixVect, QString pixStr);
    void readOsteoXml();
    void readOsteoXmlDfs(QDomElement& curEl);
    void genOsteoQuest();
    void updateGView(bool crutch);
    void crash(QString reason);
    void viewOsteoTreeDfs(QDomElement& parEl, QTreeWidgetItem* pTWI);
    void updateInfoLabel();
public:
    AnatomyAsker(QWidget *pwgt = nullptr);
    virtual ~AnatomyAsker();
public slots:
    void onStartOsteoAsk();
    void onFinishOsteoAsk();
    void onNextOsteoAsk();
    void onAns();
};
