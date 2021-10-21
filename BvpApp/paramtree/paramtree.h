#ifndef PARAMTREE_H
#define PARAMTREE_H

#include <QFontDatabase>
#include <QMap>
#include <QTreeWidget>
#include <QWidget>

#include <bvpCommon/param.h>

class TParamTree : public QTreeWidget
{
public:
    TParamTree(QWidget* parent = nullptr);
    void updateParameters();

    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);

private:
    QMap<BVP::param_t, QTreeWidgetItem*> mapItems;

    void crtGroupInternal();
    void crtGroupParamGlobal();
    void crtGroupTime();
    void crtGroupError();
    void crtGroupErrorRemote();
    void crtGroupVp();

    void crtItem(QTreeWidgetItem* top, BVP::param_t param, QString name);

    void updateParameter(BVP::param_t param);
};

#endif  // PARAMTREE_H
