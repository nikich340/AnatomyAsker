#pragma once

#include <QtWidgets>

#define upn(x, init, n) for(int x = init; x <= n; ++x)
#define to_str(a) QString::number(a)

class AnatomyAsker : public QWidget {
Q_OBJECT
private:
    static const int maxAns = 6;
    QVBoxLayout* m_pLayoutMain;
    QPushButton *m_pBtnRight, *m_pBtnNext, *m_pBtnFinish, *m_pBtnSet[3], *m_pBtnAns[maxAns];
    QLabel *m_pLblQuestion, *m_pLblInfo, *m_pLblAns[maxAns];
    QFile m_file;
    QGraphicsPixmapItem m_gPix;
    QGraphicsScene m_gScene;
    QGraphicsView m_gView;
    QMap<QString, QString> m_ru;
    bool m_langRu = true;
    int q_cnt = 0;
    int q_rightAnsCnt = 0;
    int q_ansType = 0; // 0 - undefined, 1 - right, -1 - wrong

    struct formation {
        bool used = false;
        QString name;
        QVector<QPair<int, int>> pixMark; // number of pix, number of mark
    };
    struct cell {
        bool isBone = false;
        int parentCell;
        QString name;
        QVector<int> nextCell;
        QVector<formation> boneFormations;
        QVector<QPair<int, int>> pixMark;
    };
    QVector<cell> boneData;
    QVector<int> nusedBones;
    QVector<QPair<int, int>> nusedFormations;
    int curCell = 0;
    QSettings m_settings;

    QString lang(QString s);
    bool isDigit(QChar c);
    int rand(int L, int R);
    QPushButton* setUpBtn(QLabel* pLbl);
    QDialog* createDialog(QString info, QString accept, QString reject, bool mod);
    void readOsteo();
    void genOsteoQuest();
    void updateGView(bool crutch);
    void incr(int& i, int max, int rep = 1);
    void crash(QString reason);
public:
     QTreeWidget* viewOsteoTree();
    AnatomyAsker(QWidget *pwgt = nullptr);
    virtual ~AnatomyAsker();
public slots:
    void onStartOsteoAsk();
    void onFinishOsteoAsk();
    void onNextOsteoAsk();
    void onAns();
};
