#pragma once

#include <QtWidgets>
#include <QtXml>

#define upn(x, init, n) for(int x = init; x <= n; ++x)
#define to_str(a) QString::number(a)
#define VERSION 2.1

class AnatomyAsker : public QWidget {
Q_OBJECT
private:
    static const int maxAns = 6;
    QSettings m_settings;
    QVBoxLayout* m_pLayoutMain;
    QPushButton *m_pBtnRight, *m_pBtnNext, *m_pBtnFinish, *m_pBtnPre, *m_pBtnSet[3], *m_pBtnAns[maxAns];
    QLabel *m_pLblQuestion, *m_pLblInfo, *m_pLblAns[maxAns];
    QGraphicsPixmapItem* m_pGPix;
    QGraphicsScene m_gScene;
    QGraphicsView m_gView;
    QTreeWidget* m_pTW;
    bool m_langRu = true;
    int q_sum = 0;
    int q_cnt = 1;
    int q_rightAnsCnt = 0;
    int q_ansType = 0; // 0 - undefined, 1 - right, -1 - wrong

    QDomDocument osteoDoc;
    QVector<QDomElement> unusedOsteos;

    QString elName(QDomElement& curEl);
    bool isDigit(QChar c);
    bool isUpper(QChar c);
    int rand(int L, int R);
    QPushButton* setUpBtn(QLabel* pLbl);
    QDialog* createDialog(QString info, QString accept, QString reject, bool mod);
    QTreeWidget* viewOsteoTree();
    QString findMark(QVector<QPair<int, QString>>& pixVect, int pixNum);
    void parsePixMarks(QVector<QPair<int, QString>>& pixVect, QString pixStr);
    void processOsteoXml();
    void processOsteoXmlDfs(QDomElement& curEl);
    void readXml(QDomDocument& doc, QString path);
    void writeXml(QDomDocument& doc, QString path);
    void chooseOsteoQuests(QString root);
    void genOsteoQuest();
    void updateGView(bool crutch);
    void crash(QString reason);
    void viewOsteoTreeDfs(QDomElement& parEl, QTreeWidgetItem* pTWI);
    void updateInfoLabel();
public:
    AnatomyAsker(QWidget *pwgt = nullptr);
    virtual ~AnatomyAsker();
public slots:
    void onPreStartOsteoAsk();
    void onStartOsteoAsk();
    void onFinishOsteoAsk();
    void onNextOsteoAsk();
    void onAns();
};
